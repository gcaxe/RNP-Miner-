#include "pro_rnp.h"

ProRNP::ProRNP(int seq_num, const PosDict& pos_dict,
               const std::vector<std::string>& sorted_items, int min_sup)
    : seq_num_(seq_num), s_(pos_dict), sort_item_(sorted_items), minsup_(min_sup) {}

std::pair<int, PosList> ProRNP::get_unit(const Pattern& itemsets) {
    int count = 0;
    PosList unit = s_[itemsets[0]];
    for (size_t i = 1; i < itemsets.size(); ++i) {
        for (int j = 0; j < seq_num_; ++j) {
            unit[j] = intersect_sorted(unit[j], s_[itemsets[i]][j]);
        }
    }
    for (int i = 0; i < seq_num_; ++i) {
        count += static_cast<int>(unit[i].size());
    }
    return {count, unit};
}

std::pair<int, PosList> ProRNP::pro_matching(const PosList& list1,
                                               const Pattern& itemsets) {
    auto [a, list2] = get_unit(itemsets);
    PosList list3(seq_num_);
    int count = 0;

    for (int i = 0; i < seq_num_; ++i) {
        int flag = 0;
        for (size_t j = 0; j < list1[i].size(); ++j) {
            if (flag == static_cast<int>(list2[i].size())) break;
            for (int k = flag; k < static_cast<int>(list2[i].size()); ++k) {
                if (list2[i][k] > list1[i][j]) {
                    list3[i].push_back(list2[i][k]);
                    ++count;
                    flag = k + 1;
                    break;
                }
                if (k == static_cast<int>(list2[i].size()) - 1) {
                    flag = static_cast<int>(list2[i].size());
                }
            }
        }
    }
    return {count, list3};
}

void ProRNP::join_i(std::vector<Pattern>& exp_set, PosDict& item_s) {
    while (!exp_set.empty()) {
        std::vector<Pattern> temp;
        for (const auto& p : exp_set) {
            Pattern suf_p(p.begin() + 1, p.end());
            for (const auto& q : exp_set) {
                Pattern pre_q(q.begin(), q.end() - 1);
                if (suf_p == pre_q) {
                    Pattern pattern = p;
                    pattern.push_back(q.back());
                    ++can_num_;
                    auto [count, poslist] = get_unit(pattern);
                    if (count >= minsup_) {
                        fp_.push_back(pattern);
                        temp.push_back(pattern);
                        item_s[pattern_to_key(pattern)] = poslist;
                    }
                }
            }
        }
        exp_set = temp;
    }
}

void ProRNP::gen_i(PosDict& item_s) {
    std::vector<Pattern> item = fp_;
    std::vector<Pattern> exp_set;

    for (size_t i = 0; i < item.size(); ++i) {
        for (size_t j = i + 1; j < item.size(); ++j) {
            const auto& pre = item[i];
            const auto& suf = item[j];
            Pattern p = {pre[0], suf[0]};
            ++can_num_;
            auto [count, poslist] = get_unit(p);
            if (count >= minsup_) {
                fp_.push_back(p);
                exp_set.push_back(p);
                item_s[pattern_to_key(p)] = poslist;
            }
        }
    }
    join_i(exp_set, item_s);
}

void ProRNP::mine_item_s(PosDict& item_s) {
    for (const auto& item_name : sort_item_) {
        ++can_num_;
        int count = 0;
        for (int j = 0; j < seq_num_; ++j) {
            count += static_cast<int>(s_[item_name][j].size());
        }
        if (count >= minsup_) {
            Pattern p = {item_name};
            fp_.push_back(p);
            item_s[pattern_to_key(p)] = PosList(seq_num_);
            item_s[pattern_to_key(p)] = s_[item_name];
        }
    }
    gen_i(item_s);
}

void ProRNP::join_s(std::vector<Pattern>& exp_set, PosDict& item_s) {
    while (!exp_set.empty()) {
        std::vector<Pattern> temp;
        for (const auto& p : exp_set) {
            for (const auto& q : exp_set) {
                Pattern suf_p(p.begin() + 1, p.end());
                Pattern pre_q(q.begin(), q.end() - 1);
                if (suf_p == pre_q) {
                    Pattern pattern = p;
                    pattern.push_back(q.back());
                    ++can_num_;
                    Pattern last_item = {q.back()};
                    auto [count, poslist] = pro_matching(
                        item_s[pattern_to_key(p)], last_item);
                    if (count >= minsup_) {
                        fp_.push_back(pattern);
                        temp.push_back(pattern);
                        item_s[pattern_to_key(pattern)] = poslist;
                    }
                }
            }
        }
        exp_set = temp;
    }
}

void ProRNP::mine_pattern(PosDict& item_s) {
    std::vector<Pattern> temp = fp_;
    std::vector<Pattern> exp_set;

    for (const auto& pre : temp) {
        for (const auto& suf : temp) {
            Pattern pattern = pre;
            pattern.insert(pattern.end(), suf.begin(), suf.end());
            ++can_num_;
            auto [count, poslist] = pro_matching(
                item_s[pattern_to_key(pre)], suf);
            if (count >= minsup_) {
                fp_.push_back(pattern);
                exp_set.push_back(pattern);
                item_s[pattern_to_key(pattern)] = poslist;
            }
        }
    }
    join_s(exp_set, item_s);
}

void ProRNP::run() {
    PosDict item_s;
    mine_item_s(item_s);
    mine_pattern(item_s);
}
