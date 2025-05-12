#include <iostream>
#include <unordered_map>
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
        node->cityData[cityName] = population;
    }
}