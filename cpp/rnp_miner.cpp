#include "rnp_miner.h"

RNPMiner::RNPMiner(int seq_num, const PosDict& pos_dict,
                   const std::vector<std::string>& sorted_items, int min_sup)
    : seq_num_(seq_num), s_(pos_dict), sort_item_(sorted_items), minsup_(min_sup) {}

std::pair<int, PosList> RNPMiner::matching_i(const PosList& list1, const PosList& list2) {
    PosList list3(seq_num_);
    int count = 0;
    for (int i = 0; i < seq_num_; ++i) {
        list3[i] = intersect_sorted(list1[i], list2[i]);
        count += static_cast<int>(list3[i].size());
    }
    return {count, list3};
}

std::pair<int, PosList> RNPMiner::matching_s(const PosList& list1, const PosList& list2) {
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

void RNPMiner::join_i(std::vector<Pattern>& exp_set, PosDict& item_s) {
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
                    Pattern qq = {q.back()};
                    auto [count, poslist] = matching_i(
                        item_s[pattern_to_key(p)],
                        item_s[pattern_to_key(qq)]);
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

void RNPMiner::gen_i(PosDict& item_s) {
    std::vector<Pattern> item = fp_;  // copy current size-1 patterns
    std::vector<Pattern> exp_set;

    for (size_t i = 0; i < item.size(); ++i) {
        for (size_t j = i + 1; j < item.size(); ++j) {
            const auto& pre = item[i];
            const auto& suf = item[j];
            Pattern p = {pre[0], suf[0]};
            ++can_num_;
            auto [count, poslist] = matching_i(
                item_s[pattern_to_key(pre)],
                item_s[pattern_to_key(suf)]);
            if (count >= minsup_) {
                fp_.push_back(p);
                exp_set.push_back(p);
                item_s[pattern_to_key(p)] = poslist;
            }
        }
    }
    join_i(exp_set, item_s);
}

void RNPMiner::mine_item_s(PosDict& item_s) {
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

void RNPMiner::join_s(std::vector<Pattern>& exp_set, PosDict& item_s) {
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
                    auto [count, poslist] = matching_s(
                        item_s[pattern_to_key(p)],
                        item_s[pattern_to_key({q.back()})]);
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

void RNPMiner::mine_pattern(PosDict& item_s) {
    std::vector<Pattern> temp = fp_;  // copy size-1 patterns
    std::vector<Pattern> exp_set;

    for (const auto& pre : temp) {
        for (const auto& suf : temp) {
            Pattern pattern = {pre[0], suf[0]};
            // Store as nested pattern: pattern = [pre, suf]
            Pattern nested_pattern;
            nested_pattern.push_back(pre[0]);
            nested_pattern.push_back(suf[0]);

            ++can_num_;
            auto [count, poslist] = matching_s(
                item_s[pattern_to_key(pre)],
                item_s[pattern_to_key(suf)]);
            if (count >= minsup_) {
                // RNP-Miner stores patterns as flat lists after the size-2 step
                Pattern flat = pre;
                flat.push_back(suf[0]);
                fp_.push_back(flat);
                exp_set.push_back(flat);
                item_s[pattern_to_key(flat)] = poslist;
            }
        }
    }
    join_s(exp_set, item_s);
}

void RNPMiner::run() {
    PosDict item_s;
    mine_item_s(item_s);
    mine_pattern(item_s);
}
