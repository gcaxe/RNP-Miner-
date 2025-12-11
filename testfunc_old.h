#pragma once

//未适配int

#include "type_old.h"
#include "RNPfunc_old.h"
#include "loaddata_char.h"

void testgetDataPro();
void showTPos_dict(const TPos_dict& S);
void testvector_intersection();
void testtwo_dimensional_intersection();
void testgetpoRule1();
void showpo(Tpo po);
void testgetpoRule2();




void testgetDataPro() {
	string filename;
	filename = "./datasets/SDB1.txt";
	Tdatabase D, D2;
	vector<char> allsymbol;
	int sequencecount = 0, itemsetcount = 0, maxitemsetcountofsequence = 0;
	readdatachar(filename, D, allsymbol, sequencecount, itemsetcount, maxitemsetcountofsequence);

	TPos_dict S, S2;
	getDataPro(D, S);

    showTPos_dict(S);

    D2 = { 
        {{'a'},{'a','c'}} ,
        {{'b'}},
        {{'a'}}
    };
    getDataPro(D2, S2);

    showTPos_dict(S2);
}

void showTPos_dict(const TPos_dict& S) {
    for (const auto& pair : S) {
        // 输出键和冒号
        std::cout << pair.first << ":" << std::endl;

        for (const auto& vi : pair.second) {
            cout << "[";
            for (int i : vi) {
                cout << i << " ";
            }
            cout << "]";
            cout << "   ";
        }

        // 输出三个空行
        std::cout << "\n\n\n";
    }
}

void testvector_intersection() {
    vector<int> v1{}, v2{ 1,4,7 }, v3{};
    vector<int> v4{ 1,4,7 }, v5{1,2,3,4,5,8}, v6{};
    vector_intersection(v1, v2, v3);
    vector_intersection(v4, v5, v6);

    for (int i : v3) {
        cout << i << " ";
    }
    cout << endl;
    for (int i : v6) {
        cout << i << " ";
    }
}

void testtwo_dimensional_intersection() {
    vector<vector<int>> vv1{ {} }, vv2{ { 1,4,7 } }, vv3{};
    vector<vector<int>> vv4{ {1,4,7}, {3,5,7} }, vv5{ {1,2,3,4,5,8}, {1,7,9} }, vv6{};

    two_dimensional_intersection(vv1, vv2, vv3);
    two_dimensional_intersection(vv4, vv5, vv6);

    cout << "vv3" << endl;
    for (vector<int> vi : vv3) {
        for (int i : vi) {
            cout << i << " ";
        }
        cout << endl;
    }
    cout << endl;
    cout << "vv6" << endl;
    for (vector<int> vi : vv6) {
        for (int i : vi) {
            cout << i << " ";
        }
        cout << endl;
    }
}

void testgetpoRule1() {
    string filename;
    filename = "./datasets/SDB1.txt";
    Tdatabase D;
    vector<char> allsymbol;
    int sequencecount = 0, itemsetcount = 0, maxitemsetcountofsequence = 0;
    readdatachar(filename, D, allsymbol, sequencecount, itemsetcount, maxitemsetcountofsequence);

    TPos_dict S;
    getDataPro(D, S);

    Titemset itemset1{ 'a' }, itemset2{ 'c' }, itemset3{ 'a', 'c' };
    Tpo result1, result2, result3;
    cout << 'a' << endl << endl;
    getpoRule1(itemset1, S, result1);
    //showpo(result1);

    cout << 'c' << endl << endl;
    getpoRule1(itemset2, S, result2);
    //showpo(result2);

    cout << 'a'<<'c' << endl;
    getpoRule1(itemset3, S, result3);
    //showpo(result3);
    for (int i = 0; i < result1.size();i++ ) {
        vector<int> vi1 = result1[i];
        vector<int> vi2 = result2[i];
        vector<int> vi3 = result3[i];
        for (int j : vi1) {
            cout << j << " ";
        }
        cout << "   ";
        for (int j : vi2) {
            cout << j << " ";
        }
        cout << "   ";
        for (int j : vi3) {
            cout << j << " ";
        }
        cout << endl;
    }


}

void showpo(Tpo po) {
    for (vector<int> vi : po) {
        for (int i : vi) {
            cout << i << " ";
        }
        cout << endl;
    }
}

void testgetpoRule2() {

    Tpattern p{ {'a','c'}, {'b'} };

    Tpattern p1{ {'a','c'} }, p2{ {'b'} };
    Tpo po1{ { 3, 6,7,8,9},{1,3,5} }, po2{ {1,2,4,5,8,10},{2,3} };
    map<Tpattern, Tpo> allmappo{ 
        { p1, po1 },
        { p2, po2 }
    };

    Tpo result;
    getpoRule2(p, allmappo, result); //

    showpo(result);
}