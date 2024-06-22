section s1 0x00
    jmp main
    add r0, r1
    add r0, r1
    add r0, r1

section s2 0x01
    add r0, r1
    add r0, 0x11
main:
    add r0, 0x11

section s4 0xb1
.exit:
    add r0, wrt main.exit
    add r0, 0x11

    jmp .exit



