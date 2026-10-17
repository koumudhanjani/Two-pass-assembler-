;test02_poly.asm
;computing a polynomial
;y=3x^2+2x+5,where x=4.
;Final State: A = 61 (0x3D)

        ;calculate 3x^2
        ldc 4       ;load x
        ldc 4       ;load x
        mul         ;x^2
        ldc 3       ;load coefficient
        mul         ;
        stl 0       ;store into local memory sp+0

        ;calculate 2x and add to 3x^2
        ldc 4       ;load x
        ldc 2       ;load coefficient
        mul         ;
        ldl 0       ;load 3x^2 result
        add         ;
        
        ;add constant 5
        ldc 5       ;
        add         ;

        HALT        ;