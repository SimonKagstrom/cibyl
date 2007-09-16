/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      cibyl-syscalls.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Defs for syscalls
 *
 * $Id: cibyl-syscall_defs.h 13855 2007-02-24 08:48:01Z ska $
 *
 ********************************************************************/
#ifndef __CIBYL_SYSCALL_DEFS_H__
#define __CIBYL_SYSCALL_DEFS_H__

#if defined(__cplusplus)
extern "C" {
#endif

#define _syscall0(type,name) \
type name(void) \
{ \
	unsigned long out; \
	\
	__asm__ volatile ( \
        ".set  push\n.set  noreorder\n" \
        ".pushsection .cibylstrtab, \"aS\"\n" \
        "1: .asciz \"" #name "\"\n" \
        ".popsection\n" \
        ".long 1b\n" \
	".set\tpop\n" \
        "move %[out], $2\n"\
	: [out]"=d" (out) \
	: \
        : "memory", "$2" \
	); \
	\
	return (type) out; \
}

#define _syscall1(type,name,atype,a) \
type name(atype a) \
{ \
	unsigned long out; \
	\
	__asm__ volatile ( \
        ".set  push\n.set  noreorder\n" \
        ".short 0xfefe\n" \
        ".short %1\n" \
        ".pushsection .cibylstrtab, \"aS\"\n" \
        "1: .asciz \"" #name "\"\n" \
        ".popsection\n" \
        ".long 1b\n" \
	".set\tpop\n" \
	"move %[out], $2\n"\
	: [out]"=d" (out) \
	: "r"(a) \
        : "memory", "$2" \
	); \
	\
	return (type) out; \
}

#define _syscall2(type,name,atype,a,btype,b) \
type name(atype a, btype b) \
{ \
	unsigned long out; \
	\
	__asm__ volatile ( \
        ".set  push\n.set  noreorder\n" \
        ".short 0xfefe\n" \
        ".short %1\n" \
        ".short 0xfefe\n" \
        ".short %2\n" \
        ".pushsection .cibylstrtab, \"aS\"\n" \
        "1: .asciz \"" #name "\"\n" \
        ".popsection\n" \
        ".long 1b\n" \
	".set\tpop\n" \
	"move %[out], $2\n"\
	: [out]"=d" (out) \
	: "r"(a), "r"(b) \
        : "memory", "$2" \
	); \
	\
	return (type) out; \
}

#define _syscall3(type,name,atype,a,btype,b,ctype,c) \
type name(atype a, btype b, ctype c) \
{ \
	unsigned long out; \
	\
	__asm__ volatile ( \
        ".set  push\n.set  noreorder\n" \
        ".short 0xfefe\n" \
        ".short %1\n" \
        ".short 0xfefe\n" \
        ".short %2\n" \
        ".short 0xfefe\n" \
        ".short %3\n" \
        ".pushsection .cibylstrtab, \"aS\"\n" \
        "1: .asciz \"" #name "\"\n" \
        ".popsection\n" \
        ".long 1b\n" \
	".set\tpop\n" \
	"move %[out], $2\n"\
	: [out]"=d" (out) \
	: "r"(a), "r"(b), "r"(c) \
        : "memory", "$2" \
	); \
	\
	return (type) out; \
}

#define _syscall4(type,name,atype,a,btype,b,ctype,c,dtype,d) \
type name(atype a, btype b, ctype c, dtype d) \
{ \
	unsigned long out; \
	\
	__asm__ volatile ( \
        ".set  push\n.set  noreorder\n" \
        ".short 0xfefe\n" \
        ".short %1\n" \
        ".short 0xfefe\n" \
        ".short %2\n" \
        ".short 0xfefe\n" \
        ".short %3\n" \
        ".short 0xfefe\n" \
        ".short %4\n" \
        ".pushsection .cibylstrtab, \"aS\"\n" \
        "1: .asciz \"" #name "\"\n" \
        ".popsection\n" \
        ".long 1b\n" \
	".set\tpop\n" \
	"move %[out], $2\n"\
	: [out]"=d" (out) \
	: "r"(a), "r"(b), "r"(c), "r"(d) \
        : "memory", "$2" \
	); \
	\
	return (type) out; \
}

#define _syscall5(type,name,atype,a,btype,b,ctype,c,dtype,d,etype,e) \
type name(atype a, btype b, ctype c, dtype d, etype e) \
{ \
	unsigned long out; \
	\
	__asm__ volatile ( \
        ".set  push\n.set  noreorder\n" \
        ".short 0xfefe\n" \
        ".short %1\n" \
        ".short 0xfefe\n" \
        ".short %2\n" \
        ".short 0xfefe\n" \
        ".short %3\n" \
        ".short 0xfefe\n" \
        ".short %4\n" \
        ".short 0xfefe\n" \
        ".short %5\n" \
        ".pushsection .cibylstrtab, \"aS\"\n" \
        "1: .asciz \"" #name "\"\n" \
        ".popsection\n" \
        ".long 1b\n" \
	".set\tpop\n" \
	"move %[out], $2\n"\
	: [out]"=d" (out) \
	: "r"(a), "r"(b), "r"(c), "r"(d), "r"(e) \
        : "memory", "$2" \
	); \
	\
	return (type) out; \
}


#define _syscall6(type,name,atype,a,btype,b,ctype,c,dtype,d,etype,e,ftype,f) \
type name(atype a, btype b, ctype c, dtype d, etype e, ftype f) \
{ \
	unsigned long out; \
	\
	__asm__ volatile ( \
        ".set  push\n.set  noreorder\n" \
        ".short 0xfefe\n" \
        ".short %1\n" \
        ".short 0xfefe\n" \
        ".short %2\n" \
        ".short 0xfefe\n" \
        ".short %3\n" \
        ".short 0xfefe\n" \
        ".short %4\n" \
        ".short 0xfefe\n" \
        ".short %5\n" \
        ".short 0xfefe\n" \
        ".short %6\n" \
        ".pushsection .cibylstrtab, \"aS\"\n" \
        "1: .asciz \"" #name "\"\n" \
        ".popsection\n" \
        ".long 1b\n" \
	".set\tpop\n" \
	"move %[out], $2\n"\
	: [out]"=d" (out) \
	: "r"(a), "r"(b), "r"(c), "r"(d), "r"(e), "r"(f) \
        : "memory", "$2" \
	); \
	\
	return (type) out; \
}

#define _syscall7(type,name,atype,a,btype,b,ctype,c,dtype,d,etype,e,ftype,f,gtype,g) \
type name(atype a, btype b, ctype c, dtype d, etype e, ftype f, gtype g) \
{ \
	unsigned long out; \
	\
	__asm__ volatile ( \
        ".set  push\n.set  noreorder\n" \
        ".short 0xfefe\n" \
        ".short %1\n" \
        ".short 0xfefe\n" \
        ".short %2\n" \
        ".short 0xfefe\n" \
        ".short %3\n" \
        ".short 0xfefe\n" \
        ".short %4\n" \
        ".short 0xfefe\n" \
        ".short %5\n" \
        ".short 0xfefe\n" \
        ".short %6\n" \
        ".short 0xfefe\n" \
        ".short %7\n" \
        ".pushsection .cibylstrtab, \"aS\"\n" \
        "1: .asciz \"" #name "\"\n" \
        ".popsection\n" \
        ".long 1b\n" \
	".set\tpop\n" \
	"move %[out], $2\n"\
	: [out]"=d" (out) \
	: "r"(a), "r"(b), "r"(c), "r"(d), "r"(e), "r"(f), "r"(g) \
        : "memory", "$2" \
	); \
	\
	return (type) out; \
}

#define _syscall8(type,name,atype,a,btype,b,ctype,c,dtype,d,etype,e,ftype,f,gtype,g,htype,h) \
type name(atype a, btype b, ctype c, dtype d, etype e, ftype f, gtype g, htype h) \
{ \
	unsigned long out; \
	\
	__asm__ volatile ( \
        ".set  push\n.set  noreorder\n" \
        ".short 0xfefe\n" \
        ".short %1\n" \
        ".short 0xfefe\n" \
        ".short %2\n" \
        ".short 0xfefe\n" \
        ".short %3\n" \
        ".short 0xfefe\n" \
        ".short %4\n" \
        ".short 0xfefe\n" \
        ".short %5\n" \
        ".short 0xfefe\n" \
        ".short %6\n" \
        ".short 0xfefe\n" \
        ".short %7\n" \
        ".short 0xfefe\n" \
        ".short %8\n" \
        ".pushsection .cibylstrtab, \"aS\"\n" \
        "1: .asciz \"" #name "\"\n" \
        ".popsection\n" \
        ".long 1b\n" \
	".set\tpop\n" \
	"move %[out], $2\n"\
	: [out]"=d" (out) \
	: "r"(a), "r"(b), "r"(c), "r"(d), "r"(e), "r"(f), "r"(g), "r"(h) \
        : "memory", "$2" \
	); \
	\
	return (type) out; \
}


#define _syscall9(type,name,atype,a,btype,b,ctype,c,dtype,d,etype,e,ftype,f,gtype,g,htype,h,itype,i) \
type name(atype a, btype b, ctype c, dtype d, etype e, ftype f, gtype g, htype h,itype i) \
{ \
	unsigned long out; \
	\
	__asm__ volatile ( \
        ".set  push\n.set  noreorder\n" \
        ".short 0xfefe\n" \
        ".short %1\n" \
        ".short 0xfefe\n" \
        ".short %2\n" \
        ".short 0xfefe\n" \
        ".short %3\n" \
        ".short 0xfefe\n" \
        ".short %4\n" \
        ".short 0xfefe\n" \
        ".short %5\n" \
        ".short 0xfefe\n" \
        ".short %6\n" \
        ".short 0xfefe\n" \
        ".short %7\n" \
        ".short 0xfefe\n" \
        ".short %8\n" \
        ".short 0xfefe\n" \
        ".short %9\n" \
        ".pushsection .cibylstrtab, \"aS\"\n" \
        "1: .asciz \"" #name "\"\n" \
        ".popsection\n" \
        ".long 1b\n" \
	".set\tpop\n" \
	"move %[out], $2\n"\
	: [out]"=d" (out) \
	: "r"(a), "r"(b), "r"(c), "r"(d), "r"(e), "r"(f), "r"(g), "r"(h), "r"(i) \
        : "memory", "$2" \
	); \
	\
	return (type) out; \
}



#define _syscall10(type,name,atype,a,btype,b,ctype,c,dtype,d,etype,e,ftype,f,gtype,g,htype,h,itype,i,jtype,j) \
type name(atype a, btype b, ctype c, dtype d, etype e, ftype f, gtype g, htype h, itype i, jtype j) \
{ \
	unsigned long out; \
	\
	__asm__ volatile ( \
        ".set  push\n.set  noreorder\n" \
        ".short 0xfefe\n" \
        ".short %1\n" \
        ".short 0xfefe\n" \
        ".short %2\n" \
        ".short 0xfefe\n" \
        ".short %3\n" \
        ".short 0xfefe\n" \
        ".short %4\n" \
        ".short 0xfefe\n" \
        ".short %5\n" \
        ".short 0xfefe\n" \
        ".short %6\n" \
        ".short 0xfefe\n" \
        ".short %7\n" \
        ".short 0xfefe\n" \
        ".short %8\n" \
        ".short 0xfefe\n" \
        ".short %9\n" \
        ".short 0xfefe\n" \
        ".short %10\n" \
        ".pushsection .cibylstrtab, \"aS\"\n" \
        "1: .asciz \"" #name "\"\n" \
        ".popsection\n" \
        ".long 1b\n" \
	".set\tpop\n" \
	"move %[out], $2\n"\
	: [out]"=d" (out) \
	: "r"(a), "r"(b), "r"(c), "r"(d), "r"(e), "r"(f), "r"(g), "r"(h), "r"(i), "r"(j) \
        : "memory", "$2" \
	); \
	\
	return (type) out; \
}
#if defined(__cplusplus)
}
#endif

#endif /* !__NOPH_SYSCALL_DEFS_H__ */
