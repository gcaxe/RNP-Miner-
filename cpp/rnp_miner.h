#pragma once

#include "common.h"
#include "pdata.h"

class RNPMiner {
public:
    RNPMiner(int seq_num, const PosDict& pos_dict,
             const std::vector<std::string>& sorted_items, int min_sup);

    void run();

    int get_fp_count() const { return static_cast<int>(fp_.size()); }
    int get_can_num() const { return can_num_; }
    const std::vector<Pattern>& get_fp() const { return fp_; }

private:
    int seq_num_;
    PosDict s_;               // position dictionary (copied from input)
    std::vector<std::string> sort_item_;
    int minsup_;
    int can_num_ = 0;
    std::vector<Pattern> fp_; // frequent patterns

    // Returns (count, new_poslist)
    std::pair<int, PosList> matching_i(const PosList& list1, const PosList& list2);
    std::pair<int, PosList> matching_s(const PosList& list1, const PosList& list2);

    void join_i(std::vector<Pattern>& exp_set, PosDict& item_s);
    void gen_i(PosDict& item_s);
    void mine_item_s(PosDict& item_s);
    void join_s(std::vector<Pattern>& exp_set, PosDict& item_s);
    void mine_pattern(PosDict& item_s);
};
