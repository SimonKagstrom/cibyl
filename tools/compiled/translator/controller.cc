/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      controller.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Singleton class that controls everything
 *
 * $Id:$
 *
 ********************************************************************/
#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#include <utils.h>
#include <emit.hh>
#include <controller.hh>
#include <registerallocator.hh>
#include <syscall-wrappers.hh>
#include <config.hh>

Controller::Controller(const char *dstdir, const char *elf_filename,
                       int n_dbs, const char **database_filenames) : CodeBlock()
{
  this->elf = new CibylElf(elf_filename);
  elf = this->elf;

  this->syscall_db_table = ght_create(1024);
  this->syscall_used_table = ght_create(1024);

  this->dstdir = dstdir;
  this->instructions = NULL;
  this->functions = NULL;
  this->methods = NULL;
  this->classes = NULL;
  this->n_syscall_dirs = 0;
  this->syscall_dirs = NULL;

  for (int i = 0; i < n_dbs; i++)
    this->readSyscallDatabase(database_filenames[i]);

  this->builtins = new BuiltinFactory();

  emit->setOutputFile(open_file_in_dir(this->dstdir, "Cibyl.j", "w"));
}

char *Controller::resolveStrtabAddress(char *strtab, char *offset)
{
  return strtab + be32_to_host((int)offset);
}

void Controller::readSyscallDatabase(const char *filename)
{
  cibyl_db_entry_t *syscall_entries;
  file_chunk_t *file = read_file(filename);
  int first_syscall_dir = this->n_syscall_dirs;
  uint32_t magic, n_dirs, n, args_start;
  char *strtab;

  magic = be32_to_host(((uint32_t*)file->data)[0]);
  n_dirs = be32_to_host(((uint32_t*)file->data)[1]);
  n = be32_to_host(((uint32_t*)file->data)[2]);
  args_start = (uint32_t)file->data +
    be32_to_host((((uint32_t*)file->data)[3]));
  strtab = (char*)file->data +
    be32_to_host((((uint32_t*)file->data)[4]));

  /* Add to the syscall directories */
  this->n_syscall_dirs += n_dirs;
  this->syscall_dirs = (char**)xrealloc(this->syscall_dirs, this->n_syscall_dirs * sizeof(char**));
  for (int i = first_syscall_dir; i < this->n_syscall_dirs; i++)
      this->syscall_dirs[i] = ((char*)strtab) + be32_to_host(((uint32_t*)file->data)[5 + i - first_syscall_dir]);

  syscall_entries = (cibyl_db_entry_t*)(((uint32_t*)file->data) +
                                        5 + n_dirs);

  /* Fixup the entries */
  for (uint32_t i = 0; i < n; i++)
    {
      cibyl_db_entry_t *cur = &syscall_entries[i];

      cur->nr = be32_to_host(cur->nr);
      cur->returns = be32_to_host(cur->returns);
      cur->nrArgs = be32_to_host(cur->nrArgs);
      cur->qualifier = be32_to_host(cur->qualifier);
      cur->name = strtab + be32_to_host((uint32_t)cur->name);
      cur->javaClass = strtab + be32_to_host((uint32_t)cur->javaClass);
      cur->javaMethod = strtab + be32_to_host((uint32_t)cur->javaMethod);
      cur->args = (cibyl_db_arg_t*)(args_start + be32_to_host((uint32_t)cur->args));
      cur->user = first_syscall_dir;

      for (unsigned int j = 0; j < cur->nrArgs; j++)
        {
          int jt_offs = be32_to_host((uint32_t)cur->args[j].javaType) & 0x00ffffff;
          cur->args[j].javaType = (char*)(strtab + jt_offs);
          cur->args[j].name = (char*)(strtab + be32_to_host((uint32_t)cur->args[j].name));
        }

      /* Add to the hash table */
      ght_insert(this->syscall_db_table, (void*)cur,
                 strlen(cur->name), (void*)cur->name);
    }
}

bool Controller::pass0()
{
  ElfSymbol **fn_syms;
  Instruction *last = NULL;
  uint32_t *textSegment = (uint32_t*)this->elf->getText();
  int n_insns = this->elf->getTextSize() / 4;
  int n_functions = this->elf->getNumberOfFunctions();
  uint32_t textBase = this->elf->getEntryPoint();
  int i;

  this->instructions = (Instruction**)xcalloc(n_insns + 1, sizeof(Instruction*));

  for (int i = 0; i < n_insns; i++)
    {
      Instruction *insn = InstructionFactory::getInstance()->create(textBase + i*4,
								    textSegment[i]);

      /* Append to the delay slot of last instruction, if there is one */
      if (i >= 1 && last->hasDelaySlot())
	{
	  last->setDelayed(insn);
	  insn = InstructionFactory::getInstance()->createNop( insn->getAddress() );
	}
      this->instructions[i] = insn;
      last = insn;
    }

  this->functions = (Function**)xcalloc(n_functions + 1, sizeof(Function*));
  this->methods = (JavaMethod**)xcalloc(n_functions + 1, sizeof(JavaMethod*));

  this->callTableMethod = new CallTableMethod(n_functions + 1);

  /* Create all functions and methods */
  fn_syms = this->elf->getFunctions();
  assert(fn_syms);
  for (i = 0; fn_syms[i]; i++)
    {
      ElfSymbol *sym = fn_syms[i];

      /* Create a new function with a set of instructions, this
       * will in turn create basic blocks
       */
      if (sym->addr == this->elf->getEntryPoint())
        this->functions[i] = new StartFunction(sym->name, this->instructions,
                                               (sym->addr - textBase) / 4,
                                               (sym->addr - textBase + sym->size) / 4 - 1);
      else
        this->functions[i] = new Function(sym->name, this->instructions,
                                          (sym->addr - textBase) / 4,
                                          (sym->addr - textBase + sym->size) / 4 - 1);
      /* 1-1 mapping */
      this->methods[i] = new JavaMethod(functions, i, i);
      /* For now add all methods to the call table */
      this->callTableMethod->addMethod(this->methods[i]);
    }

  /* And the (single) class */
  this->classes = (JavaClass**)xcalloc(1, sizeof(JavaClass*));
  this->classes[0] = new JavaClass(this->methods, 0, i-1);

  this->syscalls = (Syscall**)xcalloc(sizeof(Syscall*),
                                      this->elf->getCibylStrtabSize());

  this->n_classes = 1;

  return true;
}

Syscall *Controller::getSyscall(uint32_t value)
{
  assert(value < this->elf->getCibylStrtabSize() * sizeof(Syscall*));

  /*
   * this->syscalls is a sparse "table" of cached syscalls. It uses
   * the string offset as an index into the table and stores a pointer
   * there. Most of the entries will be empty, so memory is wasted,
   * but this should speed things up a bit.
   */
  if (!this->syscalls[value])
    {
      char *name = this->elf->getCibylStrtabString(value);
      cibyl_db_entry_t *p = (cibyl_db_entry_t*)ght_get(this->syscall_db_table,
                                                       strlen(name), name);
      if (!p)
        {
          fprintf(stderr, "No syscall %s:\n  Are all syscall databases added on the command line (cibyl-syscalls.db)?\n", name);
          exit(1);
        }

      this->syscalls[value] = new Syscall(p->name, p->nrArgs,
                                          p->returns ? 'I' : 'V' );
      /* Insert into the table for the syscall wrappers */
      ght_insert(this->syscall_used_table, p,
                 strlen(p->name), (void*)p->name);
    }
  return this->syscalls[value];
}


JavaMethod *Controller::getMethodByAddress(uint32_t addr)
{
  /* We only have a couple of classes anyway */
  for (int i = 0;
       i < this->n_classes;
       i++)
    {
      JavaMethod *out = this->classes[i]->getMethodByAddress(addr);

      if (out)
	return out;
    }

  return NULL;
}

JavaMethod *Controller::getCallTableMethod()
{
  return this->callTableMethod;
}

Instruction *Controller::getInstructionByAddress(uint32_t address)
{
  if (address < this->elf->getEntryPoint() ||
      address > this->elf->getEntryPoint() + this->elf->getTextSize())
    return NULL;

  return this->instructions[(address - this->elf->getEntryPoint()) / 4];
}


Instruction *Controller::getBranchTarget(uint32_t address)
{
  return this->getInstructionByAddress(address);
}

void Controller::lookupDataAddresses(JavaClass *cl, uint32_t *data, int n_entries)
{
  uint32_t text_start = this->elf->getEntryPoint();
  uint32_t text_end = text_start + this->elf->getTextSize();

  /* Add labels for data pointing to the text segment */
  for (int n = 0; n < n_entries; n++)
    {
      uint32_t v = be32_to_host(data[n]);

      if (v >= text_start && v < text_end)
        {
          /* Skip things which can not be code addresses */
          if (v & 0x3 != 0)
            continue;

          JavaMethod *mt = cl->getMethodByAddress(v);

          /* Something has an address in this method (which can be an address) */
          mt->addJumptabLabel(v);
          this->addJumptabLabel(v);
        }
    }
}

uint32_t Controller::addAlignedSection(uint32_t addr, FILE *fp, void *data,
                                       size_t data_len, int alignment)
{
  uint32_t out = addr;
  int pad = 0;

  if (addr & (alignment - 1))
    pad = (-addr) & (alignment - 1);
  out += pad;
  while (pad != 0)
    {
      int v = fputc('\0', fp);
      if (v < 0)
        {
          fprintf(stderr, "Cannot write padding to outfile in aligned section\n");
          exit(1);
        }
      pad--;
    }

  if (fwrite(data, 1, data_len, fp) != data_len)
    {
      fprintf(stderr, "Cannot write data to outfile in aligned section\n");
      exit(1);
    }

  return out + data_len;
}

bool Controller::pass1()
{
  bool out = true;

  for (int i = 0; i < this->n_classes; i++)
    {
      JavaClass *cl = this->classes[i];

      /* Add addresses in the different ELF sections to the lookup tables */
      this->lookupDataAddresses(cl, (uint32_t*)this->elf->getData(),
                                this->elf->getDataSize() / sizeof(uint32_t));
      this->lookupDataAddresses(cl, (uint32_t*)this->elf->getRodata(),
                                this->elf->getRodataSize() / sizeof(uint32_t));
      this->lookupDataAddresses(cl, (uint32_t*)this->elf->getCtors(),
                                this->elf->getCtorsSize() / sizeof(uint32_t));
      this->lookupDataAddresses(cl, (uint32_t*)this->elf->getDtors(),
                                this->elf->getDtorsSize() / sizeof(uint32_t));

      if (cl->pass1() != true)
	out = false;
    }
  this->callTableMethod->pass1();
  this->sortJumptabLabels();

  return out;
}


bool Controller::pass2()
{
  SyscallWrapperGenerator *syscallWrappers;
  bool out = true;
  uint32_t addr = 0;
  FILE *fp;

  /* Output the data sections to a file */
  fp = open_file_in_dir(this->dstdir, "program.data.bin", "w");
  addr = this->addAlignedSection(addr, fp, this->elf->getData(),
                                 this->elf->getDataSize(), 16);
  addr = this->addAlignedSection(addr, fp, this->elf->getRodata(),
                                 this->elf->getRodataSize(), 16);
  addr = this->addAlignedSection(addr, fp, this->elf->getCtors(),
                                 this->elf->getCtorsSize(), 16);
  addr = this->addAlignedSection(addr, fp, this->elf->getDtors(),
                                 this->elf->getDtorsSize(), 16);
  fclose(fp);

  for (int i = 0; i < this->n_classes; i++)
    {
      if (this->classes[i]->pass2() != true)
	out = false;
    }
  emit->setOutputFile(open_file_in_dir(this->dstdir, "CibylCallTable.java", "w"));
  this->callTableMethod->pass2();

  emit->setOutputFile(open_file_in_dir(this->dstdir, "Syscalls.java", "w"));
  syscallWrappers = new SyscallWrapperGenerator(this->n_syscall_dirs, this->syscall_dirs,
                                                this->syscall_used_table);
  syscallWrappers->pass2();

  return out;
}


Controller *controller;
Config *config;

static void usage()
{
  printf("Usage: xcibyl-translator dst-dir elf-file syscall-database...\n"
         "\n"
         "Where dst-dir is the destination directory to put translated files in, elf-file\n"
         "the input MIPS binary file, syscall-database is a cibyl-syscalls.db file with\n"
         "with possible syscalls (any number can be given)\n.");
}

int main(int argc, const char **argv)
{
  uint32_t trace_start = 0;
  uint32_t trace_end = 0;
  int opt;

  if (argc < 4)
    {
      fprintf(stderr, "Too few arguments\n");

      usage();
      return 1;
    }

  while ((opt = getopt(argc, (char* const*)argv, "t:") != -1))
    {
      switch (opt)
        {
        case 't':
          char *endp;

          printf("Option %s, %s\n", argv[optind], argv[optind+1]);
          trace_start = strtol(argv[optind], &endp, 0);
          if (endp == argv[optind])
            {
              fprintf(stderr, "Error: Argument '%s' to -t cannot be converted to a number\n",
                      argv[optind]);
              exit(1);
            }
          trace_end = strtol(argv[optind+1], &endp, 0);
          if (endp == argv[optind+1])
            {
              fprintf(stderr, "Error: Argument '%s' to -t cannot be converted to a number\n",
                      argv[optind+1]);
              exit(1);
            }
          optind += 2;
          break;
        default:
          usage();
          exit(1);
          break;
        }
    }

  emit = new Emit();
  config = new Config();
  config->traceRange[0] = trace_start;
  config->traceRange[1] = trace_end;

  regalloc = new RegisterAllocator();
  controller = new Controller(argv[1], argv[2], argc-3, &argv[3]);

  controller->pass0();
  controller->pass1();
  controller->pass2();

  return 0;
}
