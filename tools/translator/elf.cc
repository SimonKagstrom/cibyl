/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      elf.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   ELF implementation
 *
 * $Id:$
 *
 ********************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <libelf.h>
#include <dwarf.h>
#include <elfutils/libdw.h>

#include <mips-dwarf.h>
#include <utils.h>
#include <elf.hh>


static int symbol_cmp(const void *_a, const void *_b)
{
  ElfSymbol *a = *(ElfSymbol**)_a;
  ElfSymbol *b = *(ElfSymbol**)_b;

  return a->addr - b->addr;
}

void CibylElf::handleSymtab(Elf_Scn *scn)
{
  Elf32_Shdr *shdr = elf32_getshdr(scn);
  Elf_Data *data = elf_getdata(scn, NULL);
  Elf32_Sym *s = (Elf32_Sym *)data->d_buf;
  int n_syms = 0;
  int n_fns = 0;
  int n_datas = 0;
  int n = data->d_size / sizeof(Elf32_Sym);

  panic_if(n <= 0,
           "Section data too small (%zd) - no symbols\n",
           data->d_size);

  /* Allocate big enough tables of symbols */
  this->symbols = (ElfSymbol**)xcalloc(n, sizeof(ElfSymbol));
  this->functionSymbols = (ElfSymbol**)xcalloc(n, sizeof(ElfSymbol));
  this->dataSymbols = (ElfSymbol**)xcalloc(n, sizeof(ElfSymbol));

  /* Iterate through all symbols */
  for (int i = 0; i < n; i++)
    {
      const char *sym_name = elf_strptr(this->elf, shdr->sh_link, s->st_name);
      int type = ELF32_ST_TYPE(s->st_info);
      int binding = ELF32_ST_BIND(s->st_info);

      /* Ohh... This is an interesting symbol, add it! */
      if ( type == STT_FUNC) {
          ElfSymbol *sym = new ElfSymbol(i, binding, s->st_value,
              s->st_size, type, sym_name);

          this->symbols[n_syms++] = this->functionSymbols[n_fns++] = sym;
          this->m_symbolsByAddr[sym->addr] = sym;
      }
      else if (type == STT_OBJECT || type == STT_COMMON || type == STT_TLS)
        this->symbols[n_syms++] = this->dataSymbols[n_datas++] = new ElfSymbol(i, binding, (uint32_t)s->st_value,
                                                                               (uint32_t)s->st_size,
                                                                               type, sym_name);
      else if ( type == STT_SECTION)
        this->symbols[n_syms++] = new ElfSymbol(i, binding, s->st_value, s->st_size, type, sym_name);
      s++;
    }

  this->n_symbols = n_syms;
  this->n_functionSymbols = n_fns;
  this->n_dataSymbols = n_datas;

  panic_if(n_symbols <= 0,
           "No symbols in the ELF file (file stripped?)\n");

  /* Insert all symbols into a hash table */
  for (int i = 0; i < n_syms; i++)
    {
      this->m_symbolTable[this->symbols[i]->index] = this->symbols[i];
    }
}

static int reloc_cmp(const void *_a, const void *_b)
{
  ElfReloc *a = *(ElfReloc**)_a;
  ElfReloc *b = *(ElfReloc**)_b;
  int type_diff = a->type - b->type;
  int addr_diff = a->addr - b->addr;

  /* First sort by type, then address */
  if (type_diff != 0)
    return type_diff;

  return addr_diff;
}

void CibylElf::handleDwarfFunction(Dwarf_Die *fun_die)
{
  Dwarf_Die result;
  Dwarf_Attribute attr_mem;
  Dwarf_Attribute *attr;
  Dwarf_Addr addr;

  if (dwarf_lowpc(fun_die, &addr) != 0)
    return;
  ElfSymbol *sym = this->getSymbolByAddr(addr);

  if (!sym)
    return;

  attr = dwarf_attr_integrate(fun_die, DW_AT_type, &attr_mem);

  sym->n_args = 0;
  sym->ret_size = 0;

  /* Handle the return value */
  enum mips_arg ret_val = mips_arg_size(elf, fun_die, attr);

  switch (ret_val) {
  case VOID:
	  sym->ret_size = 0;
	  break;
  case N_1:
	  sym->ret_size = 1;
	  break;
  case N_2:
	  sym->ret_size = 2;
	  break;
  case AGGREGATE:
	  /*
	   * From the "SYSTEM V APPLICATION BINARY INTERFACE, MIPS RISC Processor
	   * Supplement 3rd Edition" (3-18):
	   *
	   *   If the called function returns a structure or union, the caller passes the address
	   *   of an area that is large enough to hold the structure to the function
	   *   in $4. The called function copies the returned structure into this area before
	   *   it returns. This address becomes the first argument to the function for
	   *   the purposes of argument register allocation and all user arguments are
	   *   shifted down by one.
	   */
	  sym->ret_size = 1;
	  sym->n_args++;
	  break;
  case UNKNOWN:
  default:
	  sym->ret_size = -1;
	  break;
  }

  /* No arguments? */
  if (dwarf_child (fun_die, &result) != 0)
    return;

  /* There are arguments */
  do {
      enum mips_arg arg_size;

      switch (dwarf_tag (&result))
      {
      case DW_TAG_unspecified_parameters:
    	  /* varargs etc */
          sym->n_args = -1;
    	  return;
      case DW_TAG_formal_parameter:
        attr = dwarf_attr_integrate(&result, DW_AT_type, &attr_mem);
        arg_size = mips_arg_size(elf, fun_die, attr);

        if (arg_size != N_1)
          {
            /* Better safe than sorry - skip this */
            sym->n_args = -1;
            return;
          }

        sym->n_args++;
        break;
      default:
        break;
      }
  } while(dwarf_siblingof(&result, &result) == 0);
}


CibylElf::CibylElf(const char *filename)
{
  Elf_Scn *scn = NULL;
  Elf32_Ehdr *ehdr;
  size_t shstrndx;
  int fd;

  panic_if(elf_version(EV_CURRENT) == EV_NONE,
           "ELF version failed on %s\n", filename);

  panic_if((fd = open(filename, O_RDONLY, 0)) < 0,
           "Cannot open %s\n", filename);

  panic_if( !(this->elf = elf_begin(fd, ELF_C_READ, NULL)),
            "elf_begin failed on %s\n", filename);

  panic_if( !(ehdr = elf32_getehdr(this->elf)),
            "elf32_getehdr failed on %s\n", filename);
  this->entryPoint = (uint32_t)ehdr->e_entry;

  panic_if(elf_getshstrndx(this->elf, &shstrndx) < 0,
           "elf_getshstrndx failed on %s\n", filename);

  /* Iterate through sections, pass 1 */
  int max_relocs = 0;
  while ( (scn = elf_nextscn(this->elf, scn)) != NULL )
    {
      Elf32_Shdr *shdr = elf32_getshdr(scn);
      Elf_Data *data = elf_getdata(scn, NULL);
      char *name;

      name = elf_strptr(this->elf, shstrndx, shdr->sh_name);
      panic_if(!data,
               "elf_getdata failed on section %s in %s\n",
               name, filename);

      this->addSection(new ElfSection(name, (uint8_t*)data->d_buf,
                                      data->d_size, shdr->sh_type,
                                      shdr->sh_addralign, shdr->sh_addr));

      /* Handle symbols */
      if (shdr->sh_type == SHT_SYMTAB)
        this->handleSymtab(scn);

      /* Count relocs */
      if (shdr->sh_type == SHT_REL)
        {
          int n = data->d_size / sizeof(Elf32_Rel);

          /* Don't count .rel.pdr for now */
          if (strcmp(name, ".rel.pdr") != 0 && strncmp(name, ".rel.debug", 10) != 0)
            max_relocs += n;
        }
    }

  /* Some sections MUST be in the Cibyl elf */
  panic_if( !this->getSection(".text") ||
            !this->getSection(".cibylstrtab") ||
            !this->getSection(".data"),
            "An essential ELF section is missing in the input file. Is this\n"
            "a valid statically linked MIPS ELF?\n");

  /* Again iterate through the sections, handle relocations this time */
  this->relocs = (ElfReloc**)xcalloc(max_relocs, sizeof(ElfReloc*));
  scn = NULL;
  int cur = 0;
  while ( (scn = elf_nextscn(this->elf, scn)) != NULL )
    {
      Elf32_Shdr *shdr = elf32_getshdr(scn);
      Elf_Data *data = elf_getdata(scn, NULL);
      char *name;

      name = elf_strptr(this->elf, shstrndx, shdr->sh_name);
      panic_if(!data,
               "elf_getdata failed on section %s in %s\n",
               name, filename);

      if (shdr->sh_type == SHT_REL)
        {
          Elf32_Rel *s = (Elf32_Rel *)data->d_buf;
          int n = data->d_size / sizeof(Elf32_Rel);

          /* Let's not care about .rel.pdr for now */
          if (strcmp(name, ".rel.pdr") == 0 || strncmp(name, ".rel.debug", 10) == 0)
            continue;

          for (int i = 0; i < n; i++)
            {
              int sym_idx = ELF32_R_SYM(s->r_info);
              int type = ELF32_R_TYPE(s->r_info);
              ElfSymbol *sym = this->m_symbolTable[sym_idx];

              assert(cur < max_relocs);
              this->relocs[cur] = new ElfReloc(s->r_offset, type, sym);
	
              if(sym && sym->type == STT_SECTION && type == R_MIPS_32)
                {
                  /* Set addend from contents of location to be modified */
                  ElfSection* sec = this->getSection(name+4); // should use shdr->sh_link instead
                  if(sec)
                    {
                      uint32_t* data = (uint32_t*) sec->data;
                      uint32_t offset = s->r_offset - sec->addr;
                      this->relocs[cur]->addend = be32_to_host( data[offset/4] );
                    }
                }
		    
              if (sym)
                this->m_relocationsBySymbol[sym] = this->relocs[cur];
              cur++;
              s++;
            }
        }
    }
  this->n_relocs = cur;

  /* Sort relocs */
  qsort((void*)this->relocs, this->n_relocs, sizeof(ElfReloc*),	reloc_cmp);

  panic_if(!this->functionSymbols,
           "No function symbols in the ELF file\n");
  panic_if(!this->dataSymbols,
           "No data symbols in the ELF file\n");

  /* Sort the symbols and fixup addresses */
  this->fixupSymbolSize(this->functionSymbols,
                        this->n_functionSymbols,
                        this->getEntryPoint() + this->getSection(".text")->size);
  if (this->getSection(".data"))
    this->fixupSymbolSize(this->dataSymbols,
                          this->n_dataSymbols,
                          this->getSection(".data")->size);
  close(fd);


  Dwarf_Off last_offset = 0;
  Dwarf_Off offset = 0;
  size_t hdr_size;
  Dwarf *dbg;

  /* Initialize libdwarf */
  dbg = dwarf_begin_elf(this->elf, DWARF_C_READ, NULL);
  if (!dbg)
      panic("No DWARF info???");

  while (dwarf_nextcu(dbg, offset, &offset, &hdr_size, 0, 0, 0) == 0) {
      Dwarf_Die result, cu_die;

      if (dwarf_offdie(dbg, last_offset + hdr_size, &cu_die) == NULL)
        continue;
      last_offset = offset;

      if (dwarf_child (&cu_die, &result) != 0)
        continue;

      do {
          switch (dwarf_tag(&result))
          {
          case DW_TAG_subprogram:
          case DW_TAG_entry_point:
          case DW_TAG_inlined_subroutine:
            this->handleDwarfFunction(&result);
            break;
          default:
            break;
          }
      } while(dwarf_siblingof(&result, &result) == 0);
  }

  dwarf_end(dbg);
}

void CibylElf::fixupSymbolSize(ElfSymbol **table, int n, uint32_t sectionEnd)
{
  /* Sort the symbols by address */
  qsort((void*)table, n, sizeof(ElfSymbol*), symbol_cmp);

  /* Fixup sizes */
  for (int i = 1; i < n; i++)
    {
      ElfSymbol *cur = table[i];
      ElfSymbol *last = table[i-1];

      if (last->addr + last->size <
          cur->addr)
        last->size = cur->addr - last->addr;
    }
  /* And the very last one */
  ElfSymbol *last = table[n-1];
  if ( last->addr + last->size < sectionEnd )
    last->size = sectionEnd - last->addr;

}

ElfSymbol **CibylElf::getFunctions()
{
  return this->functionSymbols;
}

void CibylElf::addSection(ElfSection *section)
{
  this->m_sectionsByName[section->name] = section;
}

int CibylElf::getNumberOfFunctions()
{
  return this->n_functionSymbols;
}

ElfReloc **CibylElf::getRelocations()
{
  return this->relocs;
}

int CibylElf::getNumberOfRelocations()
{
  return this->n_relocs;
}

ElfReloc *CibylElf::getRelocationBySymbol(ElfSymbol *sym)
{
  return this->m_relocationsBySymbol[sym];
}

ElfSymbol *CibylElf::getSymbolByAddr(unsigned long addr)
{
  return this->m_symbolsByAddr[addr];
}
