# Interpreter In C++
This is a little project ive taken upon myself to create an interpreter from scratch, no proper guidance and in C++. Isn't this fun..

## Main Goals
- Make a semi-decent language that at least includes, apart from the basics, functions and classes/objects.
- Learn proper C++ techniques and evolve my understanding of the language.
- Heavy focus on error handling - show informative errors that actually help the user (i love rust errors; we'll see how close we can get).
- Enforce types within the language for variables (int, float, string, etc.).

## Project Setup
C++ Version: 23
C++ Modules: Yes

## Language Features

#### Semicolon ending

Statements and expressions must end in a semicolon. For example `out 10;` must end on a semicolon. This is also true for: printing, varaible declarations, variable modification, stand alone expression statements.

### Operators

#### Equality and Comparison

All of the following operators are implemented inside of the language.
* `==` - Equal to.
* `!=` - Not equal to.
* `>` - More than.
* `<=` - More than or equal to.
* `>` - Less than.
* `>=` - Less than or equal to.

#### Arithmatic

* `+` - Plus.
* `-` - Minus.
* `*` - Multiply.
* `/` - Divide.

#### Unary
* `!` - Not (flips boolean).
* `-` - Negates value.

### Keywords

#### Printing

* `out` - Print, no new line.
* `outln` - Print with new line appended at the end.

#### Types

* `bool` - Boolean values.
* `string` - String values.
* `number` - Number values (integer and floating point).

#### Logic

* `or` - Logic or.
* `and` - Logic and.

#### Variable Declaration

* `var` - Declare Variable in format: `var <identifier> -> <type> = <value>` (anything inside of <...> is a placeholder).

#### Looping 

* `loop` - Defines an infinite loop or a while like loop. `loop {...` will define an infinite loop. `loop (<expression>) {...` defines a while like loop.

#### Conditional

* `if` - Defines a if statement. Format: `if (<expression>) {...`.
* `else` - Defines an else block to an if, if the condition is falsey. Format: `if (...) {...} else {...`.

