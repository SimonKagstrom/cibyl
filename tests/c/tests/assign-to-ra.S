.globl  assign_to_ra
.ent    assign_to_ra
assign_to_ra:
        sub     $29, $29, 4
        sw      $31, 0($29)
        bne     $29, $0, 1f
        li      $31, 0xdeadbeef
        lw      $31, 0($29)
1:
        add     $8, $31, $31
        jr      $31
.end    assign_to_ra
