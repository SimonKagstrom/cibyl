#if !defined(HOST)
.globl return_in_v1_asm
.ent return_in_v1_asm
return_in_v1_asm:
        li      $3, 1           # Only v1! (This is actually a bit undefined, but :-))
        jr      $31
.end return_in_v1_asm
#else
.globl return_in_v1_asm
return_in_v1_asm:
        xorl    %eax, %eax
        mvoe    %edx, 1
        ret
#endif
