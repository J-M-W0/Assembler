# RASM - Simple Assembler

__RASM__ is a simple assembler designed for educational purposes. 
It translates assembly language code into machine code. 
This project demonstrates basic assembler functionalities including lexical analysis, parsing, and code generation.

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [File Structure](#file-structure)
- [Installation](#installation)
- [Usage](#usage)
- [Example](#example)
- [Demonstration Video](#demostration-video)

## Introduction

1. __RASM__ is a straightforward assembler implemented to help understand the fundamentals of assembly language processing  and code generation. 
It is written in C language using the C17 standard.

2. It generates a custome executable binary code which is compatible with my custome designed [CPU](https://github.com/J-M-W0/CPU).

3. The name __RASM__ came from Runty ASseMbler, because this assembler is so small and tiny haha.

## Features

- **Lexical Analysis**: Tokenizes the assembly language code.
- **Parsing**: Constructs the syntax tree from tokens.
- **Code Generation**: Produces machine code from the syntax tree.
- **Error Handling**: Reports lexical and syntactic errors.

## File Structure

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

## Installation

To install and run RASM, you need to have at least one C compiler installed on your system. 
You can download and install GCC from [the official website](https://gcc.gnu.org/install/download.html).

1. Clone the repository:
    ```sh
    git clone <repo>
    ```
2. Navigate to the project directory:
    ```sh
    cd <install-directory>
    ```
3. Compile the assembler:
    ```sh
    make
    ```

## Usage

After compiling, you can run RASM from the command line. 
The assembler takes an assembly source file as input and generates a machine code file.

    ```sh
    ./rasm <sourcefile> [-o <outputfile>]
    ```

Please notice that the __sourcefile__ must be provided and have to be ended with __.rasm__.
And the __outputfile__ is optional, if none is provided, it will generate a __a.out__ as binary file.
And after all, it will generate a __a.txt__ file which can be used with the [CPU](https://github.com/J-M-W0/CPU).

## Example

1. Create a source file __fibonacci.rasm__ with the following code:
```asm
        jmp main

    ; fib(0) = 0
    ; fib(1) = 1
    ; fib(2) = 1
    ; fib(3) = 2
    ; fib(4) = 3
    ; fib(5) = 5
    ; fib(6) = 8 == 0b 1000

    section code 0x7
    main:
        ; @param push <N>
        ; fib (N)
        push 6
        call fib    ; the return value is in r0
        pop0        ; to clear the stack used for ```push 2```
        END

    section functions 0xa
    fib:
        push bp
        mov bp, sp
        sub bp, 3

        mov r0, word [bp]
        cmp r0, 1
        ja .recur
        
        pop bp
        ret
        
    .recur:
        dec r0
        mov word [bp], r0
        push r0
        call fib
        pop0
        mov r1, r0
        push r1

        mov r0, word [bp]
        dec r0
        push r0
        call fib
        pop0
        
        pop r1
        add r0, r1

        pop bp
        ret
```

2. Run the assembler:
```sh
    ./rasm fibonacci.rasm -o fibo
```

3. The generated machine code and executable binary text will be in the same directory.

## Demonstration Video

You can also see the YouTube link video where I demonstrated it: [demo](https://youtu.be/sKxlbndWQQM)

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.


