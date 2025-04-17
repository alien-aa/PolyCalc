# PolyLang: Polynomial Expression Interpreter

## Description

PolyCalc is a simple interpreter for a formal language designed to perform operations on polynomial expressions. It allows users to define polynomials, perform arithmetic operations such as addition, subtraction, multiplication, division (by a constant), and exponentiation, as well as evaluate polynomials for given variable values.

This project was created using `flex` and `bison` for lexical and syntactical analysis, and `C` for the implementation of the interpreter logic.

## Repository Structure

├── README.md # This file

├── Makefile # Build instructions

├── lexer.l # flex definition file for lexical analysis

├── parser.y # bison definition file for syntactical analysis

├── polynomial.h # Header file for polynomial data structures and function declarations

├── polynomial.c # Source file for polynomial operations and interpreter logic

├── correct.poly # Example test file with valid polynomial expressions

├── test.poly # Another example test file

└── report.pdf # Report documenting the project design and implementation


*   **`README.md`**: This file, providing an overview of the project.
*   **`Makefile`**: Contains instructions for compiling the project.
*   **`lexer.l`**: flex file defining the lexical analyzer.
*   **`parser.y`**: bison file defining the syntactical analyzer and semantic actions.
*   **`polynomial.h`**: Header file declaring the data structures and function prototypes for polynomial manipulation.
*   **`polynomial.c`**: C source file containing the implementation of polynomial operations and the interpreter logic.
*   **`correct.poly`**: Test file containing correct polynomial expressions to demonstrate the functionality of the interpreter.
*   **`test.poly`**: Another test file for additional testing.
*   **`report.pdf`**: PDF file containing a detailed report on the project's design, implementation details, algorithms, and testing results.

## Features

*   **Polynomial Definition:** Assign polynomial expressions to variables.
*   **Arithmetic Operations:** Perform addition, subtraction, multiplication, division (by constant values), and exponentiation on polynomials.
*   **Evaluation:** Evaluate polynomial expressions by assigning values to variables.
*   **Output:** Display polynomial expressions in a readable format.
*   **Error Handling:** Provides informative error messages for lexical, syntactical, and semantic errors.

## Building and Running

### Prerequisites

*   `flex`
*   `bison`
*   `gcc` or another C compiler
*   `make`

### Building

1.  Clone the repository:

    ```
    git clone [repository URL]
    ```

2.  Compile the program using `make`:

    ```
    make
    ```

### Running

To run the interpreter, use the following command:
  ```
  ./polycalc < <input_file>
  ```

where `<input_file>` is a file containing polynomial expressions and commands.
