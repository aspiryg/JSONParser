# JSON Parser & Stringifier (C++) — Work in Progress 🚧

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

### JSON Representation

JSON values are represented using a tagged union approach with `std::variant`:

- `null`
- `bool`
- `number` (`double`)
- `string`
- `array` (`std::vector<Json>`)
- `object` (`std::map<std::string, Json>`)

This allows a `Json` value to naturally hold any valid JSON type while keeping the API simple and type-safe.

### Example (Current Implementation)

```cpp
struct Json;

using JsonArray = std::vector<Json>;
using JsonObject = std::map<std::string, Json>;
using JsonValue = std::variant<
    std::nullptr_t,
    bool,
    double,
    std::string,
    JsonArray,
    JsonObject
>;

struct Json {
    JsonValue value;

    Json() : value(nullptr) {}
    Json(std::nullptr_t) : value(nullptr) {}
    Json(bool b) : value(b) {}
    Json(double d) : value(d) {}
    Json(int i) : value(double(i)) {}
    Json(const char* s) : value(std::string(s)) {}
    Json(const std::string& s) : value(s) {}
    Json(std::string&& s) : value(std::move(s)) {}
    Json(const JsonArray& a) : value(a) {}
    Json(JsonArray&& a) : value(std::move(a)) {}
    Json(const JsonObject& o) : value(o) {}
    Json(JsonObject&& o) : value(std::move(o)) {}

    bool is_null()   const { return std::holds_alternative<std::nullptr_t>(value); }
    bool is_bool()   const { return std::holds_alternative<bool>(value); }
    bool is_number() const { return std::holds_alternative<double>(value); }
    bool is_string() const { return std::holds_alternative<std::string>(value); }
    bool is_array()  const { return std::holds_alternative<JsonArray>(value); }
    bool is_object() const { return std::holds_alternative<JsonObject>(value); }
};
