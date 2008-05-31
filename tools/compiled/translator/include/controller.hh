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
  Controller(const char **defines,
             const char *dstdir, const char *elf_filename,
             int n_dbs, const char **database_filenames);

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

  JavaClass *getClassByMethodName(const char *name)
  {
    panic_if(!name, "method name is NULL");

    return (JavaClass *)ght_get(this->method_to_class,
                                strlen(name), name);
  }

private:
  void allocateClasses();

  char *resolveStrtabAddress(char *strtab, char *offset);
  unsigned long getSyscallFileLong(void *_p, int offset);
  void readSyscallDatabase(const char *filename);
  void lookupDataAddresses(uint32_t *data, int n_entries);
  void lookupRelocations(JavaClass *cl);

  uint32_t addAlignedSection(uint32_t addr, FILE *fp, void *data,
                             size_t data_len, int alignment);

  Instruction *getInstructionByAddress(uint32_t addr);

  void fixupExportedSymbols(cibyl_exported_symbol_t *exp_syms, size_t n);

  JavaClass **classes;
  JavaMethod **methods;
  Function **functions;
  Instruction **instructions;

  ght_hash_table_t *method_to_class;

  CallTableMethod *callTableMethod;

  int n_classes;
  int n_methods;
  int n_functions;
  int n_instructions;
  int n_syscall_dirs;
  int n_syscall_sets;

  size_t textSize;

  const char *dstdir;

  Syscall **syscalls; /* Sparse table of syscalls */

  ght_hash_table_t *syscall_db_table;
  ght_hash_table_t *syscall_used_table;
  char **syscall_dirs;
  char **syscall_sets;

  const char **defines; /* NULL-terminated */

  Instruction *currentInstruction;

  BuiltinFactory *builtins;
};

extern Controller *controller;

#endif /* !__CONTROLLER_HH__ */
