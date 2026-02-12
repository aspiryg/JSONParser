#include "Json.h"
#include <cctype>
#include <stdexcept>
#include <iomanip>

// ========== Getters ==========

bool Json::as_bool() const
{
	if (!is_bool())
		throw std::runtime_error("JSON value is not a boolean");
	return std::get<bool>(value);
}

double Json::as_number() const
{
	if (!is_number())
		throw std::runtime_error("JSON value is not a number");
	return std::get<double>(value);
}

const std::string& Json::as_string() const
{
	if (!is_string())
		throw std::runtime_error("JSON value is not a string");
	return std::get<std::string>(value);
}

const JsonArray& Json::as_array() const
{
	if (!is_array())
		throw std::runtime_error("JSON value is not an array");
	return std::get<JsonArray>(value);
}

const JsonObject& Json::as_object() const
{
	if (!is_object())
		throw std::runtime_error("JSON value is not an object");
	return std::get<JsonObject>(value);
}

JsonArray& Json::as_array()
{
	if (!is_array())
		throw std::runtime_error("JSON value is not an array");
	return std::get<JsonArray>(value);
}

JsonObject& Json::as_object()
{
	if (!is_object())
		throw std::runtime_error("JSON value is not an object");
	return std::get<JsonObject>(value);
}

// ========== Array/Object Access Operators ==========

Json& Json::operator[](size_t index)
{
	return as_array()[index];
}

const Json& Json::operator[](size_t index) const
{
	return as_array()[index];
}

Json& Json::operator[](const std::string& key)
{
	return as_object()[key];
}

const Json& Json::operator[](const std::string& key) const
{
	auto& obj = as_object();
	auto it = obj.find(key);
	if (it == obj.end())
		throw std::runtime_error("Key not found: " + key);
	return it->second;
}

// ========== Serialization ==========

std::string Json::stringify(bool pretty, int indent) const
{
	std::ostringstream oss;
	auto make_indent = [&]() {
		if (pretty) {
			for (int i = 0; i < indent * 2; ++i)
				oss << ' ';
		}
	};

	if (is_null()) {
		oss << "null";
	}
	else if (is_bool()) {
		oss << (as_bool() ? "true" : "false");
	}
	else if (is_number()) {
		double num = as_number();
		// Check if the number is an integer
		if (num == static_cast<long long>(num)) {
			oss << static_cast<long long>(num);
		}
		else {
			oss << num;
		}
	}
	else if (is_string()) {
		oss << '"';
		for (char c : as_string()) {
			switch (c) {
			case '"': oss << "\\\""; break;
			case '\\': oss << "\\\\"; break;
			case '\b': oss << "\\b"; break;
			case '\f': oss << "\\f"; break;
			case '\n': oss << "\\n"; break;
			case '\r': oss << "\\r"; break;
			case '\t': oss << "\\t"; break;
			default:
				if (static_cast<unsigned char>(c) < 0x20) {
					oss << "\\u" << std::hex << std::setw(4) << std::setfill('0')
						<< static_cast<int>(c);
				}
				else {
					oss << c;
				}
			}
		}
		oss << '"';
	}
	else if (is_array()) {
		const auto& arr = as_array();
		oss << '[';
		if (pretty && !arr.empty()) oss << '\n';

		for (size_t i = 0; i < arr.size(); ++i) {
			if (pretty) make_indent();
			if (pretty) oss << "  ";
			oss << arr[i].stringify(pretty, indent + 1);
			if (i < arr.size() - 1) oss << ',';
			if (pretty) oss << '\n';
		}

		if (pretty && !arr.empty()) make_indent();
		oss << ']';
	}
	else if (is_object()) {
		const auto& obj = as_object();
		oss << '{';
		if (pretty && !obj.empty()) oss << '\n';

		size_t i = 0;
		for (const auto& [key, val] : obj) {
			if (pretty) make_indent();
			if (pretty) oss << "  ";
			oss << '"' << key << "\": ";
			oss << val.stringify(pretty, indent + 1);
			if (i < obj.size() - 1) oss << ',';
			if (pretty) oss << '\n';
			++i;
		}

		if (pretty && !obj.empty()) make_indent();
		oss << '}';
	}

	return oss.str();
}

// ========== Parser Implementation ==========

class JsonParser
{
public:
	JsonParser(const std::string& json) : json_(json), pos_(0) {}

	Json parse()
	{
		skip_whitespace();
		Json result = parse_value();
		skip_whitespace();
		if (pos_ < json_.size()) {
			throw JsonParseError("Unexpected characters after JSON value", pos_);
		}
		return result;
	}

private:
	const std::string& json_;
	size_t pos_;

	char current() const
	{
		if (pos_ >= json_.size())
			throw JsonParseError("Unexpected end of input", pos_);
		return json_[pos_];
	}

	char peek() const
	{
		if (pos_ >= json_.size())
			return '\0';
		return json_[pos_];
	}

	void advance()
	{
		if (pos_ < json_.size())
			++pos_;
	}

	void skip_whitespace()
	{
		while (pos_ < json_.size() && std::isspace(static_cast<unsigned char>(json_[pos_]))) {
			++pos_;
		}
	}

	Json parse_value()
	{
		skip_whitespace();
		char c = current();

		if (c == 'n') return parse_null();
		if (c == 't' || c == 'f') return parse_bool();
		if (c == '"') return parse_string();
		if (c == '[') return parse_array();
		if (c == '{') return parse_object();
		if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) return parse_number();

		throw JsonParseError("Unexpected character", pos_);
	}

	Json parse_null()
	{
		if (json_.substr(pos_, 4) == "null") {
			pos_ += 4;
			return Json(nullptr);
		}
		throw JsonParseError("Invalid null value", pos_);
	}

	Json parse_bool()
	{
		if (json_.substr(pos_, 4) == "true") {
			pos_ += 4;
			return Json(true);
		}
		if (json_.substr(pos_, 5) == "false") {
			pos_ += 5;
			return Json(false);
		}
		throw JsonParseError("Invalid boolean value", pos_);
	}

	Json parse_number()
	{
		size_t start = pos_;

		if (peek() == '-') advance();

		if (peek() == '0') {
			advance();
		}
		else if (std::isdigit(static_cast<unsigned char>(peek()))) {
			while (pos_ < json_.size() && std::isdigit(static_cast<unsigned char>(peek()))) {
				advance();
			}
		}
		else {
			throw JsonParseError("Invalid number", pos_);
		}

		if (peek() == '.') {
			advance();
			if (!std::isdigit(static_cast<unsigned char>(peek()))) {
				throw JsonParseError("Invalid number: expected digit after decimal point", pos_);
			}
			while (pos_ < json_.size() && std::isdigit(static_cast<unsigned char>(peek()))) {
				advance();
			}
		}

		if (peek() == 'e' || peek() == 'E') {
			advance();
			if (peek() == '+' || peek() == '-') advance();
			if (!std::isdigit(static_cast<unsigned char>(peek()))) {
				throw JsonParseError("Invalid number: expected digit in exponent", pos_);
			}
			while (pos_ < json_.size() && std::isdigit(static_cast<unsigned char>(peek()))) {
				advance();
			}
		}

		std::string num_str = json_.substr(start, pos_ - start);
		try {
			return Json(std::stod(num_str));
		}
		catch (...) {
			throw JsonParseError("Invalid number format", start);
		}
	}

	Json parse_string()
	{
		if (current() != '"')
			throw JsonParseError("Expected '\"'", pos_);
		advance();

		std::string result;
		while (pos_ < json_.size() && peek() != '"') {
			if (peek() == '\\') {
				advance();
				if (pos_ >= json_.size())
					throw JsonParseError("Unterminated string", pos_);

				char escaped = peek();
				switch (escaped) {
				case '"': result += '"'; break;
				case '\\': result += '\\'; break;
				case '/': result += '/'; break;
				case 'b': result += '\b'; break;
				case 'f': result += '\f'; break;
				case 'n': result += '\n'; break;
				case 'r': result += '\r'; break;
				case 't': result += '\t'; break;
				case 'u': {
					advance();
					if (pos_ + 4 > json_.size())
						throw JsonParseError("Invalid unicode escape", pos_);
					std::string hex = json_.substr(pos_, 4);
					try {
						int code_point = std::stoi(hex, nullptr, 16);
						if (code_point < 128) {
							result += static_cast<char>(code_point);
						}
						else {
							result += "?";
						}
						pos_ += 3;
					}
					catch (...) {
						throw JsonParseError("Invalid unicode escape", pos_);
					}
					break;
				}
				default:
					throw JsonParseError("Invalid escape sequence", pos_);
				}
				advance();
			}
			else {
				result += peek();
				advance();
			}
		}

		if (peek() != '"')
			throw JsonParseError("Unterminated string", pos_);
		advance();

		return Json(result);
	}

	Json parse_array()
	{
		if (current() != '[')
			throw JsonParseError("Expected '['", pos_);
		advance();
		skip_whitespace();

		JsonArray arr;

		if (peek() == ']') {
			advance();
			return Json(arr);
		}

		while (true) {
			arr.push_back(parse_value());
			skip_whitespace();

			if (peek() == ']') {
				advance();
				break;
			}

			if (peek() != ',')
				throw JsonParseError("Expected ',' or ']' in array", pos_);
			advance();
			skip_whitespace();
		}

		return Json(arr);
	}

	Json parse_object()
	{
		if (current() != '{')
			throw JsonParseError("Expected '{'", pos_);
		advance();
		skip_whitespace();

		JsonObject obj;

		if (peek() == '}') {
			advance();
			return Json(obj);
		}

		while (true) {
			skip_whitespace();

			if (peek() != '"')
				throw JsonParseError("Expected string key in object", pos_);

			std::string key = parse_string().as_string();
			skip_whitespace();

			if (peek() != ':')
				throw JsonParseError("Expected ':' after key in object", pos_);
			advance();
			skip_whitespace();

			obj[key] = parse_value();
			skip_whitespace();

			if (peek() == '}') {
				advance();
				break;
			}

			if (peek() != ',')
				throw JsonParseError("Expected ',' or '}' in object", pos_);
			advance();
		}

		return Json(obj);
	}
};

Json Json::parse(const std::string& json_string)
{
	JsonParser parser(json_string);
	return parser.parse();
}
