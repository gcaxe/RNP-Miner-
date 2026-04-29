#pragma once

#include "common.h"
#include "pdata.h"

class RNPB {
public:
    RNPB(int seq_num, const PosDict& pos_dict,
         const std::vector<std::string>& sorted_items, int min_sup);

    void run();

    int get_fp_count() const { return static_cast<int>(fp_.size()); }
    int get_can_num() const { return can_num_; }

private:
    int seq_num_;
    PosDict s_;
    std::vector<std::string> sort_item_;
    std::vector<std::string> fp1_;  // size-1 frequent patterns
    std::vector<std::string> fp_;   // all frequent patterns (as string repr)
    PosDict p_;    // occurrence positions
    PosDict lp_;   // itemset-sub-pattern positions (shifted by -1)
    int minsup_;
    int can_num_ = 0;
};
