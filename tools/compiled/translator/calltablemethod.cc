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

bool CallTableMethod::pass2()
{
  emit->generic("class CibylCallTable {\n");

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

  emit->generic("  public static final int call(int address, int sp, int a0, int a1, int a2, int a3) throws Exception {\n"
                "    int v0 = 0;\n"
                "    switch(address) {\n"
                );

  /* For each method, output a call to it */
  for (int i = 0; i < this->n_methods; i++)
    {
      JavaMethod *mt = this->methods[i];
      const char *comma = "";

      emit->generic("      case 0x%x:  ", mt->getAddress());
      if (mt->clobbersReg( R_V0 ))
        emit->generic("v0 = ");
      emit->generic("Cibyl.%s(", mt->getName()); /* FIXME! Don't use static class name */

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
                "    return v0;\n"
                "  }\n"
                "}\n"
                );
  return true;
}
