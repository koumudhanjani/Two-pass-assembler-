; test05_median.asm
;finding the median of a sorted array
;array size N=5.index of median N/2 = 2.
;div
;Final State: A = 35 (0x23)

        ldc 5       ;n=5
        ldc 2       ;load 2
        div         ;n/2
        stl 0       ;store median index at sp+0

        ldc arr     ;addr of arr
        ldl 0       ;load index
        add         ;arr(base)+index
        ldnl 0      ;take value
        HALT        ;

arr:    data 15     ; index 0
        data 25     ; index 1
        data 35     ; index 2(median)
        data 45     ; index 3
        data 55     ; index 4