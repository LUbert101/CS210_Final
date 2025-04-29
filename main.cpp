#include <iostream>

int main() {
    const string fileName = "world_cities.csv";
    Cache cache(10);
    string countryCode, cityName;

    while (true) {
        cout << "Enter country code (or 'exit' to quit): ";
        cin >> countryCode;
        if (countryCode == "exit") break;

        cout << "Enter city name: ";
        cin.ignore();
        getline(cin, cityName);

        string key = countryCode + "," + cityName;
        string population = cache.get(key);

        if (!population.empty()) {
            cout << "Population (from cache): " << population << endl;
        } else {
            population = searchCSV(fileName, countryCode, cityName);
            if (population != "Not Found") {
                cache.put(key, population);
            }
            cout << "Population: " << population << endl;
        }

        cache.display();
    }

    return 0;

}