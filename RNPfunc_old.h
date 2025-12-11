#pragma once

//#include <functional>
#include <vector>
#include <map>
#include <set>
#include <iostream>

#include "type_old.h"

using namespace std;

/*
typedef char Tsymbol; //项的类型是符号
typedef vector<Tsymbol> Titemset; //项集
typedef vector<vector<Tsymbol>> Tpattern;//pattern模式
typedef vector<vector<vector<Tsymbol>>> Tdatabase; //数据库
typedef map<Tsymbol,vector<vector<int>>> Tpos_dict; //位置字典
typedef vector<vector<int>> Tpo;//po出现位置
*/

void getDataPro(const Tdatabase& D, Tpos_dict& S);
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
void getpoRule2(const Tpattern& p, const map<Tpattern, Tpo>& mappo, Tpo& result);
void vector_intersection(const vector<int>& v1, const vector<int>& v2, vector<int>& result);
void two_dimensional_intersection(const vector<vector<int>>& vv1, const vector<vector<int>>& vv2, vector<vector<int>>& result);

void showallfp(vector<Tpattern> allfp);
void showpattern(Tpattern p);


//获取位置字典
void getDataPro(const Tdatabase& D, Tpos_dict& S) {
    //Tpos_dict S;
    int i = 0;
    set<Tsymbol> allitem;
    for (vector<vector<Tsymbol>> sc : D) { //cpp能保证顺序访问
        
        int j = 0;
        for (vector<Tsymbol> sj : sc) {
            j++;
            for (Tsymbol item : sj) {
                while (S[item].size() <= i) { //初始化, 
                    S[item].push_back({});//补上前边的空一维数组
                }
                S[item][i].push_back(j);

                allitem.insert(item);
            }
        }
        i++;
    }
    for (Tsymbol item : allitem) { //补上后边的空一维数组
        //while ( int( S[item].size() ) <= i - 1) {//必须是int之间比较，如果是uint可能0-1=大正数
        while (S[item].size() < i ) {
            S[item].push_back({});
        }
    }
    //return S;
}


//项集模式链接ItemsetPatternJoin
void IPJ(const vector<Tpattern>& FP, vector<Tpattern>& C) {
    //外部来保证FP非空

    //处理size=1的情况，即处理前缀or后缀为空的情况
    //
    if (FP[0].size() == 1) {
        for (int i = 0; i < FP.size(); i++) {
            for (int j = 0; j < FP.size(); j++) {
                Tpattern p(2);
                p[0] = FP[i][0];
                p[1] = FP[j][0];
                C.push_back(p);
            }
        }
        return;
    }

    //处理size>=2的情况
    //vector<Tpattern> C; // 候选模式集合
    for (const auto& p : FP) {
        // 获取模式p的后缀（去掉第一个项集）
        Tpattern psuffix;
        if (!p.empty()) {  // 检查模式p不为空
            // 使用insert和迭代器范围复制从第二个项集开始的所有项集
            psuffix.insert(psuffix.end(), p.begin() + 1, p.end());
        }

        // 使用二分查找找到第一个前缀等于psuffix的模式
        int low = 0;
        int high = FP.size() - 1;
        int mid = (low + high) / 2;
        bool found = false;

        while (low <= high) {
            const auto& current = FP[mid];
            // 获取当前模式的前缀（去掉最后一个项）
            Tpattern current_prefix;
            if (!current.empty()) {
                current_prefix.insert(current_prefix.end(), current.begin(), current.end() - 1);
            }

            if (current_prefix == psuffix) {
                // 找到匹配，但需要找到第一个出现的位置
                while (mid > 0) {
                    const auto& prev = FP[mid - 1];
                    Tpattern prev_prefix;
                    if (!prev.empty()) {
                        prev_prefix.insert(prev_prefix.end(), prev.begin(), prev.end() - 1);
                    }
                    if (prev_prefix != psuffix) break;
                    mid--;
                }
                found = true;
                break;
            }
            else if (current_prefix < psuffix) {
                low = mid + 1;
            }
            else {
                high = mid - 1;
            }
            mid = (low + high) / 2;
        }

        if (!found) continue;

        // 连接所有具有相同前缀的模式
        while (mid < FP.size()) {
            const auto& q = FP[mid];
            // 检查q的前缀是否等于psuffix
            Tpattern q_prefix;
            if (!q.empty()) {
                q_prefix.insert(q_prefix.end(), q.begin(), q.end() - 1);
            }

            if (q_prefix != psuffix) break;

            // 执行连接操作：p ⊕ q
            Tpattern t = p;
            if (!q.empty() && !q.back().empty()) {
                // 添加q的最后一个项
                if (t.empty()) {
                    t.push_back({});
                }
                //t.back().push_back(q.back().back());
                t.push_back(q.back());
            }
            C.push_back(t);
            mid++;
        }
    }
    //return C;
}



//RNP，可能返回元组
void RNP(const Tdatabase& D, const int minsup, const vector<Tsymbol>& allsymbol, vector<Tpattern>& allFP) {
    //vector<Tpattern> allFP;//全部频繁模式
    vector<int> allsupport;//全部支持度 
    map<Tpattern, Tpo> allmappo;//最快，空间换时间

    Tpos_dict S;//位置字典
    getDataPro(D, S);

    vector<Tpattern> nextFP;//循环内?
    vector<Tpattern> curFP;
    vector<Tpattern> C;

    size1_miner(S, minsup, allsymbol, allFP, allsupport, allmappo, nextFP);//对的

    while (nextFP.size() != 0) {
        curFP = nextFP;
        nextFP = {};
        C = {};
        IPJ(curFP, C);

        //showallfp(C);
        //cout << endl;
        //showallfp(nextFP);

        for (auto & p : C) {
            CSC(p, S, minsup, allFP, allsupport, allmappo, nextFP);
            
        }

        //showallfp(nextFP);
        //cout << endl;
    }

    return;
}

//挖掘size为1的模式
void size1_miner(const Tpos_dict& S, int minsup, const vector<Tsymbol>& allsymbol,
    vector<Tpattern>& allFP, vector<int>& allsupport, map<Tpattern, Tpo>& allmappo, vector<Tpattern>& nextFP) {
    //len=1
    vector<Tpattern> Len1;
    for (Tsymbol sy : allsymbol) {
        Tpo vvi = S.at(sy);
        int sum = 0;
        for (vector<int> vi : vvi) {
            for (int i : vi) {
                //sum += i;
                sum++;
            }
        }
        if (sum > minsup) {
            Tpattern p{ {sy} };
            Tpo po = vvi;

            allFP.push_back(p);
            allsupport.push_back(sum);
            allmappo[p] = po;
            nextFP.push_back(p);

            Len1.push_back(p);
        }
        //cout << sy << " " << sum;
    }

    //cout << "size1len1 "<<endl;
    //showallfp(allFP);
    //cout << endl;

    //cout << "Len1 " << endl;
    //showallfp(Len1);  是先a后c
    //cout << endl;


    //len=2，双循环,不应该是ca应该是ac
    vector<Tpattern> Len2;
    for (int i = 0; i < Len1.size(); i++) {
        //for (int j = 0; j != i && j < Len1.size(); j++) {
        for (int j = i+1; j < Len1.size(); j++) {//相当于不含对角线的上半矩阵的遍历

            int sum = 0;
            Tpattern p{ { Len1[i][0][0], Len1[j][0][0] } };

            //cout << i << endl;
            //cout << Len1[i][0][0] << endl;
            //showpattern(p);//ca
            //cout << endl;

            Titemset itemset{ Len1[i][0][0], Len1[j][0][0] };
            Tpo po;
            getpoRule1(itemset, S, po);

            for (const auto& it : po) {
                sum += it.size();
            }

            if (sum > minsup) {
                allFP.push_back(p);
                allsupport.push_back(sum);
                allmappo[p] = po;
                nextFP.push_back(p);

                Len2.push_back(p);
            }
        }
    }
    //cout << "size1len2 ";
    //showallfp(allFP);
    //cout << endl;
    //cout << endl;

    //len>=3，I模式扩展，或者说项集连接

    miningS1L3(S, minsup, allsymbol, Len2, allFP, allsupport, allmappo, nextFP);

    //cout << "size1len3 ";
    //showallfp(allFP);
    //cout << endl;
}

void miningS1L3(const Tpos_dict& S, int minsup, const vector<Tsymbol>& allsymbol, const vector<Tpattern>& Len2,
    vector<Tpattern>& allFP, vector<int>& allsupport, map<Tpattern, Tpo>& allmappo, 
    vector<Tpattern>& nextFP) {

    vector<Tpattern> Lencur = Len2;
    vector<Tpattern> Lennext;
    vector<Tpattern> C;
    //可以减半查找优化
    while (!Lencur.empty()) {
        for (Tpattern p1 : Lencur) {
            Tpattern p1front(p1.begin(), p1.end() - 1);//kong
            Tpattern p1last(p1.begin() + 1, p1.end());
            for (Tpattern p2 : Lencur) {
                Tpattern p2front(p2.begin(), p2.end() - 1);
                Tpattern p2last(p2.begin() + 1, p2.end());
                if (p1front == p2last) {
                    Tpattern p = p1;//是p1不是p1front
                    p.push_back(p2.back());
                    C.push_back(p);
                }
                if (p2front == p1last) {
                    Tpattern p = p2;
                    p.push_back(p1.back());
                    C.push_back(p);
                }
            }
        }

        //计算支持度
        for (Tpattern p : C) {
            Tpo po;
            getpo(p, S, allmappo, po);
            //int 
            int sum = 0;
            for (const auto& it : po) {
                sum += it.size();
            }

            if (sum > minsup) {
                allFP.push_back(p);
                allsupport.push_back(sum);
                allmappo[p] = po;
                nextFP.push_back(p);

                Lennext.push_back(p);
            }
        }
        C = {};

        Lencur = Lennext;
        Lennext = {};
    }
}

//支持度计算CSC，改为直接输入po？
void CSC(const Tpattern& p, const Tpos_dict& Pos_dict, const int minsup,
    vector<Tpattern>& allFP, vector<int>& allsupport, map<Tpattern, Tpo>& allmappo, vector<Tpattern>& nextFP) {
    Tpo po;
    getpo(p, Pos_dict, allmappo, po);
    //int 
    int sum = 0;
    for (const auto& it : po) {
        sum += it.size();
    }

    //后面这几行可以不放在这个函数里面，可改为引用传参sum
    if (sum > minsup) {
        allFP.push_back(p);
        allsupport.push_back(sum);
        allmappo[p] = po;
        nextFP.push_back(p);
    }
    //return sum;
}

//求出现位置po
void getpo(const Tpattern& p, const Tpos_dict& Pos_dict, const map<Tpattern, Tpo>& mappo, Tpo& result) {
    //Tpo result;
    if (p.size() == 1) {
        //result=
        getpoRule1(p[0], Pos_dict, result);
    }
    else {
        //result=
        getpoRule2(p, mappo, result);
    }
    //return result;
}


//getpoRule1,返回值可能为空
void getpoRule1(const Titemset& itemset, const Tpos_dict& Pos_dict, Tpo& result) {
    if (itemset.empty()) {
        return; // 返回空的二维向量
    }

    result = Pos_dict.at(itemset[0]);
    for (size_t i = 1; i < itemset.size(); i++) {
        Tpo temp = result;
        result = {};
        two_dimensional_intersection(temp, Pos_dict.at(itemset[i]), result);//改写为多线程时可能有风险
    }
    //return result; 
}


//getpoRule2,不需要const Tpos_dict& Pos_dict
void getpoRule2(const Tpattern& p, const map<Tpattern, Tpo>& mappo, Tpo& result) {
    Tpattern pre_p;//前缀模式
    pre_p.insert(pre_p.end(), p.begin(), p.end() - 1);
    Tpo po1, po2;
    //po1=mappo[pre_p];//[]与const冲突
    po1 = mappo.at(pre_p);
    //po2=getpoRule1(p.back(), Pos_dict);
    auto back_p = { p.back() };
    //po2=mappo[back_p];
    po2 = mappo.at(back_p);

    result.resize(po1.size(), vector<int>());

    for (int k = 0; k < po1.size(); k++) {
        vector<int> v1 = po1[k], v2 = po2[k];
        int i = 0, j = 0;
        while (i < v1.size() && j < v2.size()) {
            if (v1[i] < v2[j]) {
                result[k].push_back(v2[j]);//存入后面的
                i++, j++;
            }
            else {
                j++;
            }
        }
    }

    //return result;
}



//一维数组求交集，可能大小不一，输入可能有空集，输出可能为空,要求result为空
void vector_intersection(const vector<int>& v1, const vector<int>& v2, vector<int>& result) {
    //vector<int> result;
    // 如果任一向量为空，直接返回空结果
    if (v1.empty() || v2.empty()) {
        //return result;
        return;
    }
    // 使用双指针法求交集
    int i = 0, j = 0;
    while (i < v1.size() && j < v2.size()) {
        if (v1[i] == v2[j]) {
            result.push_back(v1[i]);
            i++;
            j++;
        }
        else if (v1[i] < v2[j]) {
            i++;
        }
        else {
            j++;
        }
    }

    //return result;
}
// 求两个二维向量的交集，size一样,每个一维数组size不一定一样 一维可能为空，二维输入一定非空，二维输出可能为空,要求result为空
void two_dimensional_intersection(const vector<vector<int>>& vv1, const vector<vector<int>>& vv2, vector<vector<int>>& result) {

    //vector<vector<int>> result;
    // 对每个位置的一维向量求交集
    for (size_t i = 0; i < vv1.size(); i++) {
        vector<int> v3;
        vector_intersection(vv1[i], vv2[i], v3);
        result.push_back(v3);  //如果v3为空，也能正常运行
    }
    //return result;
}







void showallfp(vector<Tpattern> allfp) {
    for (Tpattern p : allfp) {
        cout << "[ ";
        showpattern(p);
        cout << " ] " << endl;
    }
}

void showpattern(Tpattern p) {
    for (Titemset is : p) {
        cout << "[ ";
        for (Tsymbol sy : is) {
            cout << sy << ' ';
        }
        cout << " ] ";
    }

}
