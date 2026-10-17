; bubble_sort.asm
; IITPSx Final Bubble Sort Program

        ldc 500
        a2sp        ; Set stack pointer safely away from code

        ldc 4
        stl 0       ; SP+0 = i (outer loop counter = 4)

outer:  
        ldl 0
        brz done    ; If i == 0, jump to HALT

        ldc 0
        stl 1       ; SP+1 = j (inner loop counter = 0)

inner:  
        ; Check if inner loop reached outer limit (i - j == 0)
        ldl 0
        ldl 1
        sub
        brz next_out

        ; Load arr[j]
        ldc arr
        ldl 1
        add
        ldnl 0
        stl 2       ; SP+2 = arr[j]

        ; Load arr[j+1]
        ldc arr
        ldl 1
        add
        adc 1
        ldnl 0
        stl 3       ; SP+3 = arr[j+1]

        ; Compare arr[j] and arr[j+1] (arr[j] - arr[j+1])
        ldl 2
        ldl 3
        sub
        brlz no_swap
        brz no_swap

                ; --- SWAP ---
        ; STNL requires:
        ;   A = destination address
        ;   B = value to store
        ;
        ; Use SP+4 as a temporary location for the destination address.

        ldc arr
        ldl 1
        add
        stl 4       ; SP+4 = address of arr[j]

        ldl 3       ; A = arr[j+1]
        sp2a        ; A = SP, B = arr[j+1]
        adc 4       ; A = SP+4
        ldnl 0      ; A = address of arr[j], B = arr[j+1]
        stnl 0      ; arr[j] = arr[j+1]

        ldc arr
        ldl 1
        add
        adc 1
        stl 4       ; SP+4 = address of arr[j+1]

        ldl 2       ; A = arr[j]
        sp2a        ; A = SP, B = arr[j]
        adc 4       ; A = SP+4
        ldnl 0      ; A = address of arr[j+1], B = arr[j]
        stnl 0      ; arr[j+1] = arr[j]

no_swap:
        ; j++
        ldl 1
        adc 1
        stl 1
        br inner

next_out:
        ; i--
        ldl 0
        ldc 1
        sub
        stl 0
        br outer

done:   
        HALT        ; Stops execution cleanly before data section

arr:    data 42
        data 23
        data 99
        data 5
        data 12