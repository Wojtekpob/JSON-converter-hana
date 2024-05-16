# Data Converter Code Generation Using Boost.Hana

This project creates an application that automatically generates data converter code between JSON format and a data structure in C++. The data structure description is provided as metadata, and the application uses the Boost.Hana library to analyze this metadata and generate the converter code.

## Description

The program leverages Boost.Hana for compile-time reflection of C++ structures and nlohmann::json for JSON serialization and deserialization. It allows for the conversion of complex nested structures between JSON and C++ data structures.

### Key Features

- **Automatic Code Generation**: The application generates the necessary code to convert between JSON and C++ data structures based on metadata.
- **Boost.Hana Integration**: Utilizes Boost.Hana to access and iterate over structure fields at compile time.
- **Nested Structures**: Supports conversion of nested data structures.

### Example

Given a C++ structure like `Person` which contains a nested `Job` structure, which in turn contains a `Car` structure, the program can convert instances of `Person` to JSON and back, automatically handling all nested fields.

#### Structures

```cpp
struct Car {
    std::string make;
    std::string model;
};

struct Job {
    std::string title;
    double salary;
    Car car;
};

struct Person {
    std::string name;
    int age;
    Job job;
};
