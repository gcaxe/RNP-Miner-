#pragma once

#include "common.h"
#include "pdata.h"

class DFOMRNP {
public:
    DFOMRNP(int seq_num, const PosDict& pos_dict,
            const std::vector<std::string>& sorted_items, int min_sup);

    void run();

    int get_fp_count() const { return static_cast<int>(fp_.size()); }
    int get_can_num() const { return can_num_; }

private:
    int seq_num_;
    PosDict s_;
    std::vector<std::string> sort_item_;
    int minsup_;
    int can_num_ = 0;
    std::vector<Pattern> fp_;

    std::pair<int, PosList> matching_i(const PosList& list1, const PosList& list2);
    int dfom(const Pattern& pattern, const PosDict& item_s);

    void join_i(std::vector<Pattern>& exp_set, PosDict& item_s);
    void gen_i(PosDict& item_s);
    void mine_item_s(PosDict& item_s);
    void join_s(std::vector<Pattern>& exp_set, PosDict& item_s);
    void mine_pattern(PosDict& item_s);
};
