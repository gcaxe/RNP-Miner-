#pragma once

#include "common.h"

// PrefixSpan is self-contained (does not use Pdata)
class PrefixSpan {
public:
    PrefixSpan(const std::string& filename, int min_sup);

    void run();

    int get_fp_count() const { return fp_count_; }
    int get_can_num() const { return can_num_; }

private:
    // sdb[seq][itemset][item]
    std::vector<std::vector<std::vector<std::string>>> sdb_;
    int minsup_;
    int can_num_ = 0;
    int fp_count_ = 0;

    void read_file(const std::string& filename);

    std::vector<std::string> get_elem(
        const std::vector<std::vector<std::vector<std::string>>>& data_list);

    void delete_not_fre_elem(
        std::vector<std::vector<std::vector<std::string>>>& data,
        const std::vector<std::string>& not_fre_elem);

    std::vector<std::vector<std::vector<std::string>>> get_prefix_data(
        const std::string& e,
        const std::vector<std::vector<std::vector<std::string>>>& data);

    std::vector<std::vector<std::vector<std::vector<std::string>>>> get_all_prefix_data(
        const std::vector<std::string>& elem,
        const std::string& prefix_e,
        const std::vector<std::vector<std::vector<std::string>>>& data_list);

    std::pair<std::vector<std::string>, std::vector<std::string>> use_cycle_get_fre_elem(
        const std::vector<std::vector<std::vector<std::string>>>& data_list,
        const std::string& prefix_e,
        const std::vector<std::string>& elem);

    std::vector<std::vector<std::vector<std::string>>> use_cycle_get_prefix_data(
        const std::string& e,
        const std::string& prefix_e,
        const std::vector<std::vector<std::vector<std::string>>>& data);

    // Returns all frequent sequences for a given prefix-data
    // Represented as: vector of sequences, where each sequence is [[itemset1...], [itemset2...]]
    using Sequence = std::vector<std::vector<std::string>>;
    std::vector<Sequence> cycle_get_fre_elem(
        const std::vector<std::vector<std::vector<std::string>>>& pre_fix_data,
        const std::string& e);
};
