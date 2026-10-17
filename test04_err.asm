; test04_err.asm
;assembler errors and emulator div-by-zero
;
; the assembler generates a .log file full of warnings.
;

dup:    ldc 1       ;valid
dup:    ldc 2       ;ERR: duplicate label
        
        add 10      ;ERR: unexpected operand
        ldc         ;ERR: missing operand
        
        fakeop 5    ;ERR: unknown mnemonic

        ;emulator div by zero
        ldc 50      ;
        ldc 0       ;
        div         ; Should trigger emulator RUNTIME ERROR

        HALT