#include <iostream>
#include <algorithm>
#include <map>
#include <utility>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>
#include <stack>

bool pairs_comparator(const std::pair<std::string, int>& a,
                      const std::pair<std::string, int>& b) {
    if (a.second != b.second) {
        return a.second > b.second;
    }
    return a.first < b.first;
}

void sort_with_pairs_comparator(std::vector<std::pair<std::string, int>>& vec) {
    std::sort(vec.begin(), vec.end(), pairs_comparator);
}

bool strings_comparator(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
    return a.first > b.first;
}

void sort_with_strings_comparator(std::vector<std::pair<std::string, int>>& vec) {
    std::sort(vec.begin(), vec.end(), strings_comparator);
}

std::map<std::string, int> parseDictionary(const std::string& fileName) {
    std::ifstream in(fileName);
    if (!in.is_open()) {
        throw std::runtime_error("Файл не открыт");
    }
    std::map<std::string, int> dict;
    std::string line;
    while (std::getline(in, line)) {
        std::string key;
        int value;
        size_t delimiter = line.find(',');
        if (delimiter != std::string::npos) {
            key = line.substr(0, delimiter);
            value = std::stoi(line.substr(delimiter + 1));
            dict[key] = value;
        }
    }
    return dict;
}


std::vector<std::string> parseWords(const std::string& fileName) {
    std::ifstream in(fileName);
    if (!in.is_open()) {
        throw std::runtime_error("Файл не открыт");
    }
    std::vector<std::string> words;
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string word;
        while (iss >> word) {
            if (!word.empty()) {
                words.push_back(word);
            }
        }
    }
    return words;
}


std::vector<std::pair<std::string, int>> getAllAnagrams(const std::string& word, const std::map<std::string, int>& dict){
    std::vector<std::pair<std::string, int>> result;

    std::string sortedWord = word;
    std::sort(sortedWord.begin(), sortedWord.end(), std::greater<>());

    do {
        auto it = dict.find(sortedWord);
        if (it != dict.cend()) {
            result.emplace_back( it->first, it->second);
        }
    } while (std::prev_permutation(sortedWord.begin(), sortedWord.end()));

    return result;
}


std::vector<std::vector<std::pair<std::string, int>>> getAnagramTable(const std::string& dictFile, const std::string& msgFile){
    std::map<std::string, int> dict = parseDictionary(dictFile);
    std::vector<std::string> words = parseWords(msgFile);
    std::vector<std::vector<std::pair<std::string, int>>> table;
    for (const auto& word : words){
        std::vector<std::pair<std::string, int>> anagramsWord = getAllAnagrams(word, dict);
        if (anagramsWord.empty()) {
            throw std::runtime_error("Could not find anagrams for word: " + word);
        }
        table.push_back(anagramsWord);
    }
    return table;
}


void generateAllAnagramCombinations(const std::vector<std::vector<std::pair<std::string, int>>>& input,
                                    std::vector<std::vector<std::pair<std::string, int>>>& allCombinations){
    std::stack<std::vector<std::pair<std::string, int>>> currentStack;
    currentStack.emplace();
    while (!currentStack.empty()){
        std::vector<std::pair<std::string, int>> currentCombination = currentStack.top();
        currentStack.pop();
        size_t currentIndex = currentCombination.size();
        if (currentIndex == input.size()){
            allCombinations.push_back(currentCombination);
        }
        else{
            const std::vector<std::pair<std::string, int>>& currentVector = input[currentIndex];
            for (const std::pair<std::string, int>& word : currentVector){
                std::vector<std::pair<std::string, int>> newCombination = currentCombination;
                newCombination.push_back(word);
                currentStack.push(newCombination);
            }
        }
    }
}


void print_table(const std::vector<std::vector<std::pair<std::string, int>>>& table){
    std::vector<std::vector<std::pair<std::string, int>>> allCombinations;
    generateAllAnagramCombinations(table, allCombinations);
    std::vector<std::pair<std::string, int>> tempVector1;
    for (const std::vector<std::pair<std::string, int>>& combination : allCombinations){
        std::vector<std::pair<std::string, int>> tempVector;
        for (const std::pair<std::string, int>& word : combination){
            if (word.first != "\n" && word.first != " " && word.second != 0){
                tempVector.emplace_back(word.first, word.second);
            }
        }
        sort_with_strings_comparator(tempVector);
        do{
            std::string strTemp;
            int count = 0;
            for (const auto& it : tempVector) {
                count += it.second;
                strTemp += it.first + ' ';
            }
            tempVector1.emplace_back(strTemp, count);
        } while (std::prev_permutation(tempVector.begin(), tempVector.end()));
    }
    sort_with_pairs_comparator(tempVector1);
    for (auto &it : tempVector1){
        std::cout << it.first + ' ' << "|" << " weight = "<< it.second << std::endl;
    }
}

int main(int argc, char** argv){
    if (argc != 3){
        std::cerr << "Error";
        return 1;
    }
    std::vector<std::string> words = parseWords(argv[1]);
    std::vector<std::vector<std::pair<std::string, int>>> res = getAnagramTable(argv[1], argv[2]);
    print_table(res);
    return 0;
}