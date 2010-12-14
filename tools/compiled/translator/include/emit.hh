/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      emit.hh
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Emitter
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __EMIT_HH__
#define __EMIT_HH__

#include <stdio.h>
#include <stdint.h>
#include <registerallocator.hh>

class Emit
{
public:
  Emit();

  void bc_comment(const char *what) { this->output("; "); this->write(what); }

  void bc_generic_insn(const char *what) { this->writeIndent(what); }

  void bc_label(const char *what, ...);

  void bc_label(uint32_t addr)
  {
    this->bc_label("L_%x", addr);
  }

  void bc_checkcast(const char *what) { this->writeIndent("checkcast %s", what); }

  void bc_goto(uint32_t dst) { this->writeIndent("goto L_%x", dst); }

  void bc_goto_w(uint32_t dst) { this->writeIndent("goto_w L_%x", dst); }

  void bc_goto(const char *what, ...);

  void bc_jsr(const char *what, ...);

  void bc_condbranch(const char *what, ...);

  void bc_pushconst(int32_t nr);

  void bc_pushconst_u(uint32_t nr);

  void bc_pushconst_l(uint64_t nr);
  
  void bc_ret(MIPS_register_t reg);

  void bc_astore(MIPS_register_t reg);

  void bc_aaload() { this->writeIndent("aaload"); }

  void bc_athrow() { this->writeIndent("athrow"); }

  void bc_pushregister(MIPS_register_t reg);

  void bc_popregister(MIPS_register_t reg);

  void bc_pushindex(MIPS_register_t reg, int32_t extra);

  void bc_pushaddress(MIPS_register_t reg, int32_t extra);

  void bc_getstatic(const char *what, ...);

  void bc_putstatic(const char *what, ...);

  void bc_aload(int nr);
  
  void bc_iload(int n);

  void bc_istore(int n);

  void bc_ldc(char *str) { this->writeIndent("ldc \"%s\"", str); }

  void bc_iadd() { this->writeIndent("iadd"); }

  void bc_iinc(MIPS_register_t reg, int extra);

  void bc_isub() { this->writeIndent("isub"); }

  void bc_invokestatic(const char *what, ...);

  void bc_invokevirtual(const char *what, ...);

  void bc_lookupswitch(int n, uint32_t *table, const char *def);

  void bc_tableswitch(int first, int n, uint32_t *table, const char *def);

  void bc_iushr() { this->writeIndent("iushr"); }

  void bc_ishr() { this->writeIndent("ishr"); }

  void bc_ishl() { this->writeIndent("ishl"); }

  void bc_lshl() { this->writeIndent("lshl"); }

  void bc_lshr() { this->writeIndent("lshr"); }

  void bc_lushr() { this->writeIndent("lushr"); }

  void bc_imul() { this->writeIndent("imul"); }

  void bc_idiv() { this->writeIndent("idiv"); }

  void bc_irem() { this->writeIndent("irem"); }

  void bc_ineg() { this->writeIndent("ineg"); }

  void bc_iand() { this->writeIndent("iand"); }

  void bc_ior() { this->writeIndent("ior"); }

  void bc_lor() { this->writeIndent("lor"); }

  void bc_land() { this->writeIndent("land"); }

  void bc_ixor() { this->writeIndent("ixor"); }

  void bc_lcmp() { this->writeIndent("lcmp"); }

  void bc_lmul() { this->writeIndent("lmul"); }

  void bc_ldiv() { this->writeIndent("ldiv"); }

  void bc_lrem() { this->writeIndent("lrem"); }

  void bc_dup() { this->writeIndent("dup"); }

  void bc_dup_x1() { this->writeIndent("dup_x1"); }

  void bc_dup_x2() { this->writeIndent("dup_x2"); }

  void bc_dup2() { this->writeIndent("dup2"); }

  void bc_pop() { this->writeIndent("pop"); }

  void bc_pop2() { this->writeIndent("pop2"); }

  void bc_i2l() { this->writeIndent("i2l"); }

  void bc_l2i() { this->writeIndent("l2i"); }

  void bc_i2b() { this->writeIndent("i2b"); }

  void bc_i2c() { this->writeIndent("i2c"); }

  void bc_i2s() { this->writeIndent("i2s"); }

  void bc_i2f() { this->writeIndent("i2f"); }

  void bc_f2i() { this->writeIndent("f2i"); }

  void bc_swap() { this->writeIndent("swap"); }

  void bc_iaload() { this->writeIndent("iaload"); }

  void bc_iastore() { this->writeIndent("iastore"); }

  void bc_ireturn() { this->writeIndent("ireturn"); }

  void bc_lreturn() { this->writeIndent("lreturn"); }

  void bc_return() { this->writeIndent("return"); }

  void bc_if_icmpne(const char *what, ...);

  void bc_if_icmpeq(const char *what, ...);

  void error(const char *dst, ...);

  void warning(const char *dst, ...);

  void setOutputFile(FILE *fp);

  void closeOutputFile();

  FILE *getOutputFile() { return this->fp; }

  void output(const char *what);

  void generic(const char *what, ...);

private:
  void bc_load_store_helper(const char *type, int nr);

  void write(const char *dst, ...);

  void writeIndent(const char *dst, ...);

  FILE *fp;
};

extern Emit *emit;

#endif /* !__EMIT_HH__ */
