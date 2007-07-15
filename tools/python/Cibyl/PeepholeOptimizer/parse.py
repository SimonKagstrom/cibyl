######################################################################
##
## Copyright (C) 2007,  Blekinge Institute of Technology
##
## Filename:      parse.py
## Author:        Simon Kagstrom <ska@bth.se>
## Description:   Parsing-related stuff for the peephole optimizer
##
## $Id: parse.py 14173 2007-03-11 14:52:39Z ska $
##
######################################################################
from classes import *
import template
import re, sys
import emit

from templates import goto_next_line, const_and_inc, double_stores, double_pop, getstatic_v1_pop, putstatic_getstatic

FUNCTION_NAME="([A-Z,a-z,0-9,_,\(,\)]+)"

function_name = re.compile(FUNCTION_NAME)
method_declaration = re.compile(".method [public|private]+ static " + FUNCTION_NAME)
method_end = re.compile(".end method")
label_definition = re.compile("[ ,\t]*([A-Z,a-z,0-9,_]+):")
comment = re.compile("[ ,\t]*;([A-Z,a-z,0-9,_,\(,\), ]+):")

ins_goto = re.compile("[ ,\t]*goto[ ,\t]([A-Z,a-z,0-9,_]+)")
ins_iload = re.compile("[ ,\t]*iload[_, ]([0-9]+)")   # iload_0, iload 15
ins_istore = re.compile("[ ,\t]*istore[_, ]([0-9]+)") # istore_2, istore 15
ins_getstatic_reg = re.compile("[ ,\t]*getstatic (CRunTime/[hl][io]) I")
ins_getstatic_reg_v1 = re.compile("[ ,\t]*getstatic (CRunTime/saved_v1) I")
ins_putstatic_reg = re.compile("[ ,\t]*putstatic (CRunTime/[hl][io]) I")
ins_putstatic_reg_v1 = re.compile("[ ,\t]*putstatic (CRunTime/saved_v1) I")
ins_iinc = re.compile("[ ,\t]*iinc ([0-9]+) ([-]*[0-9]+)")    # iinc reg val
ins_ipush = re.compile("[ ,\t]*[bs]ipush ([-]*[0-9]+)")
ins_ldc_int = re.compile("[ ,\t]*ldc ([-]*[0-9]+)")
ins_iconst = re.compile("[ ,\t]*iconst_([0-5]+)")
ins_iconst_m1 = re.compile("[ ,\t]*iconst_m1")
ins_cond_branch_two = re.compile("[ ,\t]*if_icmp[ne][qe][ ,\t]([A-Z,a-z,0-9,_]+)")
ins_cond_branch_one = re.compile("[ ,\t]*if[gl][et][ ,\t]([A-Z,a-z,0-9,_]+)")
ins_invokestatic = re.compile("[ ,\t]*invokestatic[ ,\t]([A-Z,a-z,0-9,_]+)")

instruction = re.compile("\t([A-Z,a-z,0-9,_,\(,\), ]+)")

def matchComment(line):
    if comment.match(line):
	return Comment(line)
    return None

def matchGoto(line):
    if ins_goto.match(line):
	match = ins_goto.match(line)
	return Goto(line, match.group(1))
    return None

def matchInvokestatic(line):
    if ins_invokestatic.match(line):
	match = ins_invokestatic.match(line)
	return Invokestatic(line, match.group(1))
    return None

def matchCondBranch(line):
    if ins_cond_branch_one.match(line):
	match = ins_cond_branch_one.match(line)
	return ConditionalBranchOne(line, match.group(1))
    if ins_cond_branch_two.match(line):
	match = ins_cond_branch_two.match(line)
	return ConditionalBranchTwo(line, match.group(1))
    return None

def matchIinc(line):
    if ins_iinc.match(line):
	match = ins_iinc.match(line)
	return Iinc(line, match.group(1), match.group(2))
    return None

def matchIstore(line):
    if ins_istore.match(line):
	match = ins_istore.match(line)
	return Istore(line, match.group(1))
    return None

def matchIload(line):
    if ins_iload.match(line):
	match = ins_iload.match(line)
	return Iload(line, match.group(1))
    return None

def matchGetstatic(line):
    if ins_getstatic_reg.match(line):
	match = ins_getstatic_reg.match(line)
	return Getstatic(line, match.group(1))
    if ins_getstatic_reg_v1.match(line):
	match = ins_getstatic_reg_v1.match(line)
	return Getstatic(line, match.group(1))
    return None

def matchPutstatic(line):
    if ins_putstatic_reg.match(line):
	match = ins_putstatic_reg.match(line)
	return Putstatic(line, match.group(1))
    if ins_putstatic_reg_v1.match(line):
	match = ins_putstatic_reg_v1.match(line)
	return Putstatic(line, match.group(1))
    return None

def matchConst(line):
    for regexp in (ins_ipush, ins_ldc_int, ins_iconst):
	if regexp.match(line):
	    match = regexp.match(line)
	    return Const(match.group(1), line)
    if ins_iconst_m1.match(line):
	return Const(-1, line)
    return None

def matchLabel(line):
    if label_definition.match(line):
	match = label_definition.match(line)
	return Label(line, match.group(1))
    return None

def matchInstruction(line):
    for fn in (matchGoto, matchConst, matchIinc, matchIstore, matchIload, matchCondBranch, matchInvokestatic, matchGetstatic, matchPutstatic):
	m = fn(line)
	if m:
	    return m
    match = instruction.match(line)
    if match:
	# Check for arithmetic instructions
	if match.group(1) in ("imul", "idiv", "iadd", "isub", "ishl", "ishr", "iushr", "ixor", "iand", "ior", "irem", "ineg", "fmul", "fdiv", "fadd", "fsub", "fneg"):
	    return ArithmeticInstruction(line)
	if match.group(1) == "ireturn":
	    return Ireturn(line)
	return Instruction(line)
    return False


def readFile(filename):
    f = open(filename)
    lines = f.readlines()
    f.close()

    all = []

    curfn = []
    cur = None
    fn = False
    fnLine = ""

    for line in lines:
	com = matchComment(line)
	lab = matchLabel(line)
	ins = matchInstruction(line)

	if com:
	    cur = com
	elif lab:
	    cur = lab
	elif ins:
	    cur = ins
	elif method_declaration.match(line):
	    match = method_declaration.match(line)
	    fn = match.group(1)
	    fnLine = line
	    curfn = []
	    all.append(Base("\n"))
	    continue
	elif method_end.match(line):
	    cur = Function(fnLine, fn, curfn)
	    fn = False
	else:
	    cur = Base(line)

	# Append cur to the current function
	if fn:
	    curfn.append(cur)
	else:
	    # Append to everything
	    all.append(cur)

    return all

def optimize(num, all):
    for n in range(0, num):
	for item in all:
	    if isinstance(item, Function):
		for t in template.getTemplates():
		    t.apply(item)

    for item in all:
	item.emit()

def run(num, infile, outfile):
    all = readFile(infile)
    emit.out = open(outfile, "w")
    optimize(num, all)
    emit.out.close()

if __name__ == "__main__":
    run(sys.argv[1], "a.j")
