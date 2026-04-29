#pragma once

#include "common.h"

class ProcessingData {
public:
    std::vector<std::string> read_file(const std::string& filename);
    void write_file(const std::vector<std::string>& lines_s, const std::string& filename);
    std::pair<std::vector<std::string>, std::vector<std::string>>
        item_sortd(const std::vector<std::string>& items);
    void item_to_dict(const std::vector<std::string>& sort_items,
                      int len_lines, PosDict& S);
    std::pair<std::vector<std::string>, std::vector<std::vector<std::string>>>
        split_array(const std::string& line);
    std::vector<std::string> statistics_items(const std::string& line,
                                              std::vector<std::string>& items);
    void general_sn(const std::vector<std::string>& lines_s, PosDict& S);

    // Returns (SeqNum, S, sort_item)
    std::tuple<int, PosDict, std::vector<std::string>>
        datap(const std::string& filename, PosDict& S);
};
