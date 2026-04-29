#include "prefixspan.h"
#include <set>

PrefixSpan::PrefixSpan(const std::string& filename, int min_sup)
    : minsup_(min_sup) {
    read_file(filename);
}

void PrefixSpan::read_file(const std::string& filename) {
    auto lines = read_lines(filename);
    for (const auto& line : lines) {
        std::vector<std::string> itemset;
        std::vector<std::vector<std::string>> seq;

        std::istringstream iss(line);
        std::string token;
        while (iss >> token) {
            if (token != "-1") {
                itemset.push_back(token);
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
}

std::vector<std::string> PrefixSpan::get_elem(
    const std::vector<std::vector<std::vector<std::string>>>& data_list) {
    std::vector<std::string> elem;
    for (const auto& seq : data_list) {
        for (const auto& itemset : seq) {
            for (const auto& item : itemset) {
                if (std::find(elem.begin(), elem.end(), item) == elem.end()) {
                    elem.push_back(item);
                }
            }
        }
    }
    std::sort(elem.begin(), elem.end());
    return elem;
}

void PrefixSpan::delete_not_fre_elem(
    std::vector<std::vector<std::vector<std::string>>>& data,
    const std::vector<std::string>& not_fre_elem) {
    if (not_fre_elem.empty()) return;

    for (auto& seq : data) {
        for (auto& itemset : seq) {
            for (const auto& nf : not_fre_elem) {
                itemset.erase(
                    std::remove(itemset.begin(), itemset.end(), nf),
                    itemset.end());
            }
        }
        seq.erase(
            std::remove_if(seq.begin(), seq.end(),
                           [](const std::vector<std::string>& v) { return v.empty(); }),
            seq.end());
    }
    data.erase(
        std::remove_if(data.begin(), data.end(),
                       [](const std::vector<std::vector<std::string>>& v) { return v.empty(); }),
        data.end());
}

std::vector<std::vector<std::vector<std::string>>> PrefixSpan::get_prefix_data(
    const std::string& e,
    const std::vector<std::vector<std::vector<std::string>>>& data) {
    // Deep copy
    auto copy_data = data;
    int flage = 0;

    for (auto& seq : copy_data) {
        for (auto& itemset : seq) {
            for (auto it = itemset.begin(); it != itemset.end(); ) {
                if (itemset.size() <= 1) {
                    if (e != *it) {
                        it = itemset.erase(it);
                    } else {
                        it = itemset.erase(it);
                        flage = 1;
                        break;
                    }
                } else {
                    if (e != *it) {
                        it = itemset.erase(it);
                    } else {
                        it = itemset.erase(it);
                        if (!itemset.empty()) {
                            itemset[0] = "_" + itemset[0];
                        }
                        flage = 1;
                        break;
                    }
                }
            }
            if (flage == 1) {
                flage = 0;
                break;
            }
        }
        seq.erase(
            std::remove_if(seq.begin(), seq.end(),
                           [](const std::vector<std::string>& v) { return v.empty(); }),
            seq.end());
    }
    copy_data.erase(
        std::remove_if(copy_data.begin(), copy_data.end(),
                       [](const std::vector<std::vector<std::string>>& v) { return v.empty(); }),
        copy_data.end());
    return copy_data;
}

std::vector<std::vector<std::vector<std::vector<std::string>>>>
PrefixSpan::get_all_prefix_data(
    const std::vector<std::string>& elem,
    const std::string& prefix_e,
    const std::vector<std::vector<std::vector<std::string>>>& data_list) {
    auto data1 = data_list;
    std::vector<std::vector<std::vector<std::vector<std::string>>>> all_prefix_data;

    for (const auto& e : elem) {
        if (!e.empty() && e[0] == '_') {
            auto temp = use_cycle_get_prefix_data(e, prefix_e, data1);
            all_prefix_data.push_back(temp);
        } else {
            auto temp2 = get_prefix_data(e, data1);
            all_prefix_data.push_back(temp2);
        }
    }
    return all_prefix_data;
}

std::pair<std::vector<std::string>, std::vector<std::string>>
PrefixSpan::use_cycle_get_fre_elem(
    const std::vector<std::vector<std::vector<std::string>>>& data_list,
    const std::string& prefix_e,
    const std::vector<std::string>& elem) {
    std::unordered_map<std::string, int> elem_sup;

    for (const auto& e : elem) {
        for (const auto& seq : data_list) {
            for (const auto& itemset : seq) {
                if (!e.empty() && e[0] == '_') {
                    std::string temp = e.substr(1);
                    bool has_prefix = (std::find(itemset.begin(), itemset.end(), prefix_e) != itemset.end());
                    bool has_temp = (std::find(itemset.begin(), itemset.end(), temp) != itemset.end());
                    if (has_prefix && has_temp) {
                        elem_sup[e] = elem_sup[e] + 1;
                    }
                }
                if (std::find(itemset.begin(), itemset.end(), e) != itemset.end()) {
                    elem_sup[e] = elem_sup[e] + 1;
                    break;
                }
            }
        }
    }

    std::vector<std::string> fre_elem, not_fre_elem;
    for (const auto& kv : elem_sup) {
        ++can_num_;
        if (kv.second >= minsup_) {
            fre_elem.push_back(kv.first);
        } else {
            not_fre_elem.push_back(kv.first);
        }
    }
    return {fre_elem, not_fre_elem};
}

std::vector<std::vector<std::vector<std::string>>>
PrefixSpan::use_cycle_get_prefix_data(
    const std::string& e,
    const std::string& prefix_e,
    const std::vector<std::vector<std::vector<std::string>>>& data) {
    auto copy_data = data;
    int flage = 0;

    for (auto& seq : copy_data) {
        for (auto& itemset : seq) {
            if (!e.empty() && e[0] == '_') {
                std::string e1 = e.substr(1);
                bool has_prefix = (std::find(itemset.begin(), itemset.end(), prefix_e) != itemset.end());
                bool has_e1 = (std::find(itemset.begin(), itemset.end(), e1) != itemset.end());
                if (has_prefix && has_e1) {
                    itemset.erase(
                        std::remove_if(itemset.begin(), itemset.end(),
                                       [&](const std::string& x) {
                                           return x == prefix_e || x == e1;
                                       }),
                        itemset.end());
                    break;
                }
            }
            for (auto it = itemset.begin(); it != itemset.end(); ) {
                if (itemset.size() <= 1) {
                    if (e != *it) {
                        it = itemset.erase(it);
                    } else {
                        it = itemset.erase(it);
                        flage = 1;
                        break;
                    }
                } else {
                    if (e != *it) {
                        it = itemset.erase(it);
                    } else {
                        it = itemset.erase(it);
                        if (!itemset.empty()) {
                            itemset[0] = "_" + itemset[0];
                        }
                        flage = 1;
                        break;
                    }
                }
            }
            if (flage == 1) {
                flage = 0;
                break;
            }
        }
        seq.erase(
            std::remove_if(seq.begin(), seq.end(),
                           [](const std::vector<std::string>& v) { return v.empty(); }),
            seq.end());
    }
    copy_data.erase(
        std::remove_if(copy_data.begin(), copy_data.end(),
                       [](const std::vector<std::vector<std::string>>& v) { return v.empty(); }),
        copy_data.end());
    return copy_data;
}

std::vector<PrefixSpan::Sequence> PrefixSpan::cycle_get_fre_elem(
    const std::vector<std::vector<std::vector<std::string>>>& pre_fix_data,
    const std::string& e) {
    auto copy_pre_fix_data = pre_fix_data;
    std::vector<Sequence> all_fre_sequence;

    auto all_elem = get_elem(copy_pre_fix_data);
    auto [fre_elem, not_fre_elem] = use_cycle_get_fre_elem(copy_pre_fix_data, e, all_elem);
    delete_not_fre_elem(copy_pre_fix_data, not_fre_elem);
    auto this_all_prefix_data = get_all_prefix_data(fre_elem, e, copy_pre_fix_data);

    for (const auto& x : fre_elem) {
        Sequence seq;
        if (!x.empty() && x[0] == '_') {
            seq.push_back({e, x.substr(1)});
        } else {
            seq.push_back({e});
            seq.push_back({x});
        }
        all_fre_sequence.push_back(seq);
    }

    for (size_t i = 0; i < fre_elem.size(); ++i) {
        auto temp = cycle_get_fre_elem(this_all_prefix_data[i], fre_elem[i]);
        for (auto& x : temp) {
            Sequence t = x;
            if (!t.empty() && !t[0].empty() && !t[0][0].empty() && t[0][0][0] == '_') {
                std::string restored = t[0][0].substr(1);
                t[0] = {e, restored};
                all_fre_sequence.push_back(t);
            } else {
                t.insert(t.begin(), {e});
                all_fre_sequence.push_back(t);
            }
        }
    }
    return all_fre_sequence;
}

void PrefixSpan::run() {
    auto elem = get_elem(sdb_);
    auto [fre_elem, not_fre_elem] = use_cycle_get_fre_elem(sdb_, "-1", elem);
    delete_not_fre_elem(sdb_, not_fre_elem);
    auto all_prefix_data = get_all_prefix_data(fre_elem, "-1", sdb_);

    std::unordered_map<std::string, std::vector<Sequence>> all_fre_sequence;
    std::vector<std::vector<Sequence>> all_list_fre_sequence;

    for (size_t x = 0; x < all_prefix_data.size(); ++x) {
        auto l = cycle_get_fre_elem(all_prefix_data[x], fre_elem[x]);
        l.insert(l.begin(), {{{fre_elem[x]}}});
        all_fre_sequence[fre_elem[x]] = l;
        all_list_fre_sequence.push_back(l);
    }

    for (size_t length_e = 0; length_e < fre_elem.size(); ++length_e) {
        std::cout << fre_elem[length_e]
                  << " prefix, its frequent sequences below ---" << std::endl;
        for (const auto& seq : all_list_fre_sequence[length_e]) {
            std::cout << "[";
            for (size_t i = 0; i < seq.size(); ++i) {
                std::cout << "[";
                for (size_t j = 0; j < seq[i].size(); ++j) {
                    if (j > 0) std::cout << ", ";
                    std::cout << seq[i][j];
                }
                std::cout << "]";
                if (i + 1 < seq.size()) std::cout << ", ";
            }
            std::cout << "]" << std::endl;
        }
    }

    for (const auto& kv : all_fre_sequence) {
        fp_count_ += static_cast<int>(kv.second.size());
    }
    std::cout << "The number of FPs: " << fp_count_ << std::endl;
    std::cout << "The number of CPs: " << can_num_ << std::endl;
}
