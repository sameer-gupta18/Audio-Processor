movz x0, #0x40
movz x1, #0x3f20, lsl #16
str w0, [x1]
movz x2, #0x1c
movz x3, #0x4
movz x5, #0x0028
loop:
    str w3, [x1,x2]
    movz x4, #0xFF, lsl #16
    delay: 
        subs w4, w4, #1
        b.ne delay
    str w3, [x1,x5]
    movz x4, #0xFF, lsl #16
    delay2: 
        subs w4, w4, #1
        b.ne delay2
    b loop

