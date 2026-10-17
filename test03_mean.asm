; test03_mean.asm
;finding a Mean
;sum of arr of 4 elements/4
;div
;Final State: A = 25 (0x19)

        ldc 0
        stl 0       ; sum=0
        ldc 4
        stl 1       ;count=4
        ldc vals
        stl 2       ;pointer=values array

loop:   ldl 1
        brz calc    ; If count==0
        ldl 2
        ldnl 0
        ldl 0
        add
        stl 0       ;sum=sum+*pointer
        ldl 2
        adc 1
        stl 2       ;pointer++
        ldl 1
        ldc 1
        sub
        stl 1       ;count--
        br loop

calc:   ldl 0       ;load total sum
        ldc 4       ;load n
        div         ;
        HALT

vals:   data 10
        data 20
        data 30
        data 40