#pragma once

#include "common.h"

// NOSEP-RNP is self-contained (does not use Pdata)
class NOSEPRNP {
public:
    NOSEPRNP(const std::string& filename, int min_sup);

    void run();

    int get_fp_count() const { return static_cast<int>(fp_.size()); }
    int get_can_num() const { return can_num_; }

private:
    // sdb[seq][itemset][item]
    std::vector<std::vector<std::vector<std::string>>> sdb_;
    std::vector<std::string> sort_item_;
    int seq_num_;
    int minsup_;
    int can_num_ = 0;
    std::vector<Pattern> fp_;

    // Nettree node
    struct NTNode {
        int position = 0;
        std::vector<NTNode*> snode;
        std::vector<NTNode*> pnode;
    };

    // Nettree: [level][node_list]
    using Nettree = std::vector<std::vector<NTNode*>>;

    void read_file(const std::string& filename);
    PosList get_index(const std::string& item);
    bool if_exist(const Pattern& items, const std::vector<std::string>& item_set);
    void create_nettree(Nettree& nettree, const Pattern& pattern,
                        const std::vector<std::vector<std::string>>& seq);
    void update_nettree(Nettree& nettree);
    void cut_tree(NTNode* node, Nettree& nettree, int levl);
    int net_gap(const Pattern& pattern);

    std::pair<int, PosList> get_unit(const Pattern& itemsets);
    std::pair<int, PosList> pro_matching(const PosList& list1, const Pattern& itemsets);

    void join_i(std::vector<Pattern>& exp_set, PosDict& item_s);
    void gen_i(PosDict& item_s);
    void mine_item_s(PosDict& item_s);
    void join_s(std::vector<Pattern>& exp_set, PosDict& item_s);
    void mine_pattern(PosDict& item_s);
};
