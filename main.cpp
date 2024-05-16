#include <boost/hana.hpp>
#include <string>
#include <nlohmann/json.hpp>
#include <iostream>
#include <stdexcept>

namespace hana = boost::hana;
using json = nlohmann::json;
using namespace std;


struct Car {
    std::string make;
    std::string model;
};
BOOST_HANA_ADAPT_STRUCT(Car, make, model);


struct Job {
    std::string title;
    double salary;
    Car car;
};
BOOST_HANA_ADAPT_STRUCT(Job, title, salary, car);


struct Person {
    std::string name;
    int age;
    Job job;
};
BOOST_HANA_ADAPT_STRUCT(Person, name, age, job);


template <typename Struct>
json to_json(const Struct& s) {
    json j;
    hana::for_each(hana::accessors<Struct>(), [&](auto accessor) {
        auto name = hana::to<char const*>(hana::first(accessor));
        auto value = hana::second(accessor)(s);
        if constexpr (hana::Struct<decltype(value)>::value) {
            j[name] = to_json(value); 
        } else {
            j[name] = value;
        }
    });
    return j;
}


template <typename Struct>
void from_json(const json& j, Struct& s) {
    hana::for_each(hana::accessors<Struct>(), [&](auto accessor) {
        auto name = hana::to<char const*>(hana::first(accessor));
        auto& value = hana::second(accessor)(s);
        try {
            if constexpr (hana::Struct<decltype(value)>::value) {
                from_json(j.at(name), value); 
            } else {
                j.at(name).get_to(value);
            }
        } catch (const std::exception& e) {
            cerr << "Error parsing field " << name << ": " << e.what() << endl;
            throw;
        }
    });
}


void print_person(const Person& person) {
    cout << "Name: " << person.name << endl;
    cout << "Age: " << person.age << endl;
    cout << "Job title: " << person.job.title << endl;
    cout << "Job salary: " << person.job.salary << endl;
    cout << "Car make: " << person.job.car.make << endl;
    cout << "Car model: " << person.job.car.model << endl;
}


int main() {
    Person person{"John Doe", 30, {"Software Engineer", 100000, {"Tesla", "Model S"}}};
    print_person(person);
    json j = to_json(person);
    cout << "JSON: " << j.dump(4) << endl;

    Person person2;
    from_json(j, person2);
    print_person(person2);
    return 0;
}