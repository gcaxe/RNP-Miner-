#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <set>

#include "type_old.h"

using namespace std;

//vector<vector<vector<char>>> readdatachar(string filename);
void readdataint(const string filename, vector<vector<vector<int>>>& data, vector<int>& allsymbol, int& sequencecount, int& itemsetcount, int& maxitemsetcountofsequence);
void showdataint(const vector<vector<vector<int>>>& data,
    const vector<int>& allsymbol, const int& sequencecount, const int& itemsetcount,
    const int& maxitemsetcountofsequence);

//char版, 已验证data、allsymbol、sequencecount、itemsetcount、maxitemsetcountofsequence
//要求全部输入参数：容器为空，整数0初始化
void readdataint(const string filename,
    vector<vector<vector<int>>>& data, vector<int>& allsymbol, int& sequencecount, int& itemsetcount,
    int& maxitemsetcountofsequence) {
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "无法打开文件: " << filename << endl;
        throw runtime_error("无法打开文件: " + filename);
    }

    string line;
    set<int> allsymbolset;
    //int maxICS = 1;

    while (getline(file, line)) {
        vector<vector<int>> row;
        vector<int> currentVector;
        stringstream ss(line);
        string token;

        int maxICS = 1;
        sequencecount++;
        itemsetcount++;

        while (ss >> token) {
            // >> 是流提取操作符，它会从 stringstream中读取数据，直到遇到空白字符
            // ss >> token 会：跳过空白字符，读取连续的非空白字符存入token，直到遇到空白字符，返回ss本身，无法再读取时（到达流末尾），返回的流会转换为 false

            if (token == "-1") {
                itemsetcount++;
                if (!currentVector.empty()) {
                    row.push_back(currentVector);
                    currentVector.clear();
                }
                maxICS++;
                if (maxICS > maxitemsetcountofsequence) maxitemsetcountofsequence = maxICS;
            }

            else {
                try {
                    int num = stoi(token);  // 将字符串转为整数
                    if (num < 0) {
                        cerr << "错误：发现非-1的负数 '" << token
                            << "' 于第 " << data.size() + 1 << " 行" << endl;
                        throw invalid_argument("错误：发现非-1的负数 '" + token +
                            "' 于第 " + to_string(data.size() + 1) + " 行");
                    }
                    currentVector.push_back(num);
                    allsymbolset.insert(num);
                }
                catch (const invalid_argument& e) {
                    //cerr << "无效的数字: " << token << endl;
                }
                catch (const out_of_range& e) {
                    cerr << "数字超出范围: " << token << endl;
                }
            }
        }

        if (!currentVector.empty()) {
            row.push_back(currentVector);
        }

        data.push_back(row);
    }

    file.close();

    for (int sy : allsymbolset) {
        allsymbol.push_back(sy);
    }

    // 输出解析结果
    //showdataint(data, allsymbol, sequencecount, itemsetcount, maxitemsetcountofsequence);



    //return data;
}

void showdataint(const vector<vector<vector<int>>>& data,
    const vector<int>& allsymbol, const int& sequencecount, const int& itemsetcount,
    const int& maxitemsetcountofsequence) {
    //int max = 0;
    for (size_t i = 0; i < data.size(); ++i) {
        cout << "行 " << i + 1 << ": ";

        for (const auto& vec : data[i]) {

            cout << "[";
            for (size_t j = 0; j < vec.size(); ++j) {
                cout << vec[j];
                if (j < vec.size() - 1) cout << ", ";
            }
            cout << "] ";
        }
        cout << endl;
    }
    cout << "allsymbol" << endl;
    //for (Tsymbol sy : allsymbol) {
    for (int sy : allsymbol) {
        cout << sy << " ";
    }
    cout << endl;
    cout << "seq " << sequencecount;
    cout << ", itemcount " << itemsetcount;
    cout << ", maxISofS " << maxitemsetcountofsequence;

}




