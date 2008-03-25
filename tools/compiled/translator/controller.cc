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

Controller::Controller(const char **defines,
                       const char *dstdir, const char *elf_filename,
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
  this->n_syscall_sets = 0;
  this->syscall_dirs = NULL;
  this->syscall_sets = NULL;
  this->defines = defines;

  for (int i = 0; i < n_dbs; i++)
    this->readSyscallDatabase(database_filenames[i]);

  this->builtins = new BuiltinFactory();

  emit->setOutputFile(open_file_in_dir(this->dstdir, "Cibyl.j", "w"));
}

char *Controller::resolveStrtabAddress(char *strtab, char *offset)
{
  return strtab + (unsigned long)offset;
}

unsigned long Controller::getSyscallFileLong(void *_p, int offset)
{
  unsigned long *p = (unsigned long*)_p;

  return p[offset];
}

void Controller::readSyscallDatabase(const char *filename)
{
  cibyl_db_entry_t *syscall_entries;
  int first_syscall_dir = this->n_syscall_dirs;
  int first_syscall_set = this->n_syscall_sets;
  unsigned long magic, n_dirs, n_sets, n, args_start, idx;
  char *strtab;
  size_t size;
  void *data;

  data = read_file(&size, filename);
  if (!data)
    {
      fprintf(stderr, "Cannot read %s\n", filename);
      exit(1);
    }

  /* This is oh-so-ugly... */
  idx = 0;
  magic = this->getSyscallFileLong(data, idx++);
  n_dirs = this->getSyscallFileLong(data, idx++);
  n_sets = this->getSyscallFileLong(data, idx++);
  n = this->getSyscallFileLong(data, idx++);
  args_start = (unsigned long)data +
    this->getSyscallFileLong(data, idx++);
  strtab = (char*)data +
    this->getSyscallFileLong(data, idx++);

  /* Add to the syscall directories and sets */
  this->n_syscall_dirs += n_dirs;
  this->syscall_dirs = (char**)xrealloc(this->syscall_dirs, this->n_syscall_dirs * sizeof(char**));
  for (int i = first_syscall_dir; i < this->n_syscall_dirs; i++)
      this->syscall_dirs[i] = ((char*)strtab) +
        this->getSyscallFileLong(data, idx++);

  this->n_syscall_sets += n_sets;
  this->syscall_sets = (char**)xrealloc(this->syscall_sets, this->n_syscall_sets * sizeof(char**));
  for (int i = first_syscall_set; i < this->n_syscall_sets; i++)
      this->syscall_sets[i] = ((char*)strtab) +
        this->getSyscallFileLong(data, idx++);

  syscall_entries = (cibyl_db_entry_t*)(((unsigned long*)data) + idx++);

  /* Fixup the entries */
  for (unsigned int i = 0; i < n; i++)
    {
      cibyl_db_entry_t *cur = &syscall_entries[i];

      cur->nr = cur->nr;
      cur->returns = cur->returns;
      cur->nrArgs = cur->nrArgs;
      cur->qualifier = cur->qualifier;
      cur->name = strtab + (unsigned long)cur->name;
      cur->javaClass = strtab + (unsigned long)cur->javaClass;
      cur->javaMethod = strtab + (unsigned long)cur->javaMethod;
      cur->set = strtab + (unsigned long)cur->set;
      cur->returnType = strtab + (unsigned long)cur->returnType;
      cur->args = (cibyl_db_arg_t*)(args_start + (unsigned long)cur->args);
      cur->user = first_syscall_dir;

      for (unsigned int j = 0; j < cur->nrArgs; j++)
        {
          unsigned long t_offs = (unsigned long)cur->args[j].type & 0x00ffffff;
          unsigned long jt = (unsigned long)cur->args[j].javaType;
          unsigned long jt_offs = jt & 0x00ffffff;
          unsigned long jt_flags = (jt & 0xff000000) >> 24;

          cur->args[j].flags = jt_flags;
          cur->args[j].javaType = (char*)(strtab + jt_offs);
          cur->args[j].type = (char*)(strtab + t_offs);
          cur->args[j].name = (char*)(strtab +
                                      (unsigned long)cur->args[j].name);
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
	  insn = InstructionFactory::getInstance()->createDelaySlotNop( insn->getAddress() );
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

          assert(mt);

          /* Add to the call table */
          if (mt->getAddress() == v)
            this->callTableMethod->addMethod(mt);

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

typedef struct
{
  int hi_start;
  int hi_end;
  int lo_start;
  int lo_end;
} hilo_reloc_limit_t;

void Controller::lookupRelocations(JavaClass *cl)
{
  ElfReloc **relocs = this->elf->getRelocations();
  int n = this->elf->getNumberOfRelocations();
  hilo_reloc_limit_t *hilos_per_method;

  /* Initialize hi/lo pairs to -1 */
  hilos_per_method = (hilo_reloc_limit_t *)xcalloc(cl->getNumberOfMethods(),
                                                   sizeof(hilo_reloc_limit_t));
  memset(hilos_per_method, -1,
         cl->getNumberOfMethods() * sizeof(hilo_reloc_limit_t));

  for (int i = 0; i < n; i++)
    {
      ElfReloc *rel = relocs[i];

      /* If we have a relocation to a function object, add that to
       * the call table */
      if (rel->sym)
        {
          if (rel->sym->type == STT_FUNC && rel->type != R_MIPS_26)
            {
              JavaMethod *mt = cl->getMethodByAddress(rel->sym->addr);

              assert(mt);

              if (mt->getAddress() == rel->sym->addr)
                this->callTableMethod->addMethod(mt);
            }
        }
      else if (rel->type == R_MIPS_HI16 || rel->type == R_MIPS_LO16)
        {
          int idx;
          JavaMethod *reloc_mt = cl->getMethodByAddress(rel->addr, &idx);

          if (reloc_mt)
            {
              hilo_reloc_limit_t *hilo = &hilos_per_method[idx];

              if (rel->type == R_MIPS_HI16 &&
                  hilo->hi_start == -1)
                ;
            }
        }
    }
  free(hilos_per_method);
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

        /* And loop through the relocations and add these */
        this->lookupRelocations(cl);

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

    syscallWrappers = new SyscallWrapperGenerator(this->defines, this->dstdir,
                                                  this->n_syscall_dirs, this->syscall_dirs,
                                                  this->n_syscall_sets, this->syscall_sets,
                                                  this->syscall_used_table);
    syscallWrappers->pass2();

    return out;
  }


  Controller *controller;
  Config *config;

  static void usage()
  {
    printf("Usage: xcibyl-translator trace-start trace-end dst-dir elf-file syscall-database...\n"
           "\n"
           "Where trace-start and trace-end are start and end addresses for instruction\n"
           "tracing, dst-dir is the destination directory to put translated files in\n"
           ", elf-file the input MIPS binary file, syscall-database is a cibyl-syscalls.db\n"
           "file with with possible syscalls (any number can be given)\n");
  }

  int main(int argc, const char **argv)
  {
    uint32_t trace_start = 0;
    uint32_t trace_end = 0;
    const char **defines = (const char **)xcalloc(argc, sizeof(const char*));
    int n, n_defines = 0;
    char *endp;

    if (argc < 6)
      {
        fprintf(stderr, "Too few arguments\n");

        usage();
        return 1;
      }

    trace_start = strtol(argv[1], &endp, 0);
    if (endp == argv[1])
      {
        fprintf(stderr, "Error: Argument '%s' to -t cannot be converted to a number\n",
                argv[1]);
        exit(1);
      }
    trace_end = strtol(argv[2], &endp, 0);
    if (endp == argv[2])
      {
        fprintf(stderr, "Error: Argument '%s' to -t cannot be converted to a number\n",
                argv[2]);
        exit(1);
      }

    /* Setup defines */
    for (n = 3; n < argc && strncmp(argv[n], "-D", 2) == 0; n++)
      {
        defines[n_defines++] = argv[n];
      }

    emit = new Emit();
    config = new Config();
    config->traceRange[0] = trace_start;
    config->traceRange[1] = trace_end;

    regalloc = new RegisterAllocator();
    controller = new Controller(defines, argv[n], argv[n+1],
                                argc - n - 2, &argv[n + 2]);

    controller->pass0();
    controller->pass1();
    controller->pass2();

    return 0;
  }
