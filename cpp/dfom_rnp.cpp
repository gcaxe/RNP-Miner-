#include "dfom_rnp.h"

DFOMRNP::DFOMRNP(int seq_num, const PosDict& pos_dict,
                 const std::vector<std::string>& sorted_items, int min_sup)
    : seq_num_(seq_num), s_(pos_dict), sort_item_(sorted_items), minsup_(min_sup) {}

std::pair<int, PosList> DFOMRNP::matching_i(const PosList& list1, const PosList& list2) {
    PosList list3(seq_num_);
    int count = 0;
    for (int i = 0; i < seq_num_; ++i) {
        list3[i] = intersect_sorted(list1[i], list2[i]);
        count += static_cast<int>(list3[i].size());
    }
    return {count, list3};
}

int DFOMRNP::dfom(const Pattern& pattern, const PosDict& item_s) {
    int count = 0;
    int plen = static_cast<int>(pattern.size());

    // Nettree: [layer][seq] = positions
    std::vector<std::vector<std::vector<int>>> nettree(
        plen, std::vector<std::vector<int>>(seq_num_));

    // unit[i] = positions for pattern[i]
    std::vector<PosList> unit(plen);
    for (int i = 0; i < plen; ++i) {
        Pattern p = {pattern[i]};
        unit[i] = item_s.at(pattern_to_key(p));
    }

    for (int i = 0; i < seq_num_; ++i) {
        for (size_t m = 0; m < unit[0][i].size(); ++m) {
            int bbb = 0;
            nettree[0][i].push_back(unit[0][i][m]);

            for (int j = 1; j < plen; ++j) {
                int n = 1;
                if (unit[j][i].empty()) {
                    bbb = 1;
                    break;
                } else {
                    for (size_t k = 0; k < unit[j][i].size(); ++k) {
                        int aaa;
                        if (nettree[j][i].empty()) {
                            aaa = -1;
                        } else {
                            aaa = nettree[j][i].back();
                        }
                        if (unit[j][i][k] > nettree[j - 1][i].back() &&
                            unit[j][i][k] > aaa) {
                            nettree[j][i].push_back(unit[j][i][k]);
                            if (j == plen - 1) {
                                ++count;
                            }
                            n = 0;
                            break;
                        }
                    }
                    if (n) {
                        bbb = 1;
                        break;
                    }
                }
            }
            if (bbb) break;
        }
    }
    return count;
}

void DFOMRNP::join_i(std::vector<Pattern>& exp_set, PosDict& item_s) {
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

void DFOMRNP::gen_i(PosDict& item_s) {
    std::vector<Pattern> item = fp_;
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

void DFOMRNP::mine_item_s(PosDict& item_s) {
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

void DFOMRNP::join_s(std::vector<Pattern>& exp_set, PosDict& item_s) {
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
                    int count = dfom(pattern, item_s);
                    if (count >= minsup_) {
                        fp_.push_back(pattern);
                        temp.push_back(pattern);
                    }
                }
            }
        }
        exp_set = temp;
    }
}

void DFOMRNP::mine_pattern(PosDict& item_s) {
    std::vector<Pattern> temp = fp_;
    std::vector<Pattern> exp_set;

    for (const auto& pre : temp) {
        for (const auto& suf : temp) {
            Pattern pattern = pre;
            pattern.insert(pattern.end(), suf.begin(), suf.end());
            ++can_num_;
            int count = dfom(pattern, item_s);
            if (count >= minsup_) {
                fp_.push_back(pattern);
                exp_set.push_back(pattern);
            }
        }
    }
    join_s(exp_set, item_s);
}

void DFOMRNP::run() {
    PosDict item_s;
    mine_item_s(item_s);
    mine_pattern(item_s);
}
