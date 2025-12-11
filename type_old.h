#pragma once

#include <vector>
#include <map>

using namespace std;

typedef int Tsymbol; //项的类型是符号
typedef vector<Tsymbol> Titemset; //项集
typedef vector<vector<Tsymbol>> Tpattern;//pattern模式
typedef vector<vector<vector<Tsymbol>>> Tdatabase; //数据库
typedef map<Tsymbol, vector<vector<int>>> Tpos_dict; //位置字典
typedef vector<vector<int>> Tpo;//po出现位置