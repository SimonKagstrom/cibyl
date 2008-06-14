### This test is mostly for the compiler (translate tail calls to normal dittos)
### It should just return 1 instead of 0
#if !defined(HOST)

.ent maboo
maboo:
        li      $2, 1
        jr      $31
.end maboo

.globl tail_call_run
.ent tail_call_run
tail_call_run:
        li      $2, 0
        bne     $2, $0, 1f
1:      j       maboo
.end tail_call_run
#else
.globl tail_call_run
tail_call_run:
        ret
#endif
