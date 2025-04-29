#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <list>

using namespace std;

class Cache {
private:
    unordered_map<string, pair<string, list<string>::iterator>> cacheMap;
    list<string> accessOrder;
    size_t capacity;

public:
    Cache(size_t cap) : capacity(cap) {}

    string get(const string& key) {
        if (cacheMap.find(key) != cacheMap.end()) {
            // Move accessed key to front of the list
            accessOrder.erase(cacheMap[key].second);
            accessOrder.push_front(key);
            cacheMap[key].second = accessOrder.begin();
            return cacheMap[key].first;
        }
        return "";
    }

    void put(const string& key, const string& value) {
        if (cacheMap.find(key) != cacheMap.end()) {
            // Update value and move key to front
            accessOrder.erase(cacheMap[key].second);
        } else if (cacheMap.size() >= capacity) {
            // Evict least recently used item
            string lruKey = accessOrder.back();
            accessOrder.pop_back();
            cacheMap.erase(lruKey);
        }

        // Add new item to cache
        accessOrder.push_front(key);
        cacheMap[key] = {value, accessOrder.begin()};
    }

    void display() {
        cout << "Cache contents:\n";
        for (const auto& key : accessOrder) {
            cout << key << " -> " << cacheMap[key].first << "\n";
        }
        cout << endl;
    }
};

string searchCSV(const string& fileName, const string& countryCode, const string& cityName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error opening file: " << fileName << endl;
        return "";
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string code, city, population;

        getline(ss, code, ',');
        getline(ss, city, ',');
        getline(ss, population, ',');

        if (code == countryCode && city == cityName) {
            return population;
        }
    }

    return "Not Found";
}

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