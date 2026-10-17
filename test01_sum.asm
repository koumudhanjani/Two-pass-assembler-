; test01_sum.asm
;adding n numbers
;sum of an array of 5 integers.
;Final State:A = 150 (0x96)

        ldc 0       ;sum=0
        stl 0       ;sum is stored at memory sp+0
        ldc 5       ;loop counter=5 (number of elements)
        stl 1       ;counter stored at memory sp+1
        ldc arr     ;load addr of arr
        stl 2       ;store arr pointer at sp+2

loop:   ldl 1       ;load counter to A
        brz done    ;if counter is 0

        ldl 2       ;load arr pointer into A
        ldnl 0      ;load arr value into A
        ldl 0       ;load curr sum into A
        add         ; A=B+A(arr value+curr sum)
        stl 0       ;store new sum to sp+0

        ldl 2       ;load arr pointer
        adc 1       ;to move to next addr
        stl 2       ;store new pointer

        ldl 1       ;counter
        ldc 1       
        sub         ;A=B-A(counter-1)
        stl 1       ;updated counter

        br loop     

done:   ldl 0       ;load final sum into A
        HALT        

arr:    data 10     ; Array element 1
        data 20     ; Array element 2
        data 30     ; Array element 3
        data 40     ; Array element 4
        data 50     ; Array element 5