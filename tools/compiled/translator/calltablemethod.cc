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

CallTableMethod::CallTableMethod(int maxMethods, cibyl_exported_symbol_t *exp_syms,
                                 size_t n_exp_syms) : JavaMethod(NULL, 0, 0)
{
  this->n_methods = 0;
  this->methods = (JavaMethod**)xcalloc(sizeof(JavaMethod*), maxMethods);
  this->method_table = ght_create(maxMethods);

  /* Exported symbols */
  this->exp_syms = exp_syms;
  this->n_exp_syms = n_exp_syms;

  memset(this->registerUsage, 0, sizeof(this->registerUsage));
}

void CallTableMethod::addMethod(JavaMethod *method)
{
  uint32_t addr = method->getAddress();

  if ( ght_get(this->method_table,
               sizeof(uint32_t), (void*)&addr) != NULL )
    {
      /* Already exists, don't insert again */
      return;
    }
  ght_insert(this->method_table, (void*)method,
             sizeof(uint32_t), (void*)&addr);

  this->methods[this->n_methods++] = method;
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
      JavaMethod *mt = this->methods[i];
      JavaClass *cl;
      const char *comma = "";

      panic_if(!mt, "this->methods[%d] is NULL!\n", i);
      cl = controller->getClassByMethodName(mt->getName());

      panic_if(!cl, "Method %s has no class mapping!\n",
               mt->getName());

      emit->generic("      case 0x%x:  ", mt->getAddress());
      if (config->threadSafe)
        {
          if (mt->clobbersReg( R_V0 ) && mt->clobbersReg( R_V1) )
            emit->generic("ret = ");
          else if (mt->clobbersReg( R_V0 ) || mt->clobbersReg( R_V1) ) /* Only one */
            emit->generic("ret = (int)");
          /* else nothing */
        }
      else if (mt->clobbersReg( R_V0 ))
        emit->generic("ret = ");
      emit->generic("%s.%s(", cl->getName(), mt->getName());

      /* Pass registers */
      if (mt->clobbersReg( R_SP ))
        { emit->generic("sp"); comma = ","; }
      if (mt->clobbersReg( R_A0 ))
        { emit->generic("%s a0", comma); comma = ","; }
      if (mt->clobbersReg( R_A1 ))
        { emit->generic("%s a1", comma); comma = ","; }
      if (mt->clobbersReg( R_A2 ))
        { emit->generic("%s a2", comma); comma = ","; }
      if (mt->clobbersReg( R_A3 ))
        emit->generic("%s a3", comma);
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
  int methods_per_level = this->n_methods / n;
  const char *else_str = "";
  char buf[80];
  unsigned int i;

  panic_if(n >= (unsigned int)this->n_methods,
           "%d methods in the hierarchy and only %d methods totally\n",
           n, this->n_methods);

  emit->generic("  public static final int call(int address, int sp, int a0, int a1, int a2, int a3) throws Exception {\n");
  for (i = 0; i < n-1; i++)
    {
      JavaMethod *first_mt = this->methods[i * methods_per_level ];
      JavaMethod *last_mt = this->methods[(i+1) * methods_per_level ];

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
      this->generateMethod(buf, i * methods_per_level, (i+1) * methods_per_level);
    }
  xsnprintf(buf, 80, "call%d", i);
  this->generateMethod(buf, i * methods_per_level, this->n_methods);
}

bool CallTableMethod::pass2()
{
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

  if (config->callTableHierarchy > 1)
    this->generateHierarchy(config->callTableHierarchy);
  else
    this->generateMethod("call", 0, this->n_methods);

  return true;
}
