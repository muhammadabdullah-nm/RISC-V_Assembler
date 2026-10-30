# RISC-V Assembler
 
A simple command-line **assembler for the RISC-V (RV32I) instruction set**, written in C. It reads an assembly source file and writes the 32-bit machine code for each instruction, one per line, as either binary or hexadecimal.
 
## Features
 
- Translates RV32I assembly into 32-bit machine code
- Output as **binary** (`-b`) or **hexadecimal** (`-h`)
- Supports R, I, S, B, U, and J instruction formats
- Label support for branches and jumps
- Accepts both register numbers (`x0`–`x31`) and ABI names (`zero`, `ra`, `sp`, `a0`, `t0`, `s0`, ...)
## Supported Instructions
 
| Type | Instructions |
|------|--------------|
| R    | `add` `sub` `sll` `slt` `sltu` `xor` `srl` `sra` `or` `and` |
| I    | `addi` `slti` `sltiu` `xori` `ori` `andi` `slli` `srli` `srai` `lb` `lh` `lw` `lbu` `lhu` `jalr` |
| S    | `sb` `sh` `sw` |
| B    | `beq` and the other branch instructions |
| U    | `lui` `auipc` |
| J    | `jal` |
