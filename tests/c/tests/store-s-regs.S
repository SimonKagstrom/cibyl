#define sp $29
#define ra $31
#define a0 $4
#define a1 $5
#define a2 $6
#define a3 $7
#define zero $0
#define s0 $16
#define s0 $16
#define s1 $17
#define s2 $18

.globl  store_s_regs
.ent    store_s_regs
store_s_regs:
        addiu   sp,sp,-40
        sw      s2,32(sp)
        sw      s1,28(sp)
        sw      s0,24(sp)
        lui     s1,0x1
        lui     s2,0x0
        lui     s0,0x0
        lui     a1,0x0
        addiu   s0,s0,16444
        addiu   a0,s1,-31868
        addiu   a3,s2,16364
        addiu   a1,a1,2048
        move    a2,zero
        sw      ra,36(sp)
        sw      s0,16(sp)
        jal     store_s_regs_c_helper
        lw      ra,36(sp)
        lw      s2,32(sp)
        lw      s1,28(sp)
        lw      s0,24(sp)
        jr      ra
        addiu   sp,sp,40
.end    store_s_regs
