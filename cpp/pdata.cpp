#include "pdata.h"

std::vector<std::string> ProcessingData::read_file(const std::string& filename) {
    return read_lines(filename);
}

void ProcessingData::write_file(const std::vector<std::string>& lines_s,
                                 const std::string& filename) {
    std::ofstream f(filename);
    for (size_t i = 0; i < lines_s.size(); ++i) {
        f << i << "\t" << lines_s[i] << "\n";
    }
}

std::pair<std::vector<std::string>, std::vector<std::string>>
ProcessingData::item_sortd(const std::vector<std::string>& items) {
    std::vector<std::string> items_no_repeat;
    for (const auto& item : items) {
        if (std::find(items_no_repeat.begin(), items_no_repeat.end(), item)
            == items_no_repeat.end()) {
            items_no_repeat.push_back(item);
        }
    }
    std::vector<std::string> sort_items = items_no_repeat;
    std::sort(sort_items.begin(), sort_items.end());
    return {sort_items, items_no_repeat};
}

void ProcessingData::item_to_dict(const std::vector<std::string>& sort_items,
                                   int len_lines, PosDict& S) {
    for (const auto& item : sort_items) {
        S[item] = PosList(len_lines);
    }
}

std::pair<std::vector<std::string>, std::vector<std::vector<std::string>>>
ProcessingData::split_array(const std::string& line) {
    std::string cleaned = line;
    // Replace double spaces with single space
    size_t pos;
    while ((pos = cleaned.find("  ")) != std::string::npos) {
        cleaned.replace(pos, 2, " ");
    }

    // Split by space
    std::vector<std::string> items_array;
    std::istringstream iss(cleaned);
    std::string token;
    while (iss >> token) {
        items_array.push_back(token);
    }

    // Build 2D itemset array
    std::vector<std::vector<std::string>> s_array;
    s_array.push_back({});
    for (const auto& item : items_array) {
        if (item != "-1") {
            s_array.back().push_back(item);
        } else {
            s_array.push_back({});
        }
    }

    return {items_array, s_array};
}

std::vector<std::string> ProcessingData::statistics_items(
    const std::string& line, std::vector<std::string>& items) {
    std::string cleaned = line;
    size_t pos;
    while ((pos = cleaned.find("  ")) != std::string::npos) {
        cleaned.replace(pos, 2, " ");
    }
    std::istringstream iss(cleaned);
    std::string token;
    while (iss >> token) {
        if (token != "-1" &&
            std::find(items.begin(), items.end(), token) == items.end()) {
            items.push_back(token);
        }
    }
    return items;
}

void ProcessingData::general_sn(const std::vector<std::string>& lines_s,
                                 PosDict& S) {
    for (size_t i = 0; i < lines_s.size(); ++i) {
        auto [items_array, s_array] = split_array(lines_s[i]);
        auto [sort_item, items_no_repeat] = item_sortd(items_array);

        for (const auto& item : sort_item) {
            if (item != "-1") {
                for (size_t k = 0; k < s_array.size(); ++k) {
                    if (std::find(s_array[k].begin(), s_array[k].end(), item)
                        != s_array[k].end()) {
                        S[item][i].push_back(static_cast<int>(k));
                    }
                }
            }
        }
    }
}

std::tuple<int, PosDict, std::vector<std::string>>
ProcessingData::datap(const std::string& filename, PosDict& S) {
    auto lines_s = read_file(filename);

    std::vector<std::string> items;
    for (const auto& line : lines_s) {
        statistics_items(line, items);
    }

    auto [sort_item, items_no_repeat] = item_sortd(items);
    write_file(sort_item, "sort_item.txt");

    item_to_dict(sort_item, static_cast<int>(lines_s.size()), S);
    general_sn(lines_s, S);

    return {static_cast<int>(lines_s.size()), S, sort_item};
}
