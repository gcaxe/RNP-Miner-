#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <set>
#include <cmath>

// Position list per sequence: for each of SeqNum sequences, a list of positions
using PosList = std::vector<std::vector<int>>;

// Position dictionary: pattern_key -> PosList
using PosDict = std::unordered_map<std::string, PosList>;

// A pattern is a list of item names (strings)
using Pattern = std::vector<std::string>;

// Convert a Pattern to its string key representation
// Python str(['a', 'b']) -> "['a', 'b']"
inline std::string pattern_to_key(const Pattern& p) {
    std::string result = "[";
    for (size_t i = 0; i < p.size(); ++i) {
        if (i > 0) result += ", ";
        result += "'" + p[i] + "'";
    }
    result += "]";
    return result;
}

// Convert a Pattern to a display string like "a b" or "a -1 b"
inline std::string pattern_to_str(const Pattern& p) {
    std::string result;
    for (size_t i = 0; i < p.size(); ++i) {
        if (i > 0) result += " ";
        result += p[i];
    }
    return result;
}

// Read all lines from a file
inline std::vector<std::string> read_lines(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream f(filename);
    if (!f.is_open()) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return lines;
    }
    std::string line;
    while (std::getline(f, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push_back(line);
    }
    return lines;
}

// Get current time in milliseconds
inline long long now_ms() {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
}

// Create an empty PosList with num_seq sequences
inline PosList make_poslist(int num_seq) {
    return PosList(num_seq);
}

// Copy a PosList
inline PosList copy_poslist(const PosList& src) {
    PosList result(src.size());
    for (size_t i = 0; i < src.size(); ++i) {
        result[i] = src[i];  // copy the vector<int>
    }
    return result;
}

// Intersection of two position vectors (set intersection)
inline std::vector<int> intersect_sorted(const std::vector<int>& a, const std::vector<int>& b) {
    std::vector<int> result;
    size_t i = 0, j = 0;
    while (i < a.size() && j < b.size()) {
        if (a[i] < b[j]) {
            ++i;
        } else if (b[j] < a[i]) {
            ++j;
        } else {
            if (result.empty() || result.back() != a[i]) {
                result.push_back(a[i]);
            }
            ++i; ++j;
        }
    }
    return result;
}
