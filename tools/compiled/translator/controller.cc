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
#include <stdio.h>

#include <utils.h>
#include <emit.hh>
#include <controller.hh>
#include <registerallocator.hh>
#include <config.hh>

Controller::Controller(const char *dstdir, const char *elf_filename,
                       int n_dbs, const char **database_filenames) : CodeBlock()
{
  this->elf = new CibylElf(elf_filename);
  elf = this->elf;

  this->syscall_db_table = ght_create(1024);

  this->dstdir = dstdir;
  this->instructions = NULL;
  this->functions = NULL;
  this->methods = NULL;
  this->classes = NULL;

  for (int i = 0; i < n_dbs; i++)
    this->readSyscallDatabase(database_filenames[i]);

  this->builtins = new BuiltinFactory();
}

void Controller::readSyscallDatabase(const char *filename)
{
  cibyl_db_entry_t *syscall_entries;
  file_chunk_t *file = read_file(filename);
  uint32_t n;
  char *strtab;

  n = be32_to_host(((uint32_t*)file->data)[0]);
  strtab = (char*)file->data + sizeof(uint32_t) + be32_to_host((((uint32_t*)file->data)[1]));

  syscall_entries = (cibyl_db_entry_t*)(((uint32_t*)file->data) + 2);

  /* Fixup the entries */
  for (uint32_t i = 0; i < n; i++)
    {
      cibyl_db_entry_t *cur = &syscall_entries[i];

      cur->nr = be32_to_host(cur->nr);
      cur->returns = be32_to_host(cur->returns);
      cur->nrArgs = be32_to_host(cur->nrArgs);
      cur->qualifier = be32_to_host(cur->qualifier);
      cur->name = strtab + be32_to_host((uint32_t)cur->name);

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
	  insn = new Nop( insn->getAddress() );
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
          JavaMethod *mt = cl->getMethodByAddress(v);

          /* Something has an address in this method */
          mt->addJumptabLabel(v);
          this->addJumptabLabel(v);
        }
    }
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
  char buf[80];
  bool out = true;

  snprintf(buf, 80, "%s/%s", this->dstdir, "Cibyl.j");
  emit->setOutputFile(buf);
  for (int i = 0; i < this->n_classes; i++)
    {
      if (this->classes[i]->pass2() != true)
	out = false;
    }
  snprintf(buf, 80, "%s/%s", this->dstdir, "CibylCallTable.java");
  emit->setOutputFile(buf);
  this->callTableMethod->pass2();

  return out;
}


Controller *controller;
Config *config;

int main(int argc, const char *argv[])
{
  if (argc < 4)
    {
      fprintf(stderr, "Too few arguments\n");
      return 1;
    }

  emit = new Emit();
  config = new Config();

  regalloc = new RegisterAllocator();
  controller = new Controller(argv[1], argv[2], argc-3, &argv[3]);

  controller->pass0();
  controller->pass1();
  controller->pass2();

  return 0;
}
