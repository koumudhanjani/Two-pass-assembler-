; test07_factorial.asm
;factorial calculation(5!)
;mul
;Final State: A = 120 (0x78)

        ldc 1       ;product=1
        stl 0       ;sp+0=product

        ldc 5       ;loop counter=5
        stl 1       ;sp+1 = count

loop:   ldl 1       ;load counter
        brz done    ;

        ldl 0       ;curr product
        mul         ;A=counter*curr product
        stl 0       ;store new product at sp+0

        ldl 1       ;count
        ldc 1       ;
        sub         ;count--
        stl 1       ;sp+1=count

        br loop     ;

done:   ldl 0       ;A=final product
        HALT        ;