# Project Description

Hi there, my name is **Junzhe Wang**.
This project is about a assembler I wrote for the CPU I've designed. 

[CPU](https://github.com/J-M-W0/CPU)

I'll simply name this project RASM (runty assembler), because this assembler is so small and tiny haha.

To run the program, after compilation, simply type on your prompt:

> rasm input [-o output]

Example:
> rasm test.rasm
- this will generate a binary file *a.out* and text file *a.txt* which can be used in Logisim.

> rasm test.rasm -o test
- this will generate a binary file *test* and text file *a.txt* which can be used in Logisim.

Things to be noticed:
1. the input file name came as the first parameter , and must be of ending ***.rasm***.
2. the output name will be identical to what you've typed, 
3. the output file is in binary format.
4. if no output name given, it will be the defult name  *a.out*.
5. it will also generate a text file named *a.txt* which could be used for **Logisim** to test the CPU I've designed.

# Example Usage
1. navigate to the directory of the project, undeer the same directory of the *Makefile*.
2. type on your prompt *make*.
> $ make
3. then compile a example rasm file:
> $ rasm ./example/fibonacci.rasm
4. then open logisim with the *cpu.circ* on [CPU](https://github.com/J-M-W0/CPU), load the a.txt file into the RAM, can run it.

Or see the YouTube link video where I demonstrated it: 
<iframe width="560" height="315" src="https://www.youtube.com/embed/https://youtu.be/sKxlbndWQQM" frameborder="0" allowfullscreen></iframe>

# File Strucure
1. ***src/***
- it contains the source files.
    - main.c
        - it contains the entry point for the program.
    - rasm.c
        - it contains the general debugging functions.
        - e.g. @func *{debug}*, @func *{info}*, @func *{errorf}*
    - str.c
        - it contains some string functions I defined to use for myself.
        - for example 
            - @func *{strpre}* which prepends the string with a header.
            - @func *{strapp}* which appends the string with an extension.
            - @func *{strloc}* which locates a substring inside a string.
            - @func *{strtoi}* which converts a string into an integer.
            - @func *{itostr}* which converts an integer into string.
            - @func *{strrev}* which reverses a string.
    - table.c
        - it contains the functions for manipulating the label table.
    - utils.c
        - it contains the utility functions used for the assembler.
    - operand.c
        - it contains the functions to parse the operands.
    - instru.c
        - it contains the functions to parse the instruction.
    - assemble.c
        - it contains the functions to assemble.
    - codegen.c
        - it contains the functions to generate code.
    - resources.c
        - it contains the resources.
2. ***inc/***
- it contains the header files.
    - rasm.h
    - str.h
    - table.h
    - utils.h
    - operand.h
    - instru.h
    - assemble.h
    - codegen.h
3. ***lib/***
- it contains the compiled object files to be linked.
4. ***examples/***
- it contains example assembly code.
    - test.rasm
        - a testing file.
    - fibonacci.rasm
        - to calculate a RASM inside my customied CPU.
        - it will return the result in register *r0*.
        - parameter is stored on stack.

