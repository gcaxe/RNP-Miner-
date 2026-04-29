#include "rnp_d.h"
#include <sstream>

static std::vector<std::string> split_str_d(const std::string& s) {
    std::vector<std::string> result;
    std::istringstream iss(s);
    std::string tok;
    while (iss >> tok) result.push_back(tok);
    return result;
}

RNPD::RNPD(int seq_num, const PosDict& pos_dict,
           const std::vector<std::string>& sorted_items, int min_sup)
    : seq_num_(seq_num), s_(pos_dict), sort_item_(sorted_items), minsup_(min_sup) {}

void RNPD::deep_gen(const std::string& fp_str) {
    for (const auto& item : fp1_) {
        // --- S-connection ---
        std::string pattern = fp_str + " -1 " + item;
        ++can_num_;
        int count = 0;
        lp_[pattern] = PosList(seq_num_);
        lp_[pattern] = p_[fp_str];
        p_[pattern] = PosList(seq_num_);

        for (int i = 0; i < seq_num_; ++i) {
            int flag = 0;
            for (size_t j = 0; j < p_[fp_str][i].size(); ++j) {
                if (flag == static_cast<int>(s_[item][i].size())) break;
                for (int k = flag; k < static_cast<int>(s_[item][i].size()); ++k) {
                    if (s_[item][i][k] > p_[fp_str][i][j]) {
                        p_[pattern][i].push_back(s_[item][i][k]);
                        ++count;
                        flag = k + 1;
                        break;
                    }
                    if (k == static_cast<int>(s_[item][i].size()) - 1) {
                        flag = static_cast<int>(s_[item][i].size());
                    }
                }
            }
        }
        if (count >= minsup_) {
            fp_.push_back(pattern);
            deep_gen(pattern);
        } else {
            p_.erase(pattern);
            lp_.erase(pattern);
        }

        // --- I-connection ---
        auto fp_parts = split_str_d(fp_str);
        if (!fp_parts.empty() && fp_parts.back() < item) {
            std::string i_pattern = fp_str + " " + item;
            ++can_num_;
            count = 0;
            lp_[i_pattern] = PosList(seq_num_);
            lp_[i_pattern] = lp_[fp_str];
            p_[i_pattern] = PosList(seq_num_);

            PosList unit = s_[item];
            int f = -1;
            for (size_t i = 0; i < fp_parts.size(); ++i) {
                if (fp_parts[i] == "-1") f = static_cast<int>(i);
            }
            for (int i = f + 1; i < static_cast<int>(fp_parts.size()); ++i) {
                for (int j = 0; j < seq_num_; ++j) {
                    unit[j] = intersect_sorted(unit[j], s_[fp_parts[i]][j]);
                }
            }

            for (int i = 0; i < seq_num_; ++i) {
                int flag = 0;
                for (size_t j = 0; j < lp_[fp_str][i].size(); ++j) {
                    if (flag == static_cast<int>(unit[i].size())) break;
                    for (int k = flag; k < static_cast<int>(unit[i].size()); ++k) {
                        if (unit[i][k] > lp_[fp_str][i][j]) {
                            p_[i_pattern][i].push_back(unit[i][k]);
                            ++count;
                            flag = k + 1;
                            break;
                        }
                        if (k == static_cast<int>(unit[i].size()) - 1) {
                            flag = static_cast<int>(unit[i].size());
                        }
                    }
                }
            }
            if (count >= minsup_) {
                fp_.push_back(i_pattern);
                deep_gen(i_pattern);
            } else {
                p_.erase(i_pattern);
                lp_.erase(i_pattern);
            }
        }
    }
    p_.erase(fp_str);
    lp_.erase(fp_str);
}

void RNPD::run() {
    // Mine size-1 frequent patterns
    int count = 0;
    for (const auto& item_name : sort_item_) {
        ++can_num_;
        for (int j = 0; j < seq_num_; ++j) {
            count += static_cast<int>(s_[item_name][j].size());
        }
        if (count >= minsup_) {
            fp1_.push_back(item_name);
            fp_.push_back(item_name);
            p_[item_name] = PosList(seq_num_);
            p_[item_name] = s_[item_name];
            lp_[item_name] = PosList(seq_num_);
            for (int k = 0; k < seq_num_; ++k) {
                for (size_t t = 0; t < p_[item_name][k].size(); ++t) {
                    lp_[item_name][k].push_back(p_[item_name][k][t] - 1);
                }
            }
        }
        count = 0;
    }

    for (const auto& fp_str : fp1_) {
        deep_gen(fp_str);
    }

    std::cout << "Number of frequent patterns: " << fp_.size() << std::endl;
    std::cout << "Number of candidate patterns: " << can_num_ << std::endl;
}
