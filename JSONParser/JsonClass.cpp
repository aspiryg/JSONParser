#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <variant>


struct Json;

using JsonArray = std::vector<Json>;
using JsonObject = std::map<std::string, Json>;
using JsonValue = std::variant<std::nullptr_t, bool, double, std::string, JsonArray, JsonObject>;

struct  Json
{
	JsonValue value;

	// Constructors
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

	// checkers
	bool is_null() const { return std::holds_alternative<std::nullptr_t>(value); }
	bool is_bool() const { return std::holds_alternative<bool>(value); }
	bool is_number() const { return std::holds_alternative<double>(value); }
	bool is_string() const { return std::holds_alternative<std::string>(value); }
	bool is_array() const { return std::holds_alternative<JsonArray>(value); }
	bool is_object() const { return std::holds_alternative<JsonObject>(value); }
};

