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
#include <limits.h>
#include <stdio.h>

#include <utils.h>
#include <emit.hh>
#include <controller.hh>
#include <registerallocator.hh>
#include <syscall-wrappers.hh>
#include <config.hh>

#include <libgen.h>
#include <unistd.h>

Controller::Controller(const char *argv0, const char **defines,
                       const char *dstdir, const char *elf_filename,
                       int n_dbs, const char **database_filenames) : CodeBlock()
{
  elf = new CibylElf(elf_filename);

  this->package_name = NULL;
  memset(this->jasmin_package_path, 0, sizeof(this->jasmin_package_path));


  // Ugly hack to get the directory path of the cibyl installation
  char cwdBuf[4096];
  char *cwd = getcwd(cwdBuf, sizeof(cwdBuf));
  panic_if(!cwd, "Can't read current directory!\n");

  char *cpy = (char *)xstrdup(argv0);
  const char *dn = dirname(cpy);
  const char *syscallRelativePath = "..";

  size_t len = strlen(dn) + strlen(syscallRelativePath) + 4;
  bool isRelative = dn[0] != '/';

  if (isRelative)
    len += strlen(cwd);

  this->m_baseDir = (char *)xcalloc(len, sizeof(char));
  xsnprintf(this->m_baseDir, len, "%s%s%s/%s/",
      isRelative ? cwd : "", isRelative ? "/" : "",
      dn, syscallRelativePath);

  free(cpy);

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

  this->colocs = NULL;
  this->n_colocs = 0;

  this->try_stack_top = 0;
  memset(this->try_stack, 0, sizeof(this->try_stack));

  /* Get the .text section size (a small optimization) */
  ElfSection *textSection = elf->getSection(".text");
  panic_if(!textSection, "No .text section\n");
  this->textSize = textSection->size;

  for (int i = 0; i < n_dbs; i++)
    this->readSyscallDatabase(database_filenames[i]);

  this->builtins = new BuiltinFactory();
}

const char *Controller::getInstallDirectory()
{
  return this->m_baseDir;
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
  panic_if(!data, "Cannot read %s\n", filename);

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
      this->m_syscall_db_table[cur->name] = cur;
    }
}

void Controller::fixupExportedSymbols(cibyl_exported_symbol_t *exp_syms, size_t n)
{
  panic_if(!exp_syms, "fixupExportedSymbols called without exp_syms!\n");

  /* Fixup the string offsets */
  for (unsigned int i = 0; i < n; i++)
    {
      uint32_t name = be32_to_host(exp_syms[i].name) & 0x00ffffff;
      uint32_t addr = be32_to_host(exp_syms[i].addr);

      exp_syms[i].name = name;
      exp_syms[i].addr = addr;
    }
}

bool Controller::pass0()
{
  ElfSymbol **fn_syms;
  Instruction *last = NULL;
  ElfSection *textSection = elf->getSection(".text");
  uint32_t *text = (uint32_t*)textSection->data; /* Would have panic:ed in the constructor otherwise */
  int n_insns = textSection->size / 4;
  int n_functions = elf->getNumberOfFunctions();
  uint32_t textBase = elf->getEntryPoint();
  int i;

  this->instructions = (Instruction**)xcalloc(n_insns + 1, sizeof(Instruction*));

  for (int i = 0; i < n_insns; i++)
    {
      Instruction *insn = InstructionFactory::getInstance()->create(textBase + i*4,
								    text[i]);

      /* Append to the delay slot of last instruction, if there is one */
      if (i >= 1 && last->hasDelaySlot())
	{
	  last->setDelayed(insn);
	  insn = InstructionFactory::getInstance()->createDelaySlotNop( insn->getAddress() );
	}
      this->instructions[i] = insn;
      last = insn;
    }
  this->n_instructions = n_insns;

  this->functions = (Function**)xcalloc(n_functions + 1, sizeof(Function*));
  this->methods = (JavaMethod**)xcalloc(n_functions + 1, sizeof(JavaMethod*));
  this->n_methods = n_functions;

  /* Setup exported symbols, if they exist */
  ElfSection *expsymsSection = elf->getSection(".cibylexpsyms");
  cibyl_exported_symbol_t *exp_syms = NULL;
  size_t n = 0;

  if (expsymsSection)
    {
      n = expsymsSection->size / sizeof(cibyl_exported_symbol_t);
      exp_syms = (cibyl_exported_symbol_t *)expsymsSection->data;

      panic_if(expsymsSection->size % sizeof(cibyl_exported_symbol_t) != 0,
               "Size of the exported symbols section is wrong: %zd\n",
               expsymsSection->size);

      this->fixupExportedSymbols(exp_syms, n);
    }

  /* Create all functions and methods */
  fn_syms = elf->getFunctions();
  assert(fn_syms);
  int cnt = 0;
  for (i = 0; fn_syms[i]; i++)
    {
      ElfSymbol *sym = fn_syms[i];
      FunctionColocation *coloc;
      Function *fn;

      /* Create a new function with a set of instructions, this
       * will in turn create basic blocks
       */
      if (sym->addr == elf->getEntryPoint())
        fn = new StartFunction(sym->name, this->instructions,
                               (sym->addr - textBase) / 4,
                               (sym->addr - textBase + sym->size) / 4 - 1);
      else
        {
          if (config->pruneUnusedFunctions && !elf->getRelocationBySymbol(sym) &&
              sym->binding != STB_LOCAL)
            {
              /* This is an "orphaned" symbol, just skip it */
              continue;
            }

          fn = new Function(sym->name, this->instructions,
                            (sym->addr - textBase) / 4,
                            (sym->addr - textBase + sym->size) / 4 - 1);
        }
      this->functions[cnt] = fn;
      coloc = FunctionColocation::lookup(fn->getRealName());
      if (coloc)
        coloc->addFunction(fn);
      cnt++;
    }
  this->n_methods = n_functions = cnt;

  this->callTableMethod = new CallTableMethod(n_functions + 1,
                                              exp_syms, n);
  /* Create methods */
  n = 0;
  for (i = 0; i < n_functions; i++)
    {
      Function *fn = this->functions[i];

      /* Add all methods to the call table if we don't optimize this */
      if (!config->optimizeCallTable)
        this->callTableMethod->addFunction(fn);

      /* If this is part of a coloc, skip it */
      FunctionColocation *coloc = FunctionColocation::lookup(fn->getRealName());
      if (coloc)
        continue;

      /* 1-1 mapping */
      this->methods[n] = new JavaMethod(functions, i, i);
      n++;
    }

  /* And create the methods for colocated functions */
  for (i = 0; i < this->n_colocs; i++)
    {
      this->methods[n] = this->colocs[i]->createJavaMethod();
      n++;
    }
  this->n_methods = n;

  /* And the classes */
  this->allocateClasses();
  this->syscalls = (Syscall**)xcalloc(sizeof(Syscall*),
                                      elf->getSection(".cibylstrtab")->size);

  return true;
}

/* Called by the constructor to setup classes */
void Controller::allocateClasses()
{
  int first = 0;
  int last = 0;
  int n = 0;

  this->classes = NULL;
  while(last < this->n_methods)
    {
      char buf[80];
      size_t size = 0;

      this->classes = (JavaClass**)xrealloc(this->classes, (n+1) * sizeof(JavaClass*));
      while (size < config->classSizeLimit &&
             last < this->n_methods)
        {
          size += this->methods[last]->getBytecodeSize();
          last++;
        }

      if (n == 0)
        xsnprintf(buf, 80, "Cibyl%s", ""); /* The first is always "Cibyl" */
      else
        xsnprintf(buf, 80, "Cibyl%d", n);
      this->classes[n] = new JavaClass(buf, this->methods,
                                       first, last-1);

      /* Setup the mapping between methods and classes */
      for (int j = first; j < last; j++) /* last is the last "real" method... */
        {
          JavaMethod *mt = this->methods[j];

          this->m_method_to_class[mt->getName()] = this->classes[n];
        }
      first = last;
      n++;
    }

  /* Add the call table method */
  this->classes = (JavaClass**)xrealloc(this->classes, (n+1) * sizeof(JavaClass*));

  this->classes[n] = new CallTableClass("CibylCallTable", (JavaMethod**)&this->callTableMethod, 0, 0);
  this->m_method_to_class[this->callTableMethod->getName()] = this->classes[n];
  n++;

  this->n_classes = n;
}

Syscall *Controller::getSyscall(uint32_t value)
{
  assert(value < elf->getSection(".cibylstrtab")->size * sizeof(Syscall*));

  /*
   * this->syscalls is a sparse "table" of cached syscalls. It uses
   * the string offset as an index into the table and stores a pointer
   * there. Most of the entries will be empty, so memory is wasted,
   * but this should speed things up a bit.
   */
  if (!this->syscalls[value])
    {
      const char *name = elf->getCibylStrtabString(value);
      cibyl_db_entry_t *p = this->m_syscall_db_table[name];

      panic_if(!p, "No syscall %s:\n"
               "  Are all syscall databases added on the command line (cibyl-syscalls.db)?\n",
               name);

      this->syscalls[value] = new Syscall(p->name, p->nrArgs,
                                          p->returns ? 'I' : 'V' );
      /* Insert into the table for the syscall wrappers */
      this->m_syscall_used_table[p->name] = p;
    }
  return this->syscalls[value];
}


JavaMethod *Controller::getMethodByAddress(uint32_t addr)
{
  /* We only have a couple of classes anyway */
  for (int i = 0; i < this->n_classes; i++)
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

void Controller::setPackageName(const char *name)
{
  size_t len = strlen(name);
  unsigned i;

  this->package_name = name;

  panic_if(len + 2 > sizeof(this->jasmin_package_path),
      "Length of package name (%s) is too large (> %d)\n",
      name, sizeof(this->jasmin_package_path));

  for (i = 0; i < len; i++)
    {
      if (name[i] == '.')
        this->jasmin_package_path[i] = '/';
      else
        this->jasmin_package_path[i] = name[i];
    }
  this->jasmin_package_path[i] = '/';
  this->jasmin_package_path[i + 1] = '\0';
}


Instruction *Controller::getInstructionByAddress(uint32_t address)
{
  if (address < elf->getEntryPoint() ||
      address > elf->getEntryPoint() + this->textSize)
    return NULL;

  return this->instructions[(address - elf->getEntryPoint()) / 4];
}


Instruction *Controller::getBranchTarget(uint32_t address)
{
  return this->getInstructionByAddress(address);
}

void Controller::pushTryStack(Instruction *insn)
{
  panic_if(this->try_stack_top >= N_TRY_STACK_ENTRIES,
      "Pushing too many entries on the stack: %d\n", this->try_stack_top);

  this->try_stack[this->try_stack_top++] = insn;
}

Instruction *Controller::popTryStack()
{
  panic_if(this->try_stack_top <= 0,
      "Popping off empty stack: %d\n", this->try_stack_top);

  this->try_stack_top--;

  Instruction *out = this->try_stack[this->try_stack_top];

  return out;
}

void Controller::lookupDataAddresses(uint32_t *data, int n_entries)
{
  uint32_t text_start = elf->getEntryPoint();
  uint32_t text_end = text_start + this->textSize;

  /* Add labels for data pointing to the text segment */
  for (int n = 0; n < n_entries; n++)
    {
      uint32_t v = be32_to_host(data[n]);

      if (v >= text_start && v < text_end)
        {
          /* Skip things which can not be code addresses */
          if ((v & 0x3) != 0)
            continue;

          bool found_method = false;

          for (int i = 0; i < this->n_classes; i++)
            {
              JavaClass *cl = this->classes[i];
              JavaMethod *mt = cl->getMethodByAddress(v);
              Function *fn;

              if (!mt)
                continue;
              fn = mt->getFunctionByAddress(v);
              panic_if(!fn, "No function for address 0x%x in method %s!\n",
                       v, mt->getName());

              found_method = true;

              /* Add to the call table */
              if (fn->getAddress() == v)
                this->callTableMethod->addFunction(fn);

              /* Something has an address in this method (which can be an address) */
              mt->addJumptabLabel(v);
              this->addJumptabLabel(v);
            }
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
      panic_if(v < 0, "Cannot write padding to outfile in aligned section\n");
      pad--;
    }

  panic_if(fwrite(data, 1, data_len, fp) != data_len,
           "Cannot write data to outfile in aligned section\n");

  return out + data_len;
}

class HiloRelocLimit
{
public:
  HiloRelocLimit()
  {
    this->hi_start = INT_MAX;
    this->hi_end = -1;
    this->lo_start = INT_MAX;
    this->lo_end = -1;
  }

  bool isValid()
  {
    return (this->hi_start != INT_MAX && this->hi_end != -1 &&
            this->lo_start != INT_MAX && this->lo_end != -1);
  }

  int hi_start, hi_end;
  int lo_start, lo_end;
};

void Controller::lookupRelocations(JavaClass *cl)
{
  ElfReloc **relocs = elf->getRelocations();
  int n = elf->getNumberOfRelocations();
  HiloRelocLimit *hilos_per_method;

  /* Initialize hi/lo pairs to -1 */
  hilos_per_method = new HiloRelocLimit[cl->getNumberOfMethods()];

  for (int i = 0; i < n; i++)
    {
      ElfReloc *rel = relocs[i];

      /* If we have a relocation to a function object, add that to
       * the call table */
      if (rel->sym)
        {
          if (rel->sym->type == STT_FUNC && rel->type != R_MIPS_26)
            {
              JavaMethod *mt = this->getMethodByAddress(rel->sym->addr);
              Function *fn;

              /* If this method is not in this class  */
              if (!mt)
                continue;
              fn = mt->getFunctionByAddress(rel->sym->addr);
              panic_if(!fn, "No function for address 0x%x in method %s!\n",
                       rel->sym->addr, mt->getName());

              if (fn->getAddress() == rel->sym->addr)
                this->callTableMethod->addFunction(fn);
            }
          else if(rel->sym->type == STT_SECTION && rel->type == R_MIPS_32)
          {
            JavaMethod *mt = this->getMethodByAddress(rel->addend);
            Function *fn;

            /* If this method is not in this class  */
            if (!mt)
              continue;
	      
            fn = mt->getFunctionByAddress(rel->addend);
            panic_if(!fn, "No function for address 0x%x in method %s!\n",
                     rel->sym->addr, mt->getName());

            if (fn->getAddress() == rel->addend)
              this->callTableMethod->addFunction(fn);
          }
        }
      if (rel->type == R_MIPS_HI16 || rel->type == R_MIPS_LO16)
        {
          int idx;
          JavaMethod *reloc_mt = cl->getMethodByAddress(rel->addr, &idx);

          if (reloc_mt)
            {
              HiloRelocLimit *hilo = &hilos_per_method[idx];

              if (rel->type == R_MIPS_HI16)
                {
                  if (hilo->hi_start > i)
                    hilo->hi_start = i;
                  if (hilo->hi_end < i)
                    hilo->hi_end = i;
                }
              else if (rel->type == R_MIPS_LO16)
                {
                  if (hilo->lo_start > i)
                    hilo->lo_start = i;
                  if (hilo->lo_end < i)
                    hilo->lo_end = i;
                }
            }
        }
    }

  /* Check all possible hi/lo pairs. This might look like a lot of
   * code (and it is!), but the purpose of doing it this complicated
   * is to avoid having to loop through _all_ HI16 and LO16 pairs of
   * relocations for the entire program, which could easily be quite
   * time-consuming.
   *
   * The code (and the above part) instead only loops through all
   * hi/lo pairs in a particular method. There are usually not more
   * than at most 20 of each, which is quite feasible.
   */
  for (int i = 0; i < cl->getNumberOfMethods(); i++)
    {
      HiloRelocLimit *hilo = &hilos_per_method[i];

      if (!hilo->isValid())
        continue;

      /* Loop through each HI/LO pair for this method */
      for (int h = hilo->hi_start; h <= hilo->hi_end; h++)
        {
          ElfReloc *rel_hi = relocs[h];
          Instruction *a, *b;

          a = this->getInstructionByAddress(rel_hi->addr);
          panic_if(!a, "Cannot find instruction for REL_HI at address 0x%x\n",
                   rel_hi->addr);

          if (a->isDelaySlotNop())
            {
              Instruction *parent = this->getInstructionByAddress(rel_hi->addr - 4);
              a = parent->getDelayed();
            }
		
          for (int l = hilo->lo_start; l <= hilo->lo_end; l++)
            {
              ElfReloc *rel_lo = relocs[l];
              int32_t addr;

              b = this->getInstructionByAddress(rel_lo->addr);
              panic_if(!b, "Cannot find instruction for REL_LO at address 0x%x\n",
                       rel_lo->addr);

              if (b->isDelaySlotNop())
                {
                  Instruction *parent = this->getInstructionByAddress(rel_lo->addr - 4);

                  b = parent->getDelayed();
                }

              addr = ((uint32_t)a->getExtra()) << 16;
              switch (b->getOpcode())
                {
                case OP_SW: /* Assume adds for these */
                case OP_SB:
                case OP_SH:
                case OP_LB:
                case OP_LBU:
                case OP_LH:
                case OP_LHU:
                case OP_LW:
                case OP_LWL:
                case OP_SWL:
                case OP_LWR:
                case OP_SWR:
                case OP_ADDI:
                case OP_ADDIU:
                  addr += b->getExtra(); break;
                case OP_ORI:
                  addr |= b->getExtra(); break;
                case OP_XORI:
                  addr ^= b->getExtra(); break;
                case OP_ANDI:
                  addr &= b->getExtra(); break;
                default:
                  emit->warning("Warning: Unknown opcode %d in hilo pair at 0x%08x : 0x%08x\n",
                                b->getOpcode(), rel_hi->addr, rel_lo->addr);
                  break;
                }
              /* Skip things which can not be code addresses */
              if ((addr & 0x3) != 0)
                continue;

              JavaMethod *dst_mt = this->getMethodByAddress(addr);
              if (dst_mt)
                {
                  Function *fn = dst_mt->getFunctionByAddress(addr);

                  panic_if(!fn, "No function for address 0x%x in method %s!\n",
                           addr, dst_mt->getName());

                  this->callTableMethod->addFunction(fn);
                }
            }
        }
    }

  delete hilos_per_method;
}

bool Controller::pass1()
{
  ElfSection *scns[4];
  bool out = true;

  scns[0] = elf->getSection(".data");
  scns[1] = elf->getSection(".rodata");
  scns[2] = elf->getSection(".ctors");
  scns[3] = elf->getSection(".dtors");

  /* Add addresses in the different ELF sections to the lookup tables */
  for (unsigned int j = 0; j < sizeof(scns) / sizeof(ElfSection*); j++)
    {
      if (scns[j])
        this->lookupDataAddresses((uint32_t*)scns[j]->data,
                                  scns[j]->size / sizeof(uint32_t));
    }

  for (int i = 0; i < this->n_classes; i++)
    {
      JavaClass *cl = this->classes[i];

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
  ElfSection *scns[4];
  SyscallWrapperGenerator *syscallWrappers;
  char path[2048];
  bool out = true;
  uint32_t addr = 0;
  FILE *fp;

  scns[0] = elf->getSection(".data");
  scns[1] = elf->getSection(".rodata");
  scns[2] = elf->getSection(".ctors");
  scns[3] = elf->getSection(".dtors");

  xsnprintf(path, 2048, "%s/%s", this->dstdir, this->getJasminPackagePath());

  /* Output the data sections to a file */
  fp = open_file_in_dir(this->dstdir, "program.data.bin", "w");
  for (unsigned int j = 0; j < sizeof(scns) / sizeof(ElfSection*); j++)
    {
      if (scns[j])
        addr = this->addAlignedSection(addr, fp, scns[j]->data,
                                       scns[j]->size, scns[j]->align);
    }
  fclose(fp);

  for (int i = 0; i < this->n_classes; i++)
    {
      emit->setOutputFile(open_file_in_dir(path,
                                           this->classes[i]->getFileName(), "w"));

      if (this->classes[i]->pass2() != true)
        out = false;
      emit->closeOutputFile();
    }

  syscallWrappers = new SyscallWrapperGenerator(this->defines, strdup(path),
                                                this->n_syscall_dirs, this->syscall_dirs,
                                                this->n_syscall_sets, this->syscall_sets,
                                                this->m_syscall_used_table);
  syscallWrappers->pass2();

  return out;
}

void Controller::addColocation(const char *str)
{
  int n = this->n_colocs;

  this->n_colocs++;
  this->colocs = (FunctionColocation**)xrealloc(this->colocs,
                                                sizeof(FunctionColocation*) * this->n_colocs);
  this->colocs[n] = new FunctionColocation(str);
}

Controller *controller;
Config *config;
CibylElf *elf;

static void usage()
{
  printf("\nUsage: xcibyl-translator config:<...> dst-dir elf-file syscall-database...\n"
         "\n"
         "Where config is the configuration to use, dst-dir is the destination\n"
         "directory to put translated files in, elf-file the input MIPS binary file,\n"
         "syscall-database is one or more cibyl-syscalls.db files. The config options\n"
         "are:\n\n"

         "   trace_start=0x...       The first address of instruction tracing\n"
         "   trace_end=0x...         The last address of instruction tracing\n"
         "   trace_stores=0/1        Set to 1 to trace memory stores\n"
         "   thread_safe=0/1         Set to 1 to generate thread-safe code (default 0)\n"
         "   class_size_limit=N      Set the size limit for classes (class split size)\n"
         "   call_table_hierarchy=N  Generate a call table hierarchy with N methods (default 1)\n"
         "   call_table_classes=N    Generate several call table classes\n"
         "   prune_call_table=0/1    Set to 1 to prune unused indirect function calls\n"
         "   optimize_partial_memory_operations=0/1  Set to 1 to generate subroutine calls for\n"
         "                           lb/lh/sb/sh (default 0)\n"
         "   prune_unused_functions=0/1  Prune unused functions from the call table\n"
         "   colocate_functions=FN1;FN2;... Colocate functions FN1... in a single method\n"
         "   package_name=NAME       Set Java package name (default: unnamed)\n"
         );
  exit(1);
}

static void parse_config(Controller *cntr, Config *cfg, const char *config_str)
{
  char *cpy = xstrdup(config_str);
  char *p;

  /* A series of "trace_start=0x12414,trace_end=0x15551,..." No spaces */
  p = strtok(cpy, ",");
  while (p)
    {
      char *value = strstr(p, "=");
      char *endp;
      int int_val;

      if (!value || strlen(value) < 2)
        usage();
      value[0] = '\0'; /* p is now the key */
      value++;         /* And value points to the value */

      int_val = strtol(value, &endp, 0);
      if (endp == value)
        {
          int_val = -1;
        }

      /* Now match the keys*/
      if (strcmp(p, "trace_start") == 0)
        cfg->traceRange[0] = int_val;
      else if (strcmp(p, "trace_end") == 0)
        cfg->traceRange[1] = int_val;
      else if (strcmp(p, "trace_stores") == 0)
        cfg->traceStores = int_val == 0 ? false : true;
      else if (strcmp(p, "thread_safe") == 0)
        cfg->threadSafe = int_val == 0 ? false : true;
      else if (strcmp(p, "prune_call_table") == 0)
        cfg->optimizeCallTable = int_val == 0 ? false : true;
      else if (strcmp(p, "optimize_partial_memory_operations") == 0)
        cfg->optimizePartialMemoryOps = int_val == 0 ? false : true;
      else if (strcmp(p, "optimize_prune_stack_stores") == 0)
        cfg->optimizePruneStackStores = int_val == 0 ? false : true;
      else if (strcmp(p, "optimize_function_return_arguments") == 0)
        cfg->optimizeFunctionReturnArguments = int_val == 0 ? false : true;
      else if (strcmp(p, "prune_unused_functions") == 0)
        cfg->pruneUnusedFunctions = int_val == 0 ? false : true;
      else if (strcmp(p, "class_size_limit") == 0)
        cfg->classSizeLimit = int_val;
      else if (strcmp(p, "call_table_hierarchy") == 0)
        cfg->callTableHierarchy = int_val;
      else if (strcmp(p, "call_table_classes") == 0)
        cfg->callTableClasses = int_val;
      else if (strcmp(p, "colocate_functions") == 0)
        cntr->addColocation(value);
      else if (strcmp(p, "package_name") == 0)
        cntr->setPackageName(value);
      else
        usage();

      p = strtok(NULL, ",");
    }

  if (cfg->traceRange[1] < cfg->traceRange[0])
    {
      fprintf(stderr, "Trace start is after trace end!\n");
      usage();
    }

  free(p);
}

int main(int argc, const char **argv)
{
  const char **defines = (const char **)xcalloc(argc, sizeof(const char*));
  int n, n_defines = 0;

  if (argc < 5)
    {
      fprintf(stderr, "Too few arguments\n");

      usage();
      return 1;
    }
  if (!strstr(argv[1], "config:"))
    {
      fprintf(stderr, "Error: expecting configuration first in argument list\n");
      usage();
    }
  /* Setup configuration */
  config = new Config();

  /* Setup defines */
  for (n = 2; n < argc && strncmp(argv[n], "-D", 2) == 0; n++)
    {
      defines[n_defines++] = argv[n];
    }

  emit = new Emit();

  regalloc = new RegisterAllocator();
  controller = new Controller(argv[0], defines, argv[n], argv[n+1],
                              argc - n - 2, &argv[n + 2]);
  parse_config(controller, config, argv[1] + strlen("config:"));

  controller->pass0();
  controller->pass1();
  controller->pass2();

  return 0;
}
