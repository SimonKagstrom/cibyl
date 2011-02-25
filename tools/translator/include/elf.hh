/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      elf.hh
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   ELF stuff
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __ELF_HH__
#define __ELF_HH__

#include <map>

#include <stdint.h>
#include <stdlib.h>

#include <libelf.h>
#include <elfutils/libdw.h>

#include <cpp-utils.hh>

using namespace std;

#if !defined(R_MIPS_NONE) // e.g. Cygwin

#define R_MIPS_NONE       0
#define R_MIPS_16         1
#define R_MIPS_32         2
#define R_MIPS_REL32      3
#define R_MIPS_26         4
#define R_MIPS_HI16       5
#define R_MIPS_LO16       6

#endif

class CibylElf;

class ElfSymbol
{
public:
  ElfSymbol(int index, int binding, uint32_t addr, uint32_t size, int type, const char *name)
  {
    this->index = index;
    this->binding = binding;
    this->addr = addr;
    this->size = size;
    this->type = type;
    this->name = name;

    /* Unknown */
    this->ret_size = -1;
    this->n_args = -1;
  }

  int index;
  int binding;
  uint32_t addr;
  uint32_t size;
  int type;
  const char *name;

  int ret_size;
  int n_args;
};


class ElfReloc
{
public:
  ElfReloc(uint32_t addr, int type, ElfSymbol *sym)
  {
    this->addr = addr;
    this->type = type;
    this->sym = sym;
    this->addend = 0;
  }

  uint32_t addr;
  int type;
  ElfSymbol *sym;
  uint32_t addend;
};

class ElfSection
{
public:
  ElfSection(const char *name, uint8_t *data, size_t size,
             int type, uint32_t align, uint32_t addr)
  {
    this->name = name;
    this->data = data;
    this->size = size;
    this->type = type;
    this->align = align;
    this->addr = addr;
  }

  uint8_t *data;
  size_t   size;
  int      type;
  uint32_t align;
  uint32_t addr;
  const char *name;
};

/* Found in the special .cibylexpsyms function */
typedef struct
{
  uint32_t name;
  uint32_t addr;
} cibyl_exported_symbol_t;

class CibylElf
{
public:
  static CibylElf *getInstance();

  CibylElf(const char *filname);

  ElfSymbol **getSymbolsByType(char type);

  ElfSymbol **getFunctions();

  ElfSymbol *getSymbolByAddr(unsigned long addr);

  int getNumberOfFunctions();

  ElfReloc **getRelocations();

  int getNumberOfRelocations();

  uint32_t getEntryPoint() { return this->entryPoint; }

  const char *getCibylStrtabString(uint32_t offset)
  {
    /* This is safe since we assert that this section is there in
     * the constructor */
    return ((const char*)this->getSection(".cibylstrtab")->data) + offset;
  }

  ElfSection *getSection(const char *name)
  {
    return this->m_sectionsByName[name];
  }

  ElfReloc *getRelocationBySymbol(ElfSymbol *sym);

  typedef map<uint32_t, ElfSymbol*> ElfSymbolTable_t;
  typedef map<const char *, ElfSection*, cmp_str> ElfSectionTable_t;
  typedef map<ElfSymbol *, ElfReloc*> ElfRelocationTable_t;

private:
  void addSection(ElfSection *section);

  void handleSymtab(Elf_Scn *scn);

  void fixupSymbolSize(ElfSymbol **table, int n, uint32_t sectionEnd);

  void handleDwarfFunction(Dwarf_Die *fun_die);

  static CibylElf *instance;

  Elf *elf; /* from libelf */

  ElfSymbol **symbols;
  ElfSymbol **functionSymbols;
  ElfSymbol **dataSymbols;

  int n_symbols;
  int n_functionSymbols;
  int n_dataSymbols;
  ElfSymbolTable_t m_symbolTable;
  ElfSymbolTable_t m_symbolsByAddr;
  ElfSectionTable_t m_sectionsByName;
  ElfRelocationTable_t m_relocationsBySymbol;

  ElfReloc **relocs;
  int n_relocs;

  uint32_t entryPoint;
};

extern CibylElf *elf;

#endif /* !__ELF_HH__ */
