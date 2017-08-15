Instruction set supported (target TI MSP430G2553)
ADD, AND, BIC, BIS, BIT, CALL, CMP, DEC, INC, JZ, JNZ, MOV, RET, PUSH, POP, and SWPB instructions
Addressing modes supported (byte and word)
Register mode: Rn
Indexed mode: X(Rn)
Absolute mode: &ADDR
Indirect register mode: @Rn 
Immediate: #N
ALL labels must start in leftmost column, no label can exceed 24 characters in length,
all labels must start with Alphabetic character, and all labels are case sensitive. Labels can include any
alphabetic character, and number 0 thru 9 (except as first character), and the underline symbol, e.g. LoopTop_1
NO opcodes can start in leftmost column (opcodes will be lower case), e.g. mov
Constants defined by EQU can use same format as labels, but will be uppercase, e.g. DELAY_1
Variables can be a combination of upper and lower case, e.g. LoopCount
Lines longer than 80 characters can be truncated
END statement is required
Assembler Directives supported: (these will always be upper case format)
ORG, DB, DW, DS, EQU, END
Assembler will produce a LIST file with line number for each source code line, address assigned for a
line, constant values assigned to any constant, total number of bytes in assembled program.
Source files up to 200 lines can be processed.
Meaningful error messages provided for source file errors, including line number where error occurred
Output filenames: LIST: FileName.lst, OBJECT: FileName.txt
Unique sequential line number (decimal) for each line of source file, address for that line (hexadecimal –
ASCII format), encoding of that line as hexadecimal (in ASCII format) in byte order as used in MSP430
memory, original source line, sorted symbol table, total bytes used in MSP430 memory 
