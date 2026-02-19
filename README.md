# Math Expression Evaluator (C++)

A simple console application that parses and evaluates mathematical expressions.

The project was created for educational purposes while learning modern C++ and classic parsing algorithms.

## Features

- Supports operators: `+`, `-`, `*`, `/`
- Supports parentheses
- Supports unary minus
- Floating point numbers
- Division by zero detection
- Expression validation before evaluation

## How It Works

The evaluation pipeline consists of several steps:

1. **Tokenization**  
   The input string is converted into a sequence of tokens.

2. **Validation**  
   The expression is checked for:
   - invalid characters
   - invalid numbers
   - unbalanced parentheses
   - invalid token order

3. **Unary minus handling**  
   Binary minus is converted to unary minus where appropriate.

4. **Shunting Yard algorithm**  
   The infix expression is converted into Reverse Polish Notation (RPN).

5. **RPN evaluation**  
   The RPN expression is evaluated using a value stack.

The Shunting Yard algorithm is based on the classic algorithm proposed by Edsger Dijkstra.

## Example

Input:
-(-3.4 * (-2.3+4.85) + (-1.23)) / .89

Output:
11.1236


## Project Structure

- `Tokenizer.*` — tokenization and validation
- `ShuntingYard.*` — infix to RPN conversion
- `Evaluator.*` — RPN evaluation
- `Main.cpp` — application entry point

## References

- Dijkstra, E. W. — Shunting Yard Algorithm
- https://en.wikipedia.org/wiki/Shunting_yard_algorithm
- https://habr.com/ru/articles/908062/

---

This project is intended for learning purposes and demonstrating understanding of parsing and stack-based evaluation.
