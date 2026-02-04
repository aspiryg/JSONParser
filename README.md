# JSON Parser & Stringifier (C++) - Work in Progress

This project is an in-progress implementation of a JSON parser and stringifier written **from scratch in C++**.  
The goal is not to replace existing JSON libraries, but to deeply understand how JSON parsing works internally while strengthening my grasp of **C++ fundamentals, data structures, memory models, and design choices**.

The project is intentionally built step by step, starting from the core data representation and gradually moving toward a full lexer, parser, and serializer.

---

## Motivation

I started this project as a learning and portfolio exercise to:
- Practice modern C++ features (`std::variant`, containers, value semantics)
- Understand how JSON data can be represented in memory
- Build a parser without relying on external libraries
- Explore different design approaches (value-based vs polymorphic AST)
- Improve my understanding of parsing, recursion, and structured data

---

## Current State

At the moment, the project focuses on **defining a clean and flexible JSON data model**.
