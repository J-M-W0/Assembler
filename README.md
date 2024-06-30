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


# File Strucure
1. ***src/***
- it contains the source files.
>- main.c
>-- it contains the entry point for the program.
>- rasm.c
>-- it contains the general debugging functions.
>-- e.g. @func {debug}, @func {info}, @func {errorf}
>- str.c
>- table.c
>- utils.c
>- operand.c
>- instru.c
>- assemble.c
>- codegen.c
>- resources.c
2. ***inc/***
- it contains the header files.
3. ***lib/***
- it contains the compiled object files to be linked.
4. ***examples/***
- it contains example assembly code.

