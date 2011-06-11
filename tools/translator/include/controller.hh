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

#include <map>

#include <javamethod.hh>
#include <javaclass.hh>
#include <elf.hh>
#include <builtins.hh>
#include <functioncolocation.hh>

#include <cpp-utils.hh>

#define N_TRY_STACK_ENTRIES 32

using namespace std;

class Controller : public CodeBlock
{
public:
  Controller(const char *argv0, const char **defines,
             const char *dstdir, const char *elf_filename,
             int n_dbs, const char **database_filenames);

  bool pass0();
  bool pass1();
  bool pass2();

  Instruction *getBranchTarget(uint32_t addr);
  JavaMethod *getMethodByAddress(uint32_t addr);
  JavaMethod *getCallTableMethod();
  Syscall *getSyscall(uint32_t value);

  const char *getDstDir()
  {
    return this->dstdir;
  }
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

  Builtin *matchBuiltin(Instruction *insn, const char *name)
  {
    return this->builtins->match(insn, name);
  }

  JavaClass *getClassByMethodName(const char *name)
  {
    panic_if(!name, "method name is NULL");

    return this->m_method_to_class[name];
  }

  void addColocation(const char *str);

  void pushTryStack(Instruction *insn);

  Instruction *popTryStack();

  const char *getPackageName()
  {
    return this->package_name;
  }

  const char *getJasminPackagePath()
  {
    if (this->jasmin_package_path[0] == '\0')
      return "";
    return this->jasmin_package_path;
  }

  void setPackageName(const char *name);

  const char *getInstallDirectory();

  typedef map<const char *, JavaClass *, cmp_str> JavaClassTable_t;
  typedef map<const char *, cibyl_db_entry_t *, cmp_str> CibylDbTable_t;

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

  JavaClassTable_t m_method_to_class;

  CallTableMethod *callTableMethod;

  int n_classes;
  int n_methods;
  int n_functions;
  int n_instructions;
  int n_syscall_dirs;
  int n_syscall_sets;

  size_t textSize;

  const char *dstdir;

  char *m_baseDir;

  Syscall **syscalls; /* Sparse table of syscalls */

  CibylDbTable_t m_syscall_db_table;
  CibylDbTable_t m_syscall_used_table;

  char **syscall_dirs;
  char **syscall_sets;

  const char **defines; /* NULL-terminated */

  Instruction *currentInstruction;

  BuiltinFactory *builtins;

  FunctionColocation **colocs;
  int n_colocs;

  /* Try/catch blocks are handled as a stack */
  int try_stack_top;
  Instruction *try_stack[N_TRY_STACK_ENTRIES];

  const char *package_name;
  char jasmin_package_path[255];
};

extern Controller *controller;

#endif /* !__CONTROLLER_HH__ */
