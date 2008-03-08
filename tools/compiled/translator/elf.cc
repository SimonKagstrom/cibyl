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


CibylElf::CibylElf(const char *filename)
{
  Elf_Scn *scn = NULL;
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

  if (elf_getshstrndx(this->elf, &shstrndx) < 0)
    {
      fprintf(stderr, "elf_getshstrndx failed on %s\n", filename);
      exit(1);
    }

  /* Iterate through sections */
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
        {
          Elf32_Sym *s = (Elf32_Sym *)data->d_buf;
          int n_fns = 0;
          int n_datas = 0;
          int n = data->d_size / sizeof(Elf32_Sym);
          int i;

          /* Allocate big enough tables of symbols */
          this->functionSymbols = (ElfSymbol**)xcalloc(n, sizeof(ElfSymbol));
          this->dataSymbols = (ElfSymbol**)xcalloc(n, sizeof(ElfSymbol));

          /* Iterate through all symbols */
          for (i = 0; i < n; i++)
            {
              const char *sym_name = elf_strptr(this->elf, shdr->sh_link, s->st_name);
              int type = ELF32_ST_TYPE(s->st_info);

              /* Ohh... This is an interesting symbol, add it! */
              if ( type == STT_FUNC)
                this->functionSymbols[n_fns++] = new ElfSymbol(s->st_value, s->st_size,
                                                               type, sym_name);
              else if (type == STT_OBJECT || type == STT_COMMON || type == STT_TLS)
                this->dataSymbols[n_datas++] = new ElfSymbol((uint32_t)s->st_value, (uint32_t)s->st_size,
                                                             type, sym_name);
              s++;
            }

          this->n_functionSymbols = n_fns;
          this->n_dataSymbols = n_datas;
        }
    }

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
  for (int i = 1; i < this->n_functionSymbols; i++)
    {
      ElfSymbol *cur = this->functionSymbols[i];
      ElfSymbol *last = this->functionSymbols[i-1];

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

Elf *elf;
