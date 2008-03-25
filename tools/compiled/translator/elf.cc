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
#include <utils.h>
#include <fcntl.h>


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

  /* Allocate big enough tables of symbols */
  this->symbols = (ElfSymbol**)xcalloc(n, sizeof(ElfSymbol));
  this->functionSymbols = (ElfSymbol**)xcalloc(n, sizeof(ElfSymbol));
  this->dataSymbols = (ElfSymbol**)xcalloc(n, sizeof(ElfSymbol));

  /* Iterate through all symbols */
  for (int i = 0; i < n; i++)
    {
      const char *sym_name = elf_strptr(this->elf, shdr->sh_link, s->st_name);
      int type = ELF32_ST_TYPE(s->st_info);

      /* Ohh... This is an interesting symbol, add it! */
      if ( type == STT_FUNC)
        this->symbols[n_syms++] = this->functionSymbols[n_fns++] = new ElfSymbol(i, s->st_value, s->st_size,
                                                                                 type, sym_name);
      else if (type == STT_OBJECT || type == STT_COMMON || type == STT_TLS)
        this->symbols[n_syms++] = this->dataSymbols[n_datas++] = new ElfSymbol(i, (uint32_t)s->st_value,
                                                                               (uint32_t)s->st_size,
                                                                               type, sym_name);
      s++;
    }

  this->n_symbols = n_syms;
  this->n_functionSymbols = n_fns;
  this->n_dataSymbols = n_datas;

  /* Insert all symbols into a hash table */
  for (int i = 0; i < n_syms; i++)
    {
      ght_insert(this->symtable, (void*)this->symbols[i],
                 sizeof(uint32_t), (void*)&this->symbols[i]->index);
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

CibylElf::CibylElf(const char *filename)
{
  Elf_Scn *scn = NULL;
  Elf32_Ehdr *ehdr;
  size_t shstrndx;
  int fd;

  this->text = NULL;
  this->textSize = 0;
  this->data = NULL;
  this->dataSize = 0;
  this->rodata = NULL;
  this->rodataSize = 0;
  this->ctors = NULL;
  this->ctorsSize = 0;
  this->dtors = NULL;
  this->dtorsSize = 0;
  this->cibylstrtab = NULL;
  this->cibylstrtabSize = 0;

  this->symtable = ght_create(1024);

  if (elf_version(EV_CURRENT) == EV_NONE)
    {
      fprintf(stderr, "ELF version failed on %s\n", filename);
      exit(1);
    }

  if ( (fd = open(filename, O_RDONLY, 0)) < 0)
    {
      fprintf(stderr, "Cannot open %s\n", filename);
      exit(1);
    }

  if ( !(this->elf = elf_begin(fd, ELF_C_READ, NULL)) )
    {
      fprintf(stderr, "elf_begin failed on %s\n", filename);
      exit(1);
    }

  if ( !(ehdr = elf32_getehdr(this->elf)) )
    {
      fprintf(stderr, "elf32_getehdr failed on %s\n", filename);
      exit(1);
    }
  this->entryPoint = (uint32_t)ehdr->e_entry;

  if (elf_getshstrndx(this->elf, &shstrndx) < 0)
    {
      fprintf(stderr, "elf_getshstrndx failed on %s\n", filename);
      exit(1);
    }

  /* Iterate through sections, pass 1 */
  int max_relocs = 0;
  while ( (scn = elf_nextscn(this->elf, scn)) != NULL )
    {
      Elf32_Shdr *shdr = elf32_getshdr(scn);
      Elf_Data *data = elf_getdata(scn, NULL);
      char *name;

      name = elf_strptr(this->elf, shstrndx, shdr->sh_name);
      if (!data)
        {
          fprintf(stderr, "elf_getdata failed on section %s in %s\n",
                  name, filename);
          exit(1);
        }

      if ( strcmp(name, ".text") == 0 )
        {
          this->text = (uint8_t*)data->d_buf;
          this->textSize = data->d_size;
        }
      if ( strcmp(name, ".data") == 0 )
        {
          this->data = (uint8_t*)data->d_buf;
          this->dataSize = data->d_size;
        }
      if ( strcmp(name, ".rodata") == 0 )
        {
          this->rodata = (uint8_t*)data->d_buf;
          this->rodataSize = data->d_size;
        }
      if ( strcmp(name, ".ctors") == 0 )
        {
          this->ctors = (uint8_t*)data->d_buf;
          this->ctorsSize = data->d_size;
        }
      if ( strcmp(name, ".dtors") == 0 )
        {
          this->dtors = (uint8_t*)data->d_buf;
          this->dtorsSize = data->d_size;
        }
      if ( strcmp(name, ".cibylstrtab") == 0 )
        {
          this->cibylstrtab = (uint8_t*)data->d_buf;
          this->cibylstrtabSize = data->d_size;
        }

      /* Handle symbols */
      if (shdr->sh_type == SHT_SYMTAB)
        this->handleSymtab(scn);

      /* Count relocs */
      if (shdr->sh_type == SHT_REL)
        {
          int n = data->d_size / sizeof(Elf32_Rel);

          /* Don't count .rel.pdr for now */
          if (strcmp(name, ".rel.pdr") != 0)
            max_relocs += n;
        }
    }

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
      if (!data)
        {
          fprintf(stderr, "elf_getdata failed on section %s in %s\n",
                  name, filename);
          exit(1);
        }

      if (shdr->sh_type == SHT_REL)
        {
          Elf32_Rel *s = (Elf32_Rel *)data->d_buf;
          int n = data->d_size / sizeof(Elf32_Rel);

          /* Let's not care about .rel.pdr for now */
          if (strcmp(name, ".rel.pdr") == 0)
            continue;

          for (int i = 0; i < n; i++)
            {
              int sym_idx = ELF32_R_SYM(s->r_info);
              int type = ELF32_R_TYPE(s->r_info);
              ElfSymbol *sym = (ElfSymbol*)ght_get(this->symtable, sizeof(uint32_t),
                                                   (void*)&sym_idx);

              assert(cur < max_relocs);
              this->relocs[cur++] = new ElfReloc(s->r_offset, type, sym);
              s++;
            }
        }
    }
  this->n_relocs = cur;

  /* Sort relocs */
  qsort((void*)this->relocs, this->n_relocs, sizeof(ElfReloc*),	reloc_cmp);

  /* Sort the symbols and fixup addresses */
  this->fixupSymbolSize(this->functionSymbols,
                        this->n_functionSymbols);
  this->fixupSymbolSize(this->dataSymbols,
                        this->n_dataSymbols);

  close(fd);
}

void CibylElf::fixupSymbolSize(ElfSymbol **table, int n)
{
  /* Sort the symbols by address */
  qsort((void*)table, n, sizeof(ElfSymbol*), symbol_cmp);

  /* Fixup sizes */
  for (int i = 1; i < n; i++)
    {
      ElfSymbol *cur = table[i];
      ElfSymbol *last = table[i-1];

      if (last->addr + last->size <
          cur->addr - 4)
        last->size = cur->addr - last->addr - 4;
    }
}

ElfSymbol **CibylElf::getFunctions()
{
  return this->functionSymbols;
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

Elf *elf;
