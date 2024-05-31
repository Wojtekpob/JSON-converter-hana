/*
Opis:
Program wykorzystuje bibliotekę Boost.Hana do automatycznego generowania kodu
konwertera danych między formatem JSON a strukturą danych w języku C++.
Struktury danych są adaptowane do Boost.Hana za pomocą makr BOOST_HANA_ADAPT_STRUCT, 
co pozwala na iterowanie po polach struktur i automatyczne konwertowanie ich na JSON oraz z JSON.


Skrócony przewodnik użycia konwerterów:
1. Definiowanie struktur danych:
   - Utwórz struktury danych, które mają być konwertowane, np. `Car`, `Job`, `Person`.
   - Zaadoptuj struktury do Boost.Hana używając `BOOST_HANA_ADAPT_STRUCT`.

2. Konwersja struktur do JSON:
   - Użyj funkcji `to_json` przekazując strukturę, np. `json j = to_json(person);`.
   - Funkcja ta iteruje po wszystkich polach struktury i konwertuje je do odpowiednich wartości JSON.

3. Konwersja JSON do struktur:
   - Użyj funkcji `from_json` przekazując obiekt JSON oraz strukturę, do której mają zostać zapisane dane, np. `from_json(j, person2);`.
   - Funkcja ta iteruje po wszystkich polach struktury i wypełnia je danymi z obiektu JSON.


Materiały i odnośniki:
1. Boost.Hana - https://www.boost.org/doc/libs/1_75_0/libs/hana/doc/html/index.html
2. nlohmann/json - https://github.com/nlohmann/json
*/

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
// Adaptacja struktur danych Car, Job, Person do Boost.Hana, co umożliwia jej użycie w szablonach
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

// Szablon funkcji konwertującej strukturę danych na obiekt JSON.
// Funkcja przyjmuje dowolną strukturę, która została zaadaptowana do Boost.Hana.
template <typename Struct>
json to_json(const Struct& s) {
    json j;
    // Iteracja po wszystkich polach struktury za pomocą hana::for_each.
    hana::for_each(hana::accessors<Struct>(), [&](auto accessor) {
        auto name = hana::to<char const*>(hana::first(accessor));
        auto value = hana::second(accessor)(s);
        // Sprawdzanie, czy wartość pola jest również strukturą zaadaptowaną do Boost.Hana.
        if constexpr (hana::Struct<decltype(value)>::value) {
            // Rekurencyjne wywołanie to_json dla zagnieżdżonej struktury.
            j[name] = to_json(value);
        } else {
            j[name] = value;
        }
    });
    return j;
}

// Szablon funkcji konwertującej obiekt JSON na strukturę danych.
// Funkcja przyjmuje obiekt JSON oraz strukturę, do której zostaną zapisane dane.
template <typename Struct>
void from_json(const json& j, Struct& s) {
    hana::for_each(hana::accessors<Struct>(), [&](auto accessor) {
        auto name = hana::to<char const*>(hana::first(accessor));
        auto& value = hana::second(accessor)(s);
        try {
            // Sprawdzanie, czy wartość pola jest również strukturą zaadaptowaną do Boost.Hana.
            if constexpr (hana::Struct<decltype(value)>::value) {
                // Rekurencyjne wywołanie from_json dla zagnieżdżonej struktury.
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


