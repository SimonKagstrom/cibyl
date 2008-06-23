#!/bin/sh

lcov --directory .. --base-directory .. --zerocounters
rm -rf html/* out/* lcov.info

# Run with different options
../xcibyl-translator config:maboo out $CIBYL_BASE/tests/c/program $CIBYL_BASE/include/generated/cibyl-syscalls.db $CIBYL_BASE/tests/include/cibyl-syscalls.db
../xcibyl-translator config:maboo=1 out $CIBYL_BASE/tests/c/program $CIBYL_BASE/include/generated/cibyl-syscalls.db $CIBYL_BASE/tests/include/cibyl-syscalls.db
../xcibyl-translator config: out $CIBYL_BASE/tests/c/program $CIBYL_BASE/include/generated/cibyl-syscalls.db $CIBYL_BASE/tests/include/cibyl-syscalls.db
../xcibyl-translator config:thread_safe=1 out $CIBYL_BASE/tests/c/program $CIBYL_BASE/include/generated/cibyl-syscalls.db $CIBYL_BASE/tests/include/cibyl-syscalls.db
../xcibyl-translator config:prune_call_table=1,call_table_hierarchy=2,class_size_limit=99999999,prune_unused_functions=1,trace_start=0x01000000,trace_end=0x2000000,trace_stores=1 out $CIBYL_BASE/tests/c/program $CIBYL_BASE/include/generated/cibyl-syscalls.db $CIBYL_BASE/tests/include/cibyl-syscalls.db
../xcibyl-translator config:optimize_partial_memory_operations=1,class_size_limit=99999999 -DMABOO=1  out $CIBYL_BASE/tests/c/program $CIBYL_BASE/include/generated/cibyl-syscalls.db $CIBYL_BASE/tests/include/cibyl-syscalls.db
../xcibyl-translator "config:colocate_functions=main;test_run_all_tests;vsnprintf,optimize_partial_memory_operations=1,class_size_limit=99999999" -DMABOO=1  out $CIBYL_BASE/tests/c/program $CIBYL_BASE/include/generated/cibyl-syscalls.db $CIBYL_BASE/tests/include/cibyl-syscalls.db

lcov --directory .. --base-directory .. --capture --output-file lcov.info
genhtml --output-directory html/ lcov.info
