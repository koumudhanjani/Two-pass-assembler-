; test06_variance.asm
;variance (Standard Deviation without Square Root)
;array {2, 4, 6, 8}.mean is 5, n is 4.
;Final State: A = 5 (0x05)

        ldc 0       ;sum of squares=0
        stl 0       ;sp+0=sum of squares
        
        ldc 4       ;n=4
        stl 1       ;sp+1=counter

        ldc arr     ;
        stl 2       ;sp+2=array pointer

loop:   ldl 1
        brz calc    ;if count=0

        ;calculate (value - mean)
        ldl 2       ;curr arr pointer
        ldnl 0      ;actual value
        ldc 5       ;load mean
        sub         ;
        
        ;square the difference using 'mul'
        stl 3       ;sp+3=value-mean
        ldl 3       ;
        ldl 3       ;
        mul         ;(value-mean)^2
        
        ;add to curr sum
        ldl 0       ;curr sum
        add         ;diff^2+sum
        stl 0       ;store sum at SP+0

        ;advance pointer and decrement counter
        ldl 2       
        adc 1       ;pointer++
        stl 2       

        ldl 1       
        ldc 1       
        sub         ;count--
        stl 1       

        br loop     ;

calc:   ldl 0       ;load total sum of squares
        ldc 4       ;load n
        div         ;
        HALT        ;

arr:    data 2
        data 4
        data 6
        data 8