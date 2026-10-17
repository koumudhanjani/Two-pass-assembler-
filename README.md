# Two-Pass Assembler & Emulator for the IITPSx Processor (CS2102 MiniProject)

A two-pass assembler (`asm.c`) and emulator (`emu.c`) written in C for the **IITPSx Processor** architecture, developed as part of the **CS2102 MiniProject**.

---

## Overview

The **IITPSx** processor is a 32-bit stack-organized architecture with four internal 32-bit registers:
- **`A` & `B`**: Arranged as a two-register internal evaluation stack (`A` acts as the top-of-stack / accumulator, and `B` holds the second element).
- **`PC`**: Program Counter (implicitly incremented by `1` *before* the execution of each instruction).
- **`SP`**: Stack Pointer pointing to local memory frames.

### Instruction Encoding
Every instruction is **32 bits** wide and takes either no operand or a single signed 24-bit 2's complement operand:
```text
+-----------------------------------+-------------------+
|        Signed Operand (24 bits)   |  Opcode (8 bits)  |
+-----------------------------------+-------------------+
31                                 8 7                 0
```

- **Branch Instructions (`call`, `brz`, `brlz`, `br`)**: Use PC-relative displacement (`offset = target_address - (PC + 1)`).
- **Non-Branch Instructions (`ldc`, `adc`, `ldl`, `stl`, `ldnl`, `stnl`, `adj`)**: Use direct values or label addresses.
- **Number Formats Supported**: Decimal (`10`, `-5`), Hexadecimal (`0x5ab4`), and Octal (`075`).
- **Comments**: Any text following a semicolon (`;`) on a line is ignored.

---

## IITPSx Instruction Set

The table below lists the complete IITPSx instruction set (including the extended arithmetic instructions `mul` and `div`, as well as the `data` and `SET` directives):

| Mnemonic | Opcode | Operand | Formal Specification | Description |
| :--- | :---: | :---: | :--- | :--- |
| `data` | *(N/A)* | `value` | *(Directive)* | Reserve a memory location initialized to the specified 32-bit `value` |
| `ldc` | `0` | `value` | `B := A; A := value;` | Load accumulator `A` with `value` (pushes old `A` to `B`) |
| `adc` | `1` | `value` | `A := A + value;` | Add the specified `value` to accumulator `A` |
| `ldl` | `2` | `offset` | `B := A; A := memory[SP + offset];` | Load local variable at `SP + offset` into `A` |
| `stl` | `3` | `offset` | `memory[SP + offset] := A; A := B;` | Store `A` into local memory at `SP + offset` and pop `B` to `A` |
| `ldnl` | `4` | `offset` | `A := memory[A + offset];` | Load non-local value from `memory[A + offset]` |
| `stnl` | `5` | `offset` | `memory[A + offset] := B;` | Store non-local value `B` into `memory[A + offset]` |
| `add` | `6` | *(none)* | `A := B + A;` | Addition |
| `sub` | `7` | *(none)* | `A := B - A;` | Subtraction |
| `shl` | `8` | *(none)* | `A := B << A;` | Bitwise shift left |
| `shr` | `9` | *(none)* | `A := B >> A;` | Bitwise shift right |
| `adj` | `10` | `value` | `SP := SP + value;` | Adjust stack pointer `SP` by `value` |
| `a2sp` | `11` | *(none)* | `SP := A; A := B;` | Transfer `A` to `SP` |
| `sp2a` | `12` | *(none)* | `B := A; A := SP;` | Transfer `SP` to `A` |
| `call` | `13` | `offset` | `B := A; A := PC; PC := PC + offset;` | Call procedure at relative `offset` |
| `return` | `14` | *(none)* | `PC := A; A := B;` | Return from procedure |
| `brz` | `15` | `offset` | `if A == 0 then PC := PC + offset;` | Branch to relative `offset` if `A == 0` |
| `brlz` | `16` | `offset` | `if A < 0 then PC := PC + offset;` | Branch to relative `offset` if `A < 0` |
| `br` | `17` | `offset` | `PC := PC + offset;` | Unconditional branch to relative `offset` |
| `HALT` | `18` | *(none)* | *(Stop Emulator)* | Stop emulator execution and print final register state |
| `mul` | `19` | *(none)* | `A := B * A;` | Multiplication (`B * A`) |
| `div` | `20` | *(none)* | `if A == 0 then error; else A := B / A;` | Signed integer division (`B / A`), with division-by-zero detection |
| `SET` | *(N/A)* | `value` | *(Pseudo-instruction)* | Set the label on the current line to `value` instead of the current `PC` |

---

## Project Architecture & Features

### 1. Two-Pass Assembler (`asm.c`)
- **Pass 1 (Symbol Discovery & Address Assignment)**:
  - Strips comments (`;`) and parses label definitions (`label:`).
  - Validates label names (must begin with a letter followed by alphanumeric characters or underscores).
  - Detects duplicate labels and invalid label identifiers.
  - Handles the `SET` pseudo-instruction to bind labels to explicit constant values rather than the current `PC`.
  - Tracks program counter (`PC`) mappings for each source line.
- **Pass 2 (Code Generation & Diagnostics)**:
  - Translates mnemonics via the instruction table (`optab`) and resolves label references and numeric literals (decimal, octal, hex).
  - Computes PC-relative offsets (`target - (PC + 1)`) for branch/call instructions (`opcodes 13..17`).
  - Checks for 24-bit signed operand overflow (`[-8388608, 8388607]`).
  - Diagnoses errors: unknown mnemonics, undefined labels, missing operands, unexpected operands, invalid `data`/`SET` operands.
  - Issues warnings: unused labels and backward self-jumps (`offset == -1`, potential infinite loop).
- **Generated Outputs**:
  - `<name>.o`: Binary machine-code object file.
  - `<name>.lst`: Human-readable listing file displaying address (`%04x`), 32-bit hex machine code (`%08X`), source statement, and inline warnings/errors.
  - `<name>.log`: Detailed assembly log summarizing errors, warnings, and the symbol table.

### 2. IITPSx Emulator (`emu.c`)
- Loads binary `.o` object files into a 10,000-word simulated memory array.
- Outputs a formatted hexadecimal **Memory Dump** prior to execution.
- Decodes the 8-bit opcode and sign-extends the 24-bit operand (`if (arg & 0x800000) arg |= 0xFF000000`).
- Executes all standard and extended (`mul`, `div`) instructions.
- Performs runtime safety checks:
  - Program Counter out-of-bounds detection.
  - Memory access bounds checking (`SP + offset` and `A + offset`).
  - Division-by-zero trapping on `div` (`opcode 20`).
  - Unknown opcode detection.

---

## Repository Structure

```text
├── asm.c                  # Two-pass assembler source code
├── emu.c                  # IITPSx processor emulator source code
├── claims.txt             # Submission claims against the CS2102 marking scheme
├── Makefile               # Build configuration
├── bubble_sort.asm        # Bubble sort implementation in IITPSx assembly
├── test0.asm .. test7.asm # Core assembler & instruction set test cases
├── test01_sum.asm         # Sum of N numbers algorithm
├── test02_poly.asm        # Polynomial evaluation (using mul)
├── test03_mean.asm        # Arithmetic mean calculation (using div)
├── test04_err.asm         # Error diagnosis test suite
├── test05_median.asm      # Median calculation test case
├── test06_variance.asm    # Variance calculation (using mul and div)
├── test07_factorial.asm   # Factorial calculation
└── *.lst, *.log, *.o      # Assembled listing, diagnostic log, and binary object files
```

---

## Building and Running

### Compile Assembler & Emulator
```bash
gcc asm.c -o asm
gcc emu.c -o emu
```

### Assemble a Program
```bash
./asm bubble_sort.asm
```
This generates:
- `bubble_sort.o` (binary object file)
- `bubble_sort.lst` (listing file)
- `bubble_sort.log` (diagnostics & symbol table log)

### Run on the Emulator
```bash
./emu bubble_sort.o
```

#### Example Output (`test4.asm` — `mul` & `div` test)
```text
=== Memory Dump ===
0000  00000600
0001  00000700
0002  00000013
0003  00000200
0004  00000014
0005  00000012
...
===================

Executing program...
Program execution halted successfully.
Final State: A = 21, B = 2, PC = 6, SP = 0
```
