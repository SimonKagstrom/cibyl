.text
.globl  sw_in_delay_slot
.ent    sw_in_delay_slot
sw_in_delay_slot:
        sub     $29, $29, 4

.set    noreorder
        bne     $4, $5, 1f
        sw      $0, 0($29)       # Delay slot. There was a problem with pass1 here before
.set    reorder
1:
        add     $29, $29, 4
        jr      $31
.end    sw_in_delay_slot
