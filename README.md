# RNP-Miner-

2025/12/3<br>
RNP-Miner复现，c++实现。原文《RNP-Miner: Repetitive Nonoverlapping Sequential Pattern Mining》，优先参考论文，其次参考原作者的python代码 https://github.com/wuc567/Pattern-Mining/tree/master/RNP-Miner

上传datasets文件夹，RNP.cpp, RNPfunc_old.h, RNPfunc_bit.h, loaddata_char.h, loaddata_int.h, type_bit.h, type_old.h。<br>
_old表示按照原有论文复现，_bit表示尝试修改为二进制位图实现。（二进制位图实现并不完整，可以在编译时排除_bit的文件）<br>
我的环境是vs2022，需要在与RNP.cpp同一级的目录下，有datasets文件夹。

<br>
<br>
<br>

2026/4/29<br>
上传cpp文件夹，这是第二种复现，代码是纯ai生成的<br>
环境vscode，g++，claude code插件，deepseek v4模型，包括运行配置在内的全部文件。要求“cpp”文件夹与“datasets”文件夹在同一级目录下


运行示例：ctrl+shift+b

在终端输入

cd 略/cpp<br>
./rnp_miner_main.exe rnp-miner <br>
../datasets/SDB1.txt 3000
