#include "nosep_rnp.h"
#include <cstring>

NOSEPRNP::NOSEPRNP(const std::string& filename, int min_sup)
    : minsup_(min_sup) {
    read_file(filename);
}

void NOSEPRNP::read_file(const std::string& filename) {
    auto lines = read_lines(filename);
    seq_num_ = 0;

    for (const auto& line : lines) {
        ++seq_num_;
        std::vector<std::string> itemset;
        std::vector<std::vector<std::string>> seq;

        std::istringstream iss(line);
        std::string token;
        while (iss >> token) {
            if (token != "-1") {
                itemset.push_back(token);
                if (std::find(sort_item_.begin(), sort_item_.end(), token)
                    == sort_item_.end()) {
                    sort_item_.push_back(token);
                }
            } else {
                if (!itemset.empty()) {
                    seq.push_back(itemset);
                    itemset.clear();
                }
            }
        }
        if (!itemset.empty()) {
            seq.push_back(itemset);
        }
        sdb_.push_back(seq);
    }
    std::sort(sort_item_.begin(), sort_item_.end());
}

PosList NOSEPRNP::get_index(const std::string& item) {
    PosList index2;
    for (const auto& seq : sdb_) {
        std::vector<int> index;
        for (size_t i = 0; i < seq.size(); ++i) {
            if (std::find(seq[i].begin(), seq[i].end(), item) != seq[i].end()) {
                index.push_back(static_cast<int>(i));
            }
        }
        index2.push_back(index);
    }
    return index2;
}

bool NOSEPRNP::if_exist(const Pattern& items, const std::vector<std::string>& item_set) {
    for (const auto& it : items) {
        if (std::find(item_set.begin(), item_set.end(), it) == item_set.end()) {
            return false;
        }
    }
    return true;
}

void NOSEPRNP::create_nettree(Nettree& nettree, const Pattern& pattern,
                               const std::vector<std::vector<std::string>>& seq) {
    for (size_t j = 0; j < pattern.size(); ++j) {
        for (size_t i = 0; i < seq.size(); ++i) {
            Pattern pj = {pattern[j]};
            if (if_exist(pj, seq[i])) {
                NTNode* node = new NTNode();
                node->position = static_cast<int>(i);
                nettree[j].push_back(node);
            }
        }
    }
    for (size_t i = 0; i + 1 < nettree.size(); ++i) {
        int k = 0;
        while (k < static_cast<int>(nettree[i + 1].size())) {
            for (auto* j : nettree[i]) {
                if (j->position < nettree[i + 1][k]->position) {
                    j->snode.push_back(nettree[i + 1][k]);
                    nettree[i + 1][k]->pnode.push_back(j);
                }
            }
            if (nettree[i + 1][k]->pnode.empty()) {
                delete nettree[i + 1][k];
                nettree[i + 1].erase(nettree[i + 1].begin() + k);
            } else {
                ++k;
            }
        }
    }
}

void NOSEPRNP::update_nettree(Nettree& nettree) {
    int i = static_cast<int>(nettree.size()) - 2;
    while (i >= 0) {
        int j = 0;
        while (j < static_cast<int>(nettree[i].size())) {
            if (nettree[i][j]->snode.empty()) {
                for (auto* k : nettree[i][j]->pnode) {
                    auto& sn = k->snode;
                    sn.erase(std::remove(sn.begin(), sn.end(), nettree[i][j]), sn.end());
                }
                delete nettree[i][j];
                nettree[i].erase(nettree[i].begin() + j);
            } else {
                ++j;
            }
        }
        --i;
    }
}

void NOSEPRNP::cut_tree(NTNode* node, Nettree& nettree, int levl) {
    for (auto* child : node->snode) {
        auto& pn = child->pnode;
        pn.erase(std::remove(pn.begin(), pn.end(), node), pn.end());
    }
    for (auto* parent : node->pnode) {
        auto& sn = parent->snode;
        sn.erase(std::remove(sn.begin(), sn.end(), node), sn.end());
    }
    if (!node->snode.empty()) {
        cut_tree(node->snode[0], nettree, levl + 1);
    }
    auto& level_nodes = nettree[levl];
    level_nodes.erase(std::remove(level_nodes.begin(), level_nodes.end(), node),
                      level_nodes.end());
    delete node;
}

int NOSEPRNP::net_gap(const Pattern& pattern) {
    int count = 0;
    for (int i = 0; i < seq_num_; ++i) {
        Nettree nettree(pattern.size());
        create_nettree(nettree, pattern, sdb_[i]);
        update_nettree(nettree);

        while (!nettree[0].empty()) {
            ++count;
            cut_tree(nettree[0][0], nettree, 0);
            update_nettree(nettree);
        }
    }
    return count;
}

std::pair<int, PosList> NOSEPRNP::get_unit(const Pattern& itemsets) {
    int count = 0;
    PosList unit = get_index(itemsets[0]);
    for (size_t i = 1; i < itemsets.size(); ++i) {
        PosList index_i = get_index(itemsets[i]);
        for (int j = 0; j < seq_num_; ++j) {
            unit[j] = intersect_sorted(unit[j], index_i[j]);
        }
    }
    for (int i = 0; i < seq_num_; ++i) {
        count += static_cast<int>(unit[i].size());
    }
    return {count, unit};
}

std::pair<int, PosList> NOSEPRNP::pro_matching(const PosList& list1,
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

void NOSEPRNP::join_i(std::vector<Pattern>& exp_set, PosDict& item_s) {
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

void NOSEPRNP::gen_i(PosDict& item_s) {
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

void NOSEPRNP::mine_item_s(PosDict& item_s) {
    for (const auto& item_name : sort_item_) {
        ++can_num_;
        PosList index_i = get_index(item_name);
        int count = 0;
        for (int j = 0; j < seq_num_; ++j) {
            count += static_cast<int>(index_i[j].size());
        }
        if (count >= minsup_) {
            Pattern p = {item_name};
            fp_.push_back(p);
            item_s[pattern_to_key(p)] = PosList(seq_num_);
            item_s[pattern_to_key(p)] = index_i;
        }
    }
    gen_i(item_s);
}

void NOSEPRNP::join_s(std::vector<Pattern>& exp_set, PosDict& item_s) {
    (void)item_s;  // not used in NOSEP join_s (uses net_gap)
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
                    int count = net_gap(pattern);
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

void NOSEPRNP::mine_pattern(PosDict& item_s) {
    std::vector<Pattern> temp = fp_;
    std::vector<Pattern> exp_set;

    for (const auto& pre : temp) {
        for (const auto& suf : temp) {
            Pattern pattern = pre;
            pattern.insert(pattern.end(), suf.begin(), suf.end());
            ++can_num_;
            int count = net_gap(pattern);
            if (count >= minsup_) {
                fp_.push_back(pattern);
                exp_set.push_back(pattern);
            }
        }
    }
    join_s(exp_set, item_s);
}

void NOSEPRNP::run() {
    PosDict item_s;
    mine_item_s(item_s);
    mine_pattern(item_s);
}
