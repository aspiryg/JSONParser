/**
 * JSON Parser - A complete C++ JSON parsing and serialization library
 * 
 * This example demonstrates all the features of the JSON parser including:
 * - Parsing JSON strings
 * - Creating JSON objects programmatically
 * - Accessing values
 * - Serializing to JSON strings
 * - Error handling
 */

#include "Json.h"
#include <iostream>
#include <iomanip>

void print_separator(const std::string& title)
{
	std::cout << "\n" << std::string(60, '=') << "\n";
	std::cout << "  " << title << "\n";
	std::cout << std::string(60, '=') << "\n\n";
}

// Example 1: Parsing a simple JSON string
void example_parse_simple()
{
	print_separator("Example 1: Parsing Simple JSON");

	std::string json_str = R"({
		"name": "John Doe",
		"age": 30,
		"isStudent": false,
		"score": 95.5
	})";

	try {
		Json json = Json::parse(json_str);

		std::cout << "Parsed JSON:\n";
		std::cout << json.stringify(true) << "\n\n";

		std::cout << "Accessing individual values:\n";
		std::cout << "Name: " << json["name"].as_string() << "\n";
		std::cout << "Age: " << json["age"].as_number() << "\n";
		std::cout << "Is Student: " << (json["isStudent"].as_bool() ? "true" : "false") << "\n";
		std::cout << "Score: " << json["score"].as_number() << "\n";
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << "\n";
	}
}

// Example 2: Creating JSON programmatically
void example_create_json()
{
	print_separator("Example 2: Creating JSON Programmatically");

	Json person;
	person = JsonObject{
		{"firstName", "Alice"},
		{"lastName", "Smith"},
		{"age", 28},
		{"email", "alice@example.com"},
		{"isActive", true}
	};

	std::cout << "Created JSON object:\n";
	std::cout << person.stringify(true) << "\n";
}

// Example 3: Working with arrays
void example_arrays()
{
	print_separator("Example 3: Working with JSON Arrays");

	std::string json_str = R"({
		"students": [
			{"name": "Alice", "grade": 90},
			{"name": "Bob", "grade": 85},
			{"name": "Charlie", "grade": 92}
		],
		"numbers": [1, 2, 3, 4, 5]
	})";

	try {
		Json json = Json::parse(json_str);

		std::cout << "Students:\n";
		const JsonArray& students = json["students"].as_array();
		for (size_t i = 0; i < students.size(); ++i) {
			std::cout << "  " << i + 1 << ". "
				<< students[i]["name"].as_string() << " - "
				<< "Grade: " << students[i]["grade"].as_number() << "\n";
		}

		std::cout << "\nNumbers: ";
		const JsonArray& numbers = json["numbers"].as_array();
		for (size_t i = 0; i < numbers.size(); ++i) {
			std::cout << numbers[i].as_number();
			if (i < numbers.size() - 1) std::cout << ", ";
		}
		std::cout << "\n";
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << "\n";
	}
}

// Example 4: Creating nested structures
void example_nested_structures()
{
	print_separator("Example 4: Creating Nested Structures");

	Json company;
	company = JsonObject{
		{"name", "Tech Corp"},
		{"founded", 2010},
		{"departments", JsonArray{
			JsonObject{
				{"name", "Engineering"},
				{"employees", 50},
				{"projects", JsonArray{"Project A", "Project B", "Project C"}}
			},
			JsonObject{
				{"name", "Sales"},
				{"employees", 30},
				{"projects", JsonArray{"Campaign 1", "Campaign 2"}}
			}
		}},
		{"address", JsonObject{
			{"street", "123 Main St"},
			{"city", "San Francisco"},
			{"state", "CA"},
			{"zip", "94105"}
		}}
	};

	std::cout << "Complex nested JSON structure:\n";
	std::cout << company.stringify(true) << "\n";

	std::cout << "\nAccessing nested values:\n";
	std::cout << "Company: " << company["name"].as_string() << "\n";
	std::cout << "City: " << company["address"]["city"].as_string() << "\n";
	std::cout << "First department: "
		<< company["departments"][0]["name"].as_string() << "\n";
}

// Example 5: Type checking
void example_type_checking()
{
	print_separator("Example 5: Type Checking");

	Json values = JsonArray{
		nullptr,
		true,
		42,
		3.14,
		"Hello",
		JsonArray{1, 2, 3},
		JsonObject{{"key", "value"}}
	};

	const JsonArray& arr = values.as_array();
	for (size_t i = 0; i < arr.size(); ++i) {
		std::cout << "Value " << i << " is: ";

		if (arr[i].is_null()) std::cout << "null";
		else if (arr[i].is_bool()) std::cout << "boolean (" << arr[i].as_bool() << ")";
		else if (arr[i].is_number()) std::cout << "number (" << arr[i].as_number() << ")";
		else if (arr[i].is_string()) std::cout << "string (\"" << arr[i].as_string() << "\")";
		else if (arr[i].is_array()) std::cout << "array";
		else if (arr[i].is_object()) std::cout << "object";

		std::cout << "\n";
	}
}

// Example 6: Modifying JSON
void example_modifying_json()
{
	print_separator("Example 6: Modifying JSON");

	Json data = JsonObject{
		{"users", JsonArray{
			JsonObject{{"name", "Alice"}, {"age", 25}},
			JsonObject{{"name", "Bob"}, {"age", 30}}
		}}
	};

	std::cout << "Original JSON:\n";
	std::cout << data.stringify(true) << "\n\n";

	// Add a new user
	data["users"].as_array().push_back(
		JsonObject{{"name", "Charlie"}, {"age", 35}}
	);

	// Modify existing user
	data["users"][0]["age"] = 26;

	std::cout << "Modified JSON:\n";
	std::cout << data.stringify(true) << "\n";
}

// Example 7: Error handling
void example_error_handling()
{
	print_separator("Example 7: Error Handling");

	// Invalid JSON examples
	std::vector<std::string> invalid_jsons = {
		"{invalid}",
		"{'key': 'value'}",  // Single quotes not allowed
		"{\"key\": undefined}",
		"[1, 2, 3,]",  // Trailing comma
		"{\"key\": \"value\""  // Missing closing brace
	};

	for (size_t i = 0; i < invalid_jsons.size(); ++i) {
		std::cout << "Attempting to parse: " << invalid_jsons[i] << "\n";
		try {
			Json json = Json::parse(invalid_jsons[i]);
			std::cout << "  Unexpectedly succeeded!\n";
		}
		catch (const JsonParseError& e) {
			std::cout << "  Parse error: " << e.what() << "\n";
		}
		catch (const std::exception& e) {
			std::cout << "  Error: " << e.what() << "\n";
		}
		std::cout << "\n";
	}
}

// Example 8: Real-world scenario - API response
void example_api_response()
{
	print_separator("Example 8: Real-World API Response");

	std::string api_response = R"({
		"status": "success",
		"data": {
			"user": {
				"id": 12345,
				"username": "johndoe",
				"email": "john@example.com",
				"profile": {
					"firstName": "John",
					"lastName": "Doe",
					"bio": "Software developer",
					"location": "New York"
				},
				"posts": [
					{
						"id": 1,
						"title": "First Post",
						"likes": 42,
						"comments": 5
					},
					{
						"id": 2,
						"title": "Second Post",
						"likes": 128,
						"comments": 12
					}
				],
				"followers": 567,
				"following": 234
			}
		},
		"timestamp": 1234567890
	})";

	try {
		Json response = Json::parse(api_response);

		std::cout << "API Status: " << response["status"].as_string() << "\n\n";

		const Json& user = response["data"]["user"];
		std::cout << "User Information:\n";
		std::cout << "  Username: " << user["username"].as_string() << "\n";
		std::cout << "  Email: " << user["email"].as_string() << "\n";
		std::cout << "  Name: " << user["profile"]["firstName"].as_string() << " "
			<< user["profile"]["lastName"].as_string() << "\n";
		std::cout << "  Location: " << user["profile"]["location"].as_string() << "\n";
		std::cout << "  Followers: " << user["followers"].as_number() << "\n";
		std::cout << "  Following: " << user["following"].as_number() << "\n\n";

		std::cout << "Posts:\n";
		const JsonArray& posts = user["posts"].as_array();
		for (size_t i = 0; i < posts.size(); ++i) {
			std::cout << "  " << i + 1 << ". " << posts[i]["title"].as_string() << "\n";
			std::cout << "     Likes: " << posts[i]["likes"].as_number()
				<< ", Comments: " << posts[i]["comments"].as_number() << "\n";
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << "\n";
	}
}

int main()
{
	std::cout << R"(
  ╔══════════════════════════════════════════════════════════╗
  ║                                                          ║
  ║              JSON Parser - Complete Examples            ║
  ║                                                          ║
  ║  A learning project demonstrating:                      ║
  ║  • JSON parsing and serialization                       ║
  ║  • Data structures (variant, map, vector)               ║
  ║  • Tokenization and parsing algorithms                  ║
  ║  • Error handling and type safety                       ║
  ║                                                          ║
  ╚══════════════════════════════════════════════════════════╝
)";

	try {
		example_parse_simple();
		example_create_json();
		example_arrays();
		example_nested_structures();
		example_type_checking();
		example_modifying_json();
		example_error_handling();
		example_api_response();

		std::cout << "\n" << std::string(60, '=') << "\n";
		std::cout << "  All examples completed successfully!\n";
		std::cout << std::string(60, '=') << "\n\n";
	}
	catch (const std::exception& e) {
		std::cerr << "Fatal error: " << e.what() << "\n";
		return 1;
	}

	return 0;
}
