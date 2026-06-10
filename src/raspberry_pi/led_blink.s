movz w0 #0x40
movz w1 #0x3f200000
str w0 [w1]
movz w2 #0x1c
movz w3 #0x4
movz w5 #0x0028
loop:
    str w3 [w1,w2]
    movz w4 #0x00FFF
    delay: 
        subs w4, w4, #1
        b.ne delay
    str w3 [w1,w5]
    movz w4 #0x00FFF
    delay2: 
        subs w4, w4, #1
        b.ne delay2
    b loop

