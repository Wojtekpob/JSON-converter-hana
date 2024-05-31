/*
Opis zagadnienia:
Program wykorzystuje bibliotekę Boost.Hana do automatycznego generowania kodu
konwertera danych między formatem JSON a strukturą danych w języku C++.
Struktury danych są adaptowane do Boost.Hana za pomocą makr BOOST_HANA_ADAPT_STRUCT, 
co pozwala na iterowanie po polach struktur i automatyczne konwertowanie ich na JSON oraz z JSON.

Boost.Hana pozwala na metaprogramowanie w C++ i jest szczególnie przydatna do pracy z typami złożonymi, 
ułatwiając manipulację i analizę typów na etapie kompilacji.

Materiały i odnośniki:
1. Boost.Hana - https://www.boost.org/doc/libs/1_75_0/libs/hana/doc/html/index.html
2. nlohmann/json - https://github.com/nlohmann/json
*/

#include <boost/hana.hpp>
#include <string>
#include <nlohmann/json.hpp>
#include <iostream>
#include <stdexcept>

// Używamy przestrzeni nazw hana z biblioteki Boost.Hana.
namespace hana = boost::hana;
using json = nlohmann::json;
using namespace std;

// Struktura danych reprezentująca samochód.
struct Car {
    std::string make;
    std::string model;
};
// Adaptujemy strukturę Car do Boost.Hana, co umożliwia jej użycie w szablonach Boost.Hana.
BOOST_HANA_ADAPT_STRUCT(Car, make, model);

// Struktura danych reprezentująca pracę.
struct Job {
    std::string title;
    double salary;
    Car car;
};
// Adaptujemy strukturę Job do Boost.Hana.
BOOST_HANA_ADAPT_STRUCT(Job, title, salary, car);

// Struktura danych reprezentująca osobę.
struct Person {
    std::string name;
    int age;
    Job job;
};
// Adaptujemy strukturę Person do Boost.Hana.
BOOST_HANA_ADAPT_STRUCT(Person, name, age, job);

// Szablon funkcji konwertującej strukturę danych na obiekt JSON.
// Funkcja przyjmuje dowolną strukturę, która została zaadaptowana do Boost.Hana.
template <typename Struct>
json to_json(const Struct& s) {
    json j;
    // Iteracja po wszystkich polach struktury za pomocą hana::for_each.
    hana::for_each(hana::accessors<Struct>(), [&](auto accessor) {
        // Pobieranie nazwy pola.
        auto name = hana::to<char const*>(hana::first(accessor));
        // Pobieranie wartości pola.
        auto value = hana::second(accessor)(s);
        // Sprawdzanie, czy wartość pola jest również strukturą zaadaptowaną do Boost.Hana.
        if constexpr (hana::Struct<decltype(value)>::value) {
            // Rekurencyjne wywołanie to_json dla zagnieżdżonej struktury.
            j[name] = to_json(value); 
        } else {
            // Przypisanie wartości pola do obiektu JSON.
            j[name] = value;
        }
    });
    return j;
}

// Szablon funkcji konwertującej obiekt JSON na strukturę danych.
// Funkcja przyjmuje obiekt JSON oraz strukturę, do której zostaną zapisane dane.
template <typename Struct>
void from_json(const json& j, Struct& s) {
    // Iteracja po wszystkich polach struktury za pomocą hana::for_each.
    hana::for_each(hana::accessors<Struct>(), [&](auto accessor) {
        // Pobieranie nazwy pola.
        auto name = hana::to<char const*>(hana::first(accessor));
        // Pobieranie referencji do wartości pola.
        auto& value = hana::second(accessor)(s);
        try {
            // Sprawdzanie, czy wartość pola jest również strukturą zaadaptowaną do Boost.Hana.
            if constexpr (hana::Struct<decltype(value)>::value) {
                // Rekurencyjne wywołanie from_json dla zagnieżdżonej struktury.
                from_json(j.at(name), value); 
            } else {
                // Pobranie wartości pola z obiektu JSON i zapisanie jej w strukturze.
                j.at(name).get_to(value);
            }
        } catch (const std::exception& e) {
            // Obsługa wyjątków, np. jeśli pole nie istnieje w obiekcie JSON.
            cerr << "Error parsing field " << name << ": " << e.what() << endl;
            throw;
        }
    });
}

// Funkcja pomocnicza do wyświetlania danych osoby.
void print_person(const Person& person) {
    cout << "Name: " << person.name << endl;
    cout << "Age: " << person.age << endl;
    cout << "Job title: " << person.job.title << endl;
    cout << "Job salary: " << person.job.salary << endl;
    cout << "Car make: " << person.job.car.make << endl;
    cout << "Car model: " << person.job.car.model << endl;
}

// Funkcja main() - punkt wejścia programu.
int main() {
    // Tworzenie obiektu Person i wyświetlanie jego danych.
    Person person{"John Doe", 30, {"Software Engineer", 100000, {"Tesla", "Model S"}}};
    print_person(person);

    // Konwersja obiektu Person na JSON i wyświetlanie wyniku.
    json j = to_json(person);
    cout << "JSON: " << j.dump(4) << endl;

    // Konwersja obiektu JSON z powrotem na strukturę Person i wyświetlanie wyników.
    Person person2;
    from_json(j, person2);
    print_person(person2);

    return 0;
}


