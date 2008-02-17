/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      controller.hh
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   The singleton class which holds everything
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __CONTROLLER_HH__
#define __CONTROLLER_HH__

#include <ght_hash_table.h>

#include <javamethod.hh>
#include <javaclass.hh>
#include <elf.hh>
#include <builtins.hh>

class Controller : public CodeBlock
{
public:
  Controller(const char *dstdir, const char *elf_filename, int n_dbs, const char **database_filenames);

  bool pass0();
  bool pass1();
  bool pass2();

  Instruction *getBranchTarget(uint32_t addr);
  JavaMethod *getMethodByAddress(uint32_t addr);
  JavaMethod *getCallTableMethod();
  Syscall *getSyscall(uint32_t value);

  /**
   * Set the instruction currently being compiled
   *
   * @param insn the instruction being compiled
   */
  void setCurrentInstruction(Instruction *insn)
  {
    this->currentInstruction = insn;
  }

  /**
   * Get the instruction currently being compiled
   *
   * @return the instruction being compiled
   */
  Instruction* getCurrentInstruction()
  {
    return this->currentInstruction;
  }

  Builtin *matchBuiltin(const char *name)
  {
    return this->builtins->match(name);
  }

private:
  void readSyscallDatabase(const char *filename);
  void lookupDataAddresses(JavaClass *cl, uint32_t *data, int n_entries);

  Instruction *getInstructionByAddress(uint32_t addr);

  JavaClass **classes;
  JavaMethod **methods;
  Function **functions;
  Instruction **instructions;

  CallTableMethod *callTableMethod;

  int n_classes;
  int n_methods;
  int n_functions;
  int n_instructions;

  const char *dstdir;

  CibylElf *elf;
  Syscall **syscalls; /* Sparse table of syscalls */

  ght_hash_table_t *syscall_db_table;

  Instruction *currentInstruction;

  BuiltinFactory *builtins;
};

extern Controller *controller;

#endif /* !__CONTROLLER_HH__ */