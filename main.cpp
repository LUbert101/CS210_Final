#include <iostream>
#include <fstream>
#include <unordered_map>
#include <list>
#include <random>
#include <string>

using namespace std;

struct TrieNode {
    unordered_map<char, TrieNode*> children;
    unordered_map<string, int> cityData;
    bool isEnd = false;
};

class Trie {
    private:
    TrieNode* root;
    public:
    Trie() : root(new TrieNode()) {}

    void insert(const string& cityName, const string& countryCode, int population) {
        TrieNode* node = root;
        for (char c : cityName) {
            if (!node->children[c]) {
                node->children[c] = new TrieNode();
            }
            node = node->children[c];
        }
        node->isEnd = true;
        node->cityData[countryCode] = population;
    }

    int search(const string& cityName, const string& countryCode) {
        TrieNode* node = root;
        for (char c : cityName) {
            if (!node->children[c]) {
                return -1;
            }
            node = node->children[c];
        }
        if (node->isEnd && node->cityData.count(countryCode)) {
            return node->cityData[countryCode];
        }
        return -1;
    }
};

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

class LFUCache : public Cache {
    private:
    unordered_map<string, pair<string, list<string>::iterator>> cacheMap;
    list<string> accessOrder;
    size_t capacity;
    public:
    LFUCache(size_t cap) : Cache(cap), capacity(cap) {
    }

    string get(const string& key) {
        if (cacheMap.find(key) != cacheMap.end()) {
            accessOrder.erase(cacheMap[key].second);
            cacheMap[key].second = accessOrder.begin();
            return cacheMap[key].first;
        }
        return "";
    }
    void put(const string& key, const string& value) {
        if (cacheMap.find(key) != cacheMap.end()) {
            accessOrder.erase(cacheMap[key].second);
            cacheMap[key].second = accessOrder.begin();
            cacheMap[key].first = value;
        } else if (cacheMap.size() >= capacity) {
            string lruKey = accessOrder.back();
            accessOrder.pop_back();
            cacheMap.erase(lruKey);
        }
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

class FIFOCache : public Cache {
    private:
    unordered_map<string, pair<string, list<string>::iterator>> cacheMap;
    list<string> accessOrder;
    size_t capacity;
    public:
    FIFOCache(size_t cap) : Cache(cap), capacity(cap) {}
    string get(const string& key) {
        if (cacheMap.find(key) != cacheMap.end()) {
            accessOrder.erase(cacheMap[key].second);
            cacheMap[key].second = accessOrder.begin();
            return cacheMap[key].first;
        }
        return "";
    }
    void put(const string& key, const string& value) {
        if (cacheMap.find(key) != cacheMap.end()) {
            accessOrder.erase(cacheMap[key].second);
            cacheMap[key].second = accessOrder.begin();
            cacheMap[key].first = value;
        }
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

class RandomCache : public Cache {
    private:
    unordered_map<string, pair<string, list<string>::iterator>> cacheMap;
    list<string> accessOrder;
    size_t capacity;
    mt19937 rng; // Mersenne Twister random number generator
    void evictRandomItem() {
        if (cacheMap.empty()) return;
        uniform_int_distribution<size_t> dist(0, cacheMap.size() - 1);

        auto it = cacheMap.begin();
        advance(it, dist(rng));

        accessOrder.erase(it->second.second);
        cacheMap.erase(it);
    }
    public:
    RandomCache(size_t cap) : Cache(cap), capacity(cap), rng(random_device{}()) {}
    string get(const string& key) {
        if (cacheMap.find(key) != cacheMap.end()) {
            accessOrder.erase(cacheMap[key].second);
            accessOrder.push_front(key);
            cacheMap[key].second = accessOrder.begin();
            return cacheMap[key].first;
        }
        return "";
    }
    void put(const string& key, const string& value) {
        if (cacheMap.find(key) != cacheMap.end()) {
            accessOrder.erase(cacheMap[key].second);
        } else if (cacheMap.size() >= capacity) {
            evictRandomItem();
        }
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

void loadCities(const string& fileName, Trie& trie) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Unable to open file " << fileName << "\n";
        return;
    }
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string countryCode, cityName, populationStr;

        if (lineNumber == 1 && line.find("country code,city name,population") != string::npos) {
            continue;
        }

        getline(ss, countryCode, ',');
        getline(ss, cityName, ',');
        getline(ss, populationStr, ',');

        int population = stoi(populationStr);
        trie.insert(cityName, countryCode, population);
    }
}