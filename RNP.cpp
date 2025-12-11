//#include "usefullfunc.h"

#include <fstream>
#include "loaddata_char.h"
#include "loaddata_int.h"
#include "RNPfunc_old.h"
//#include "testfunc_old.h"

using namespace std;




int main() {
    string filename;
    filename = "./datasets/SDB1.txt";//64
    //2 3 4 5 6 7 8是数字
    
    filename = "./datasets/SDB2.txt";
    //filename = "./datasets/SDB3.txt";
    //filename = "./datasets/SDB4.txt";
    //filename = "./datasets/SDB5.txt";
    //filename = "./datasets/SDB6.txt";
    //filename = "./datasets/SDB7.txt";
    //filename = "./datasets/SDB8.txt";

    //filename = "./datasets/SDB19.txt";//60


    //vector<vector<vector<char>>> data;
    //vector<char> allsymbol;
    //int sequencecount=0, itemsetcount=0, maxitemsetcountofsequence=0;//必须显式0初始化
    //readdatachar(filename, data, allsymbol, sequencecount, itemsetcount, maxitemsetcountofsequence);
    //showdatachar(data, allsymbol, sequencecount, itemsetcount, maxitemsetcountofsequence);


    vector<vector<vector<int>>> data;
    vector<int> allsymbol;
    int sequencecount=0, itemsetcount=0, maxitemsetcountofsequence=0;//必须显式0初始化
    readdataint(filename, data, allsymbol, sequencecount, itemsetcount, maxitemsetcountofsequence);
    //showdataint(data, allsymbol, sequencecount, itemsetcount, maxitemsetcountofsequence);



    //test
    //testgetDataPro();
    //testvector_intersection();
    //testtwo_dimensional_intersection();
    //testgetpoRule1();
    //testgetpoRule2();


    vector<Tpattern> allfp;
    RNP(data, 3000, allsymbol, allfp);

    showallfp(allfp);


    return 0;
}

