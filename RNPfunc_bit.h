#pragma once

#include <vector>
//#include <bitset>
#include <map>
#include <set>
#include <iostream>
#include "type_bit.h"

#include <numeric>  //std::accumulate()

using namespace std;

//为了之后改成多线程的，本文件内尽量不要使用push_back等


void IPJ(const vector<Tpattern>& FP, vector<Tpattern>& C);

void size1_miner(const Tpos_dict& S, int minsup, const vector<Tsymbol>& allsymbol,
    vector<Tpattern>& allFP, vector<int>& allsupport, map<Tpattern, Tpo>& allmappo, vector<Tpattern>& nextFP);
void miningS1L3(const Tpos_dict& S, int minsup, const vector<Tsymbol>& allsymbol, const vector<Tpattern>& Len2,
    vector<Tpattern>& allFP, vector<int>& allsupport, map<Tpattern, Tpo>& allmappo,
    vector<Tpattern>& nextFP);

void RNP(const Tdatabase& D, const int minsup, const vector<Tsymbol>& allsymbol, vector<Tpattern>& allFP);

void CSC(const Tpattern& p, const Tpos_dict& Pos_dict, const int minsup,
    vector<Tpattern>& allFP, vector<int>& allsupport, map<Tpattern, Tpo>& allmappo, vector<Tpattern>& nextFP);

void getpo(const Tpattern& p, const Tpos_dict& Pos_dict, const map<Tpattern, Tpo>& mappo, Tpo& result);
void getpoRule1(const Titemset& itemset, const Tpos_dict& Pos_dict, Tpo& result);



void showallfp(vector<Tpattern> allfp);
void showpattern(Tpattern p);


//我需要这个Disize,写在loaddatal里面
//vector<int> Disize;
//这个没必要在gpu上运行
//for (int i = 0; i < D.size(); i++) {
//    Disize.push_back(D[i].size());
//}
//

//vector<int> Diasize, Dibsize;
//这个没必要在gpu上运行
//for (int i = 0; i < D.size(); i++) {  // size=64*a+b
//    // (int1 + int2 - 1) / int2;
//    // 当int2为2^n时可以用 (int1 + int2 - 1) >> (int)log2(int2);
//    Diasize.push_back((D[i].size() + 63) >> 6);  // 储存a+1或者说，向上取整的size/64。
//
//    // int1%64可能被编译器自动优化为&63
//    Dibsize.push_back(D[i].size() & 63);  //储存b
//}


//vector<int> Diasize, Dibsize;
//这个没必要在gpu上运行
//for (int i = 0; i < D.size(); i++) {  // size=64*a+b
//    // (int1 + int2 - 1) / int2;
//    // 当int2为2^n时可以用 (int1 + int2 - 1) >> (int)log2(int2);
//    Diasize.push_back((D[i].size() + 191) / 192);  // 储存a+1或者说，向上取整的size/64。
//
//    // int1%64可能被编译器自动优化为&63
//    Dibsize.push_back(D[i].size() % 192);  //储存b
//}


// 初始化时似乎只需要Diasize或者Disize
// 是否我可以把他们存入一个vector，每次只取[0]



//
void getDataPro(const Tdatabase& D, const vector<Tsymbol>& allsymbol, const vector<int> Disize, unordered_map<Tsymbol, vector<vector<char>>>& S) {
    //gpu版，需要再写个cpu版看看是否真的有速度优势。记得包括显存复制的时间

    //同步
    //这个不能在gpu上运行，
    for (int i = 0; i < allsymbol.size(); i++) {
        Tsymbol sy = allsymbol[i];
        S[sy].resize(D.size());
        for (int j = 0; j < S[sy].size(); j++) {
            //S[sy][j].resize(D[j].size());
            S[sy][j].resize(Disize[j]);
        }
    }
    //同步
    //前文相当于初始化S

    for (int i = 0; i < D.size(); i++) {  // D[i]是一个序列
        for (int j = 0; j < D[i].size(); j++) {  // D[i][j]是一个项集itemset
            for (int k = 0; k < D[i][j].size(); k++) {
                Tsymbol sy = D[i][j][k];
                S[sy][i][j] = 1;
            }
        }
    }
}
void getDataPro(const Tdatabase& D, const vector<Tsymbol>& allsymbol, const vector<int> Diasize, unordered_map<Tsymbol, vector<vector<uint64_t>>>& S) {
    //gpu版，需要再写个cpu版看看是否真的有速度优势。记得包括显存复制的时间
    // 
    //同步
    //这个不能在gpu上运行，
    for (int i = 0; i < allsymbol.size(); i++) {
        Tsymbol sy = allsymbol[i];
        S[sy].resize(D.size());
        for (int j = 0; j < S[sy].size(); j++) {
            S[sy][j].resize(Diasize[i]);
        }
    }
    //前文相当于初始化S
    //同步
    for (int i = 0; i < D.size(); i++) {  // D[i]是一个序列
        for (int j = 0; j < D[i].size(); j++) {  // D[i][j]是一个项集itemset
            for (int k = 0; k < D[i][j].size(); k++) {
                Tsymbol sy = D[i][j][k];
                S[sy][i][ j>>6 ]  &=  ((uint64_t)1 ) << (j&63);
            }
        }
    }
}
void getDataPro(const Tdatabase& D, const vector<Tsymbol>& allsymbol, const vector<int> Diasize, unordered_map<Tsymbol, vector<vector<Tuint192>>>& S) {
    //gpu版，需要再写个cpu版看看是否真的有速度优势。记得包括显存复制的时间
    // 
    //同步
    //这个不能在gpu上运行，
    for (int i = 0; i < allsymbol.size(); i++) {
        Tsymbol sy = allsymbol[i];
        S[sy].resize(D.size());
        for (int j = 0; j < S[sy].size(); j++) {
            S[sy][j].resize(Diasize[i]);
        }
    }

    //同步
    //前文相当于初始化S
    for (int i = 0; i < D.size(); i++) {  // D[i]是一个序列
        for (int j = 0; j < D[i].size(); j++) {  // D[i][j]是一个项集itemset
            for (int k = 0; k < D[i][j].size(); k++) {
                Tsymbol sy = D[i][j][k];
                int b = j % 192;
                /*
                S[sy][i][j / 192] &= { //Tuint192是聚合类型(无自定义构造函数、无私有成员等),可以直接用 {}初始化
                    b < 64  ?  1<<b  :  0  ,  
                    (b >= 64) && (b <128) ?  1<<b-64  :  0,
                    b >=128  ?  1<<b-128  :  0                    
                }; 
                */
                S[sy][i][j / 192] &= { //Tuint192是聚合类型(无自定义构造函数、无私有成员等),可以直接用 {}初始化
                    (b < 64)* (1 << b),
                    ((b >= 64) & (b < 128))* (1 << (b - 64)),
                    (b >= 128)* (1 << (b - 128))
                };

                //((uint64_t)1) << (j & 63);
            }
        }
    }
}


void IPJ(const vector<Tpattern>& FP, vector<Tpattern>& C) {
    //

    //在外面构建C，可以设置特殊值表示空
    multimap<Tpattern, Tpattern> mper, msuf;
    vector<Tpattern> keyper, keysuf;
    for (int i = 0; i < FP.size(); i++) {
        Tpattern p = FP[i];
        Tpattern per(p.begin(), p.end() - 1);
        Tpattern last{ p.back() };
        Tpattern suf(p.begin() + 1, p.end());
        Tpattern first{ p.front() };
        mper.insert({ per,last });
        msuf.insert({ suf,first });
        //m[per].push_back(last);
        //m[suf].push_back(first);
        keyper.push_back(per);
        keysuf.push_back(suf);

    }
    set<Tpattern> s(keyper.begin(), keyper.end());
    vector<Tpattern> keyper_unique(s.begin(), s.end());

    for (int i = 0; i < keyper_unique.size(); i++) {
        Tpattern per = keyper_unique[i];
        auto range1 = msuf.equal_range(per);
        auto range2 = mper.equal_range(per);
        for (auto it1 = range1.first; it1 != range1.second; ++it1) {  // 条件没问题
            // std::cout << it->second << " ";
            //it->first是键，键长度为n-1
            //it->second是值,值长度为1

            Tpattern newp= it1->second;
            newp.insert(newp.end(), per.begin(), per.end());
            for(auto it2 = range2.first; it2 != range2.second; ++it2) {
                Tpattern finalp = newp;
                finalp.push_back(it2->second[0]);
                C.push_back(finalp);
            }
        }
    }



}

void size1_miner(const Tpos_dict& S, const int minsup, const vector<Tsymbol>& allsymbol, const vector<int>& Disize,
    vector<Tpattern>& nextFP, vector<Tpattern>& allFP, map<Tpattern, Tpo>& allmappo) {
    
    S1L1miner();
    S1L2miner();
    S1L3miner();

}

void S1L1miner(const Tpos_dict& S, const int minsup, const vector<Tsymbol>& allsymbol, 
    vector<Tpattern>& nextFP, vector<Tpattern>& allFP, map<Tpattern, Tpo>& allmappo, vector<Tpattern>& Len1) {

    for (int i = 0; i < allsymbol.size(); i++) {
        Tsymbol sy = allsymbol[i];
        Tpo po = S.at(sy);
        int count = 0;
        for (int i = 0; i < po.size(); i++) {
            countbitone(po[i], count);
        }

        if (count > minsup) {
            Tpattern p{ {sy} };
            allmappo[p] = po;
            allFP.push_back(p);
            nextFP.push_back(p);
            Len1.push_back(p);
        }
    }
}

void S1L2miner(const Tpos_dict& S, const int minsup, const vector<Tsymbol>& allsymbol, const vector<Tpattern>& Len1, const vector<int> Disize,
    vector<Tpattern>& nextFP, vector<Tpattern>& allFP, map<Tpattern, Tpo>& allmappo, vector<Tpattern>& Len2) {
    for (int i = 0; i < Len1.size(); i++) {
        for (int j = i + 1; j < Len1.size(); j++) {
            Tpattern p{ { Len1[i][0][0], Len1[j][0][0] } };
            Tpo po;
            initpo(Disize, po);
            getpoRule1len2(p, S, po);
            int count = 0;
            for (int i = 0; i < po.size(); i++) {
                countbitone(po[i], count);
            }

            if (count > minsup) {
                allmappo[p] = po;
                allFP.push_back(p);
                nextFP.push_back(p);
                Len2.push_back(p);
            }
        }
    }
}

void S1L3miner(const Tpos_dict& S, const int minsup, const vector<Tsymbol>& allsymbol, const vector<Tpattern>& Len2, const vector<int> Disize,
    vector<Tpattern>& nextFP, vector<Tpattern>& allFP, map<Tpattern, Tpo>& allmappo) {

    vector<Tpattern> Lencur = Len2;
    vector<Tpattern> Lennext;
    vector<Tpattern> C;
    //可以减半查找优化

    multimap<Titemset, Titemset> m1, m2;
    vector<Titemset> keyper;
    while (!Lencur.empty()) {
        for (int i = 0; i < Lencur.size(); i++) {
            Tpattern p = Lencur[i];
            Titemset itemset = p[0];
            Titemset per(itemset.begin(), itemset.end() - 1), last{ itemset.back()}, 
                suf( itemset.begin() + 1, itemset.end()), front{ itemset[0]};
            m1.insert({ per, last });
            m2.insert({ suf, front });
            keyper.push_back(per);
        }
        set<Titemset> s(keyper.begin(), keyper.end());
        vector<Titemset> keyper_unique(s.begin(), s.end());

        for (int i = 0; i < keyper_unique.size(); i++) {
            Titemset per = keyper_unique[i];
            auto range1 = m2.equal_range(per);
            auto range2 = m1.equal_range(per);
            for (auto it1 = range1.first; it1 != range1.second; ++it1) {  // 条件没问题
                // std::cout << it->second << " ";
                //it->first是键
                //it->second是值

                Titemset itemset = it1->second;//front
                itemset.insert(itemset.end(), per.begin(), per.end());
                for (auto it2 = range2.first; it2 != range2.second; ++it2) {
                    itemset.push_back(it2->second[0]);
                    Tpattern p{ itemset };
                    C.push_back(p);

                }
            }
        }
        for (int i = 0; i < C.size(); i++) {
            Tpattern p = C[i];
            Tpo po;
            initpo(Disize, po);
            getpoRule1len3(p, S, allmappo, po);

            int count = 0;
            for (int i = 0; i < po.size(); i++) {
                countbitone(po[i], count);
            }
            if (count > minsup) {
                allmappo[p] = po;
                Lennext.push_back(p);
                nextFP.push_back(p);
                allFP.push_back(p);
            }
        }
        Lencur = Lennext;
        Lennext = {};

    }
}


void RNP(const Tdatabase& D, const int minsup, const vector<Tsymbol>& allsymbol, const vector<int> Disize, vector<Tpattern>& allFP) {
    
    map<Tpattern, Tpo> allmappo;

    Tpos_dict S;//位置字典
    getDataPro(D, allsymbol, Disize, S);

    vector<Tpattern> nextFP;
    vector<Tpattern> curFP;
    vector<Tpattern> C;
    vector<uint8_t> nextFPflag;

    size1_miner(S, minsup, allsymbol, Disize, nextFP, allFP, allmappo);

    while (nextFP.size() != 0) {
        curFP = {};
        /*
        auto end = copy_if(nextFP.begin(), nextFP.end(), nextFPflag.begin(), std::back_inserter(curFP),
            [](int flag) { return flag == 1; }
        );//似乎gpu的vector也有高效多线程插入

        int compressed_size = end - curFP.begin();
        curFP.resize(compressed_size);
        */
        std::copy_if(nextFP.begin(), nextFP.end(), std::back_inserter(curFP),
            [&nextFPflag, index = 0](const auto& element) mutable {
                return vflag[index++] == 1;
        });

        
        
        allFP.insert(allFP.end(), curFP.begin(), curFP.end());
        curFP = nextFP;
        //nextFP = {};
        C = {};

        IPJ(curFP, C);

        int n = C.size();
        nextFP = {};
        nextFP.resize(n * n);
        nextFPflag = {};
        nextFPflag.resize(n * n);

        for (int i = 0; i < C.size(); i++) {
            Tpattern p = C[i];
            CSC(p, S, minsup, i, Disize, allmappo, nextFP, nextFPflag);
        }
    }
    
}


void CSC(const Tpattern& p, const Tpos_dict& Pos_dict, const int minsup, const int iii,  //iii表示线程号
    const vector<int>& Disize,
    //vector<Tpattern>& allFP, vector<int>& allsupport, 改为在外面处理allfp
    map<Tpattern, Tpo>& allmappo, vector<Tpattern>& nextFP, vector<uint8_t>& nextFPflag) {
    //在外面初始化nextFP, nextFPflag
    if(p.size()==1) {
        //?
    }
    else {
        Tpo po;
        initpo(Disize, po);
        getpoRule2(p, allmappo, po);
        int count = 0;
        for (int i = 0; i < po.size(); i++) {
            countbitone(po[i], count);
        }
        if (count > minsup) {
            allmappo[p] = po;
            nextFP[iii] = p;
            nextFPflag[iii] = 1;
        }
    }
}

void initpo(const vector<int> Disize, Tpo& po) {
    po.resize(Disize.size());
    for (int i = 0; i < po.size(); i++) {
        po[i].resize(Disize[i]);
    }
}

void countbitone(const vector<char>& bitarr, int& count) {
    count += std::accumulate(bitarr.begin(), bitarr.end(), 0);
}
void countbitone(const vector<uint64_t>& bitarr, int& count) {
    // 最佳，__builtin_popcountll或 std::popcount
    // 其次，查表 或 Kernighan 算法
    static const uint8_t lookup[256] = {
        0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
        1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
        1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
        2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
        1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
        2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
        2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
        3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8
    };

    for (int i = 0; i < bitarr.size(); i++) {
        int x = bitarr[i];
        count += lookup[x & 0xFF] + lookup[(x >> 8) & 0xFF] +
            lookup[(x >> 16) & 0xFF] + lookup[(x >> 24) & 0xFF] +
            lookup[(x >> 32) & 0xFF] + lookup[(x >> 40) & 0xFF] +
            lookup[(x >> 48) & 0xFF] + lookup[(x >> 56) & 0xFF];
    }
}

inline void getpoRule1len1(const Tpattern& p, const Tpos_dict& Pos_dict, Tpo& result) {
    Tsymbol sy = p[0][0];
    result = Pos_dict.at(sy);
}
inline void getpoRule1len2(const Tpattern& p, const Tpos_dict& Pos_dict, Tpo& result) {
    //需要提前初始化result
    Titemset itemset = p[0];
    //result = Pos_dict.at(itemset[0]) & Pos_dict.at(itemset[1]); //这里是vector<bitarr>
    bitarr_intersection(Pos_dict.at(itemset[0]), Pos_dict.at(itemset[1]), result);//需要提前初始化result
}
void getpoRule1len3(const Tpattern& p, const Tpos_dict& Pos_dict, const map<Tpattern, Tpo>& mappo, Tpo& result) {
    //需要提前初始化result
    Titemset itemset = p[0];
    Titemset peritemset(itemset.begin()+1, itemset.end() );
    Tpattern per{ peritemset };
    Tpo po1 = Pos_dict.at(itemset[0]);
    Tpo po2 = mappo.at(per);
    bitarr_intersection(po1, po2, result);//需要提前初始化result
}

void getpoRule2(const Tpattern& p, const map<Tpattern, vector<vector<char>>>& mappo, vector<vector<char>>& result) {
    //result在外面初始化与调整大小
    Tpattern per(p.begin(), p.end() - 1);
    Tpattern end{ p.back() };
    vector<vector<char>> perpo = mappo.at(per);//可以只读
    vector<vector<char>> endpo = mappo.at(end);//必须复制
    
    for (int i = 0; i < perpo.size(); i++) {
        for (int j = 0; j < perpo[i].size(); j++) {
            if (perpo[i][j] == 1) {
                for (int k = j + 1; k < endpo[i].size(); k++) {
                    if(endpo[i][k] == 1) {
                        endpo[i][k] = 0;
                        result[i][k] = 1;
                        break;
                    }
                }
            }
        }
    }
}

void getpoRule2(const Tpattern& p, const map<Tpattern, vector<vector<uint64_t>>>& mappo, vector<vector<uint64_t>>& result) {
    //result在外面初始化与调整大小
    Tpattern per(p.begin(), p.end() - 1);
    Tpattern end{ p.back() };
    vector<vector<uint64_t>> perpo = mappo.at(per);//可以只读
    vector<vector<uint64_t>> endpo = mappo.at(end);//必须复制

    for (int i = 0; i < perpo.size(); i++) {
        for (int j1 = 0, L1=0; j1 < perpo[i].size(); L1++) {//这里写成最好1层for，不然if里面的for不好写
            //这个if的确应当放在最前面
            if (L1 == 64) {
                L1 = 0;
                j1++;
            }

            if (perpo[i][j1] & 1ULL << L1) {
                for (int j2 = j1, L2 = L1 + 1; j2 < endpo[i].size(); L2++) {
                    if (L2 == 64) {
                        L2 = 0;
                        j1++;
                    }
                    if (endpo[i][j2] & 1ULL << L2) {
                        endpo[i][j2] &= ~(1ULL << L2);
                        result[i][j2] |= 1ULL << L2;
                        break;
                    }
                }
            }

        }
    }
}

void bitarr_intersection(const vector<Tbitarr>& vbitarr1, const vector<Tbitarr>& vbitarr2, vector<Tbitarr>& vbitarr3) {
    //需要 提前 初始化或者resize  vbitarr3
    if (vbitarr1.size() != vbitarr2.size()) {
        cerr << "error in bitarr_intersection" << endl;
        return;
    }
    for (int i = 0; i < vbitarr1.size(); i++) {
        vbitarr3[i] = vbitarr1[i] & vbitarr2[i];
    }
}





/*
//二进制计算
//计算1的数量
int countBits(Tbitarr a) {//gcx检查了正确性
    int count = 0;
    while (a != 0) {
        a &= (a - 1);  // 消除最低位的 1
        count++;
    }
    return count;
}

*/