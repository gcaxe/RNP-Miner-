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
void readdatachar(const string filename, vector<vector<vector<char>>>& data, vector<char>& allsymbol, int& sequencecount, int& itemsetcount, int& maxitemsetcountofsequence);
void showdatachar(const vector<vector<vector<char>>>& data,
    const vector<char>& allsymbol, const int& sequencecount, const int& itemsetcount,
    const int& maxitemsetcountofsequence);

//char版, 已验证data、allsymbol、sequencecount、itemsetcount、maxitemsetcountofsequence
//要求全部输入参数：容器为空，整数0初始化
void readdatachar(const string filename,
    vector<vector<vector<char>>>& data, vector<char>& allsymbol, int& sequencecount, int& itemsetcount,
    int& maxitemsetcountofsequence) {
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "无法打开文件: " << filename << endl;
        throw runtime_error("无法打开文件: " + filename);
    }

    //vector<vector<vector<char>>> data;  // 使用vector<char>
    string line;
    set<char> allsymbolset;
    int maxICS = 1;

    while (getline(file, line)) {
        vector<vector<char>> row;
        vector<char> currentVector;
        stringstream ss(line);
        string token;

        int maxICS = 1;
        sequencecount++;
        itemsetcount++;

        while (ss >> token) {

            

            if (token == "-1") {
                itemsetcount++;
                if (!currentVector.empty()) {
                    row.push_back(currentVector);
                    currentVector.clear();
                }
                maxICS++;
                if (maxICS > maxitemsetcountofsequence) maxitemsetcountofsequence = maxICS;
            }
            else if (token.length() == 1) {  // 确保是单个字符
                currentVector.push_back(token[0]);

                allsymbolset.insert(token[0]);
                
            }
            else {
                cerr << "错误：发现非单个字符元素 '" << token
                    << "' 于第 " << data.size() + 1 << " 行" << endl;
                throw invalid_argument("错误：发现非单个字符元素 '" + token +
                    "' 于第 " + to_string(data.size() + 1) + " 行");
            }
        }

        if (!currentVector.empty()) {
            row.push_back(currentVector);
        }

        data.push_back(row);
    }

    file.close();

    for (Tsymbol sy : allsymbolset) {
        allsymbol.push_back(sy);
    }

    // 输出解析结果
    //showdatachar(data, allsymbol, sequencecount, itemsetcount, maxitemsetcountofsequence);



    //return data;
}

void showdatachar(const vector<vector<vector<char>>>& data, 
    const vector<char>& allsymbol, const int& sequencecount, const int& itemsetcount, 
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
    for (Tsymbol sy : allsymbol) {
        cout << sy << " ";
    }
    cout << endl;
    cout << "seq " << sequencecount;
    cout << ", itemcount " << itemsetcount;
    cout << ", maxISofS " << maxitemsetcountofsequence;

}




