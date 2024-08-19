MAIN:   add r3, LIST
LOOP:   prn #48
        macr m_macr 
        cmp r3, #-6
        bne END


        endmacr 
        lea STR, 16

;sdasdasdadasdaasdas
        inc r6
        mov *r6,K

        sub rl, r4
        m_macr
;sdasdasdadasdaasdas
        dec K
        jmp LOOP

END:    stop
STR:    .string "abcd"
LIST:   .data 6, -9
        .data -100

K:      .data 31
;sdasdasdadasdaasdas
