# Math Expression Evaluator

[![CI](https://github.com/IliaRakhlevski/Math-Expression-Evaluator/actions/workflows/ci.yml/badge.svg)](https://github.com/IliaRakhlevski/Math-Expression-Evaluator/actions/workflows/ci.yml)

A C++17 evaluator for infix arithmetic expressions. The input is tokenized,
checked against a table of legal token sequences, converted to Reverse
Polish Notation with Dijkstra's Shunting Yard algorithm and evaluated on a
value stack. Every failure is reported as a specific error code instead of a
wrong number.

## Usage

One expression from the command line (exit code 0 on success, 1 on error):

```
$ math_eval "-(-3.4 * (-2.3 + 4.85) + (-1.23)) / .89"
= 11.1235955056
$ math_eval "1 / (2 - 2)"
Error: Division by zero
```

Interactive mode: one expression per line, an empty line or `quit` exits.

```
$ math_eval
Enter an expression (empty line or "quit" to exit).
> 2 - 3 - 4
= -5
> (1 + 2
Error: Unbalanced parentheses
> 2 3
Error: Invalid expression
```

From code, a single call runs the whole pipeline:

```cpp
#include "Calculator.h"

CalcResult r = calculate("8 / 2 / 2");
if (r.ok())
    std::cout << r.value;             // 2
else
    std::cout << to_message(r.error);
```

## Supported syntax

| Element | Examples |
|---|---|
| Numbers | `42`, `3.14`, `.5`, `2.` |
| Binary operators | `+ - * /` (`*` `/` bind tighter; all left-associative) |
| Unary minus | `-5`, `-(2 + 3)`, `4 * (-2)` |
| Parentheses | any depth |
| Spaces | ignored between tokens |

## Errors

| Code | Example input |
|---|---|
| `EMPTY_EXPRESSION` | `""`, `"   "` |
| `INVALID_CHAR` | `2 + a` |
| `INVALID_NUMBER` | `1.2.3`, `.` |
| `UNBALANCED_PARENTHESIS` | `(1 + 2`, `)(` |
| `INVALID_EXPRESSION` | `2 3`, `()`, `1 +`, `(2)(3)` |
| `DIVISION_BY_ZERO` | `1 / 0`, `1 / (2 - 2)` |

## Limitations

These are deliberate scope decisions, each covered by a test:

- Unary minus is accepted only at the start of the expression or right after
  `(`. `2 * -3` is rejected; write `2 * (-3)`.
- No exponent notation (`1e3`), no unary plus, no `^`, no functions.
- Results are `double`; printing uses 12 significant digits.

## How it works

```
"2*(3+4)" ──► Tokenizer ──► validate ──► unary minus ──► ShuntingYard ──► evaluate_rpn ──► 14
               tokens       legal order   "-" → negate     RPN: 2 3 4 + *   value stack
```

1. **Tokenize** (`Tokenizer::tokenize`). Characters become tokens. A token is
   `std::variant<double, TokenOperator, TokenParen>`: a number carries its
   value, an operator or parenthesis carries only its kind. Numbers are parsed
   with `std::stod` and rejected unless the whole text was consumed, so
   `1.2.3` is an error rather than `1.2`. Parenthesis balance is counted here.
2. **Validate** (`Tokenizer::validate`). A table lists which token may follow
   which (after a number: an operator or `)`; after `(`: a number, `-` or `(`;
   and so on), plus rules for the first and last token. Any illegal pair is
   rejected. Because the grammar lives in data, extending it means editing the
   table, not a chain of `if`s.
3. **Mark unary minus** (`Tokenizer::update_unary_minus`). A `-` at the start or
   after `(` becomes `UNARY_MINUS`, a separate operator with the highest
   precedence and right associativity.
4. **Shunting Yard** (`ShuntingYard::create_rpn`). Operators wait on a stack and
   are moved to the output according to precedence and associativity. This is
   what makes `2-3-4` equal `(2-3)-4` and `1+2*3` equal `1+(2*3)`.
5. **Evaluate RPN** (`evaluate_rpn`). Numbers are pushed; each operator pops its
   operands and pushes the result. A zero divisor is detected with an exact
   comparison: only a true zero is undefined, and `1 / 1e-20` is a valid result.

`calculate()` (`Calculator.h`) runs the five steps in the required order and
maps their status codes to one public `CalcError`. Callers never need to know
the order, which removes the easiest way to misuse the lower-level classes.

## Layout

```
Tokenizer.h/.cpp      tokens, tokenizing, validation, unary minus
ShuntingYard.h/.cpp   infix → RPN
Evaluator.h/.cpp      RPN → value
Calculator.h/.cpp     calculate(): the whole pipeline in one call
Main.cpp              console front end (one-shot and interactive)
tests/                tests without a framework: CHECK macro + summary
CMakeLists.txt        library mee_core, program math_eval, test_calculator
```

## Building

Requires CMake 3.16+ and a C++17 compiler (GCC, Clang or MSVC).

```
cmake -S . -B build
cmake --build build
```

In Visual Studio: *File → Open → Folder*, pick the repository folder; the
CMake project is detected automatically.

## Testing

```
ctest --test-dir build --output-on-failure
```

The tests cover values, precedence and associativity, unary minus, division
by zero, every error code, the documented limitations, and the RPN produced
for known inputs. CI builds with GCC, Clang, GCC with AddressSanitizer and
UndefinedBehaviorSanitizer, and MSVC, with warnings treated as errors.

## References

- E. W. Dijkstra, the Shunting Yard algorithm —
  <https://en.wikipedia.org/wiki/Shunting_yard_algorithm>
- <https://habr.com/ru/articles/908062/>
