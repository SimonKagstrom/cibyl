/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      calltablemethod.cc
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   The special call table
 *
 * $Id:$
 *
 ********************************************************************/
#include <javamethod.hh>
#include <javamethod.hh>
#include <controller.hh>
#include <config.hh>
#include <emit.hh>

CallTableMethod::CallTableMethod(int maxFunctions, cibyl_exported_symbol_t *exp_syms,
                                 size_t n_exp_syms) : JavaMethod(NULL, 0, 0)
{
  this->n_functions = 0;
  this->functions = (Function**)xcalloc(sizeof(Function*), maxFunctions);

  /* Exported symbols */
  this->exp_syms = exp_syms;
  this->n_exp_syms = n_exp_syms;

  memset(this->registerUsage, 0, sizeof(this->registerUsage));
}

void CallTableMethod::addFunction(Function *fn)
{
  uint32_t addr = fn->getAddress();
  int i,j;

  if (this->m_function_table[addr])
    {
      /* Already exists, don't insert again */
      return;
    }
  this->m_function_table[addr] = fn;

  /* Find correct position in function list, based on sorting by address */
  for(i=0; i<this->n_functions; i++)
    {
      if(this->functions[i]->getAddress() > addr)
	break;
    }

  /* Shift remaining functions up by one to make room for new entry */
  for(j=this->n_functions; j>i; j--)
   {
      this->functions[j] = this->functions[j-1];
   }

  this->functions[i] = fn;  
  this->n_functions++;
}

bool CallTableMethod::pass1()
{
  this->registerUsage[ R_SP ] = 1;
  this->registerUsage[ R_A0 ] = 1;
  this->registerUsage[ R_A1 ] = 1;
  this->registerUsage[ R_A2 ] = 1;
  this->registerUsage[ R_A3 ] = 1;
  this->registerUsage[ R_V0 ] = 1;
  this->registerUsage[ R_V1 ] = 1;

  /* Clobbers both registers */
  this->setReturnSize(2);

  return true;
}

void CallTableMethod::generateMethod(const char *name,
                                     int start, int end)
{
  const char *ret_type = config->threadSafe ? "long" : "int";

  emit->generic("  public static final %s %s(int address, int sp, int a0, int a1, int a2, int a3) throws Exception {\n"
                "    %s ret = 0;\n"
                "    switch(address) {\n",
                ret_type, name, ret_type );

  /* For each method, output a call to it */
  for (int i = start; i < end; i++)
    {
      Function *fn = this->functions[i];
      JavaMethod *mt = controller->getMethodByAddress(fn->getAddress());
      JavaClass *cl;
      const char *comma = "";

      panic_if(!mt, "this->methods[%d] is NULL!\n", i);
      cl = controller->getClassByMethodName(mt->getName());

      panic_if(!cl, "Method %s has no class mapping!\n",
               mt->getName());

      emit->generic("      case 0x%x:  ", fn->getAddress());
      if (config->threadSafe)
        {
          if (mt->returnSize() == 2)
            emit->generic("ret = ");
          else if (mt->returnSize() == 1) /* Only one */
            emit->generic("ret = (int)");
          /* else nothing */
        }
      else if (mt->returnSize() >= 1)
        emit->generic("ret = ");
      emit->generic("%s.%s(", cl->getName(), mt->getName());

      /* Pass registers */
      void *it;
      for (MIPS_register_t reg = mt->getFirstRegisterToPass(&it);
          reg != R_ZERO;
          reg = mt->getNextRegisterToPass(&it))
        {
          if (reg == R_SP)
            { emit->generic("sp"); comma = ","; }
          if (reg == R_FNA)
            {
              int idx = mt->getFunctionIndexByAddress(fn->getAddress());

              panic_if(idx < 0, "Could not find function index for function %s in method %s",
                  fn->getName(), mt->getName());
              emit->generic("%s %d", comma, idx);
            }
          if (reg == R_A0)
            { emit->generic("%s a0", comma); comma = ","; }
          if (reg == R_A1)
            { emit->generic("%s a1", comma); comma = ","; }
          if (reg == R_A2)
            { emit->generic("%s a2", comma); comma = ","; }
          if (reg == R_A3)
            { emit->generic("%s a3", comma); comma = ","; }
        }
      emit->generic("); break;\n");
    }

  emit->generic("      default:\n"
                "         throw new Exception(\"Call to unknown location \" + Integer.toHexString(address));\n"
                "    }\n"
                "    return ret;\n"
                "  }\n"
                );
}

void CallTableMethod::generateHierarchy(unsigned int n)
{
  int functions_per_level = this->n_functions / n;
  const char *else_str = "";
  char buf[80];
  unsigned int i;

  panic_if(n >= (unsigned int)this->n_functions,
           "%d methods in the hierarchy and only %d methods totally\n",
           n, this->n_functions);

  emit->generic("  public static final int call(int address, int sp, int a0, int a1, int a2, int a3) throws Exception {\n");
  for (i = 0; i < n-1; i++)
    {
      Function *first_fn = this->functions[i * functions_per_level ];
      Function *last_fn = this->functions[(i+1) * functions_per_level ];
      JavaMethod *first_mt = controller->getMethodByAddress(first_fn->getAddress());
      JavaMethod *last_mt = controller->getMethodByAddress(last_fn->getAddress());

      xsnprintf(buf, 80, "call%d", i);

      emit->generic("    %s if (address >= 0x%x && address < 0x%x) \n"
                    "       return CibylCallTable.%s(address, sp, a0, a1, a2, a3);\n",
                    else_str, first_mt->getAddress(), last_mt->getAddress(), buf);
      else_str = "else";
    }
  /* And the last */
  xsnprintf(buf, 80, "call%d", i);
  emit->generic("    %s \n"
                "       return CibylCallTable.%s(address, sp, a0, a1, a2, a3);\n",
                else_str, buf);

  emit->generic("  }\n\n");

  /* And actually generate the methods */
  for (i = 0; i < n-1; i++)
    {
      xsnprintf(buf, 80, "call%d", i);
      this->generateMethod(buf, i * functions_per_level, (i+1) * functions_per_level);
    }
  xsnprintf(buf, 80, "call%d", i);
  this->generateMethod(buf, i * functions_per_level, this->n_functions);
}

bool CallTableMethod::pass2()
{
  unsigned int functions_per_class = this->n_functions / config->callTableClasses;

  /* If it exists, generate a table of exported symbols */
  if (this->exp_syms)
    {
      emit->generic(" public static final int getAddressByName(String name) throws Exception {\n");

      for (unsigned int i = 0; i < this->n_exp_syms; i++)
        {
          emit->generic("    if ( name == \"%s\") return 0x%x;\n",
                        elf->getCibylStrtabString(this->exp_syms[i].name),
                        this->exp_syms[i].addr);
        }

      emit->generic("    else throw new Exception(name + \" is not exported \");\n"
                    " }\n\n");
    }


  panic_if(config->callTableClasses != 1 && config->callTableHierarchy != 1,
      "Setting both number of call table classes (%d) and call table hierarchy (%d)\n"
      "is not allowed\n", config->callTableClasses, config->callTableHierarchy);

  if (config->callTableHierarchy > 1)
    this->generateHierarchy(config->callTableHierarchy);
  else
    {
      FILE *old_fp = emit->getOutputFile();

      if ( config->callTableClasses > 1 )
        {
          /* First generate a class that just calls into the "correct" one */

          emit->generic("  public static final int call(int address, int sp, int a0, int a1, int a2, int a3) throws Exception {\n");
          for ( unsigned int i = 0; i < config->callTableClasses; i++ )
            {
              Function *first_fn = this->functions[i * functions_per_class];
              Function *last_fn = this->functions[(i + 1) * functions_per_class];

              if (i < config->callTableClasses - 1)
                emit->generic("    %s ( address >= 0x%08x && address < 0x%08x)\n"
                              "        return CibylCallTable%d.call(address, sp, a0, a1, a2, a3);\n",
                              i == 0 ? "if" : "else if",
                              first_fn->getAddress(), last_fn->getAddress(), i);
              else /* The last */
                emit->generic("    else\n"
                              "        return CibylCallTable%d.call(address, sp, a0, a1, a2, a3);\n",
                              i);

            }
          emit->generic("  }\n\n");
        }
      for ( unsigned int i = 0; i < config->callTableClasses; i++ )
        {
          /* Add a new class if there are multiple call table classes (yes, hack) */
          if ( config->callTableClasses > 1 )
            {
              char buf[80];

              xsnprintf(buf, 80, "CibylCallTable%d.java", i);
              emit->setOutputFile(open_file_in_dir(controller->getDstDir(),
                                                   buf, "w"));
              emit->generic("class CibylCallTable%d {\n", i);
            }

          this->generateMethod("call", i * functions_per_class,
              (i + 1) * functions_per_class);

          if ( config->callTableClasses > 1 )
            {
              emit->generic("}\n");
              emit->closeOutputFile();
            }
        }
      /* Take back the old output file */
      emit->setOutputFile(old_fp);
    }

  return true;
}
