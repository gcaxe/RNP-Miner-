#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
//#include <bitset>
#include <cstdint>

using namespace std;



struct  Tuint192 {
    int64_t x;
    int64_t y;
    int64_t z;
};

struct Tvui64 {
    vector<uint64_t> vi64;
    int length;
};

struct Tvui192 {
    vector<Tuint192> vi192;
    int length;
};

/*
struct alignas(24) Tuint192 { //不能是24，必须2^N
    int64_t x;
    int64_t y;
    int64_t z;
};
*/

/*
struct __align__(24) Tint192 {  // 192位=24字节
    int64_t x, y, z;
};


__global__ void kernel_aligned(AlignedData *data) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    data[idx].x = idx;
    data[idx].y = idx * 2;
    data[idx].z = idx * 3;
}
*/

//vector<bool>的坑
// 默认构造的 vector<bool>不会初始化内存，不一定是0还是1。vector<bool>的底层不是连续内存布局
// 某些 STL 算法可能无法正确处理 vector<bool>的代理对象
// // 读写需要位掩码操作，比直接访问 bool变量慢。比特压缩可能导致缓存不友好。不如vector<uint8_t>或者vector<char>
// 多线程环境下同时修改不同位置的 bool值可能引发数据竞争（因为多个bool可能共享同一个字节）
// 不满足标准容器的所有要求（如无法获取 bool& 引用)

// 
//候选者vector<bool> vector<char>=vector<uint8_t> uint64_t Tbitarr vector<uint64_t> vector<Tuint192> Tvui64 Tvui192;
//不能同时重载char和uint8
typedef vector<char> Tbitarr;

typedef char Tsymbol; //符号的类型是char或int16_t
typedef vector<Tsymbol> Titemset; //项集
typedef vector<vector<Tsymbol>> Tpattern;//pattern模式
typedef vector<vector<vector<Tsymbol>>> Tdatabase; //数据库
//typedef map<Tsymbol, vector<Tbitarr>> Tpos_dict; //位置字典
typedef unordered_map<Tsymbol, vector<Tbitarr>> Tpos_dict; //位置字典
typedef vector<Tbitarr> Tpo; //Po





// 移位操作符重载声明
vector<bool>& operator>>(vector<bool>& vec, int k);
vector<bool>& operator>>(vector<uint8_t>& vec, int k);
vector<uint64_t>& operator>>(vector<uint64_t>& vec, int k);
vector<Tuint192>& operator>>(vector<Tuint192>& vec, int k);

vector<bool>& operator<<(vector<bool>& vec, int k);
vector<bool>& operator<<(vector<uint8_t>& vec, int k);
vector<uint64_t>& operator<<(vector<uint64_t>& vec, int k);
vector<Tuint192>& operator<<(vector<Tuint192>& vec, int k);

// 辅助函数声明
void shiftright64(vector<Tuint192>& vec, int b);
void shiftleft64(vector<Tuint192>& vec, int b);

// 其他位运算符重载声明
vector<bool> operator&(const vector<bool>& v1, const vector<bool>& v2);
vector<bool> operator|(const vector<bool>& v1, const vector<bool>& v2);
vector<bool> operator^(const vector<bool>& v1, const vector<bool>& v2);
vector<bool> operator~(const vector<bool>& v1);

vector<bool> operator&(const vector<uint8_t>& v1, const vector<uint8_t>& v2);
vector<bool> operator|(const vector<uint8_t>& v1, const vector<uint8_t>& v2);
vector<bool> operator^(const vector<uint8_t>& v1, const vector<uint8_t>& v2);
vector<bool> operator~(const vector<uint8_t>& v1);

vector<uint64_t> operator&(const vector<uint64_t>& v1, const vector<uint64_t>& v2);
vector<uint64_t> operator|(const vector<uint64_t>& v1, const vector<uint64_t>& v2);
vector<uint64_t> operator^(const vector<uint64_t>& v1, const vector<uint64_t>& v2);
vector<uint64_t> operator~(const vector<uint64_t>& v1);

vector<Tuint192> operator&(const vector<Tuint192>& v1, const vector<Tuint192>& v2);
vector<Tuint192> operator|(const vector<Tuint192>& v1, const vector<Tuint192>& v2);
vector<Tuint192> operator^(const vector<Tuint192>& v1, const vector<Tuint192>& v2);
vector<Tuint192> operator~(const vector<Tuint192>& v1);



//全按照int的右移来，右移1位，最低位没有了

vector<bool>& operator>>(vector<bool>& vec, int k) {
    //实际上反而应当左移
    if (k == 0) return vec;  // 不移
    else if (k < 0) return vec << k;  //反向移动
    else if (k >= vec.size()) {
        vec.assign(vec.size(), false);  // 全部移出，补0
        return vec;
    }

    else {
        // 将元素向左移动k位
        std::copy(vec.begin() + k, vec.end(), vec.begin());
        //OutputIt copy( InputIt first, InputIt last, OutputIt d_first );copy不能处理某些重叠
        // 右侧补0
        std::fill(vec.end() - k, vec.end(), false);

        return vec;
    }
}

vector<char>& operator>>(vector<char>& vec, int k) {
    //实际上反而应当左移
    if (k == 0) return vec;  // 不移
    else if (k < 0) return vec << k;  //反向移动
    else if (k >= vec.size()) {
        vec.assign(vec.size(), 0);  // 全部移出，补0
        return vec;
    }

    else {
        // 将元素向左移动k位
        std::copy(vec.begin() + k, vec.end(), vec.begin());
        //OutputIt copy( InputIt first, InputIt last, OutputIt d_first );copy不能处理某些重叠
        // 右侧补0
        std::fill(vec.end() - k, vec.end(), 0);

        return vec;
    }
}

vector<uint64_t>& operator>>(vector<uint64_t>& vec, int k) {
    //gpu上最好少使用if分支，之后再优化吧
    if (k == 0) return vec;
    else if (k < 0) return vec << (-k);
    else if (k >= 64 * vec.size()) {
        vec.assign(vec.size(), 0);  // 全部移出，补0
        return vec;
    }
    else if (k == 1) {
        //uint64_t mask = 1;
        uint64_t carry = 0;
        for (int i = vec.size() - 1; i >= 0; i--) {
            uint64_t new_carry = vec[i] & 1;  // 保存当前元素的LSB
            vec[i] = vec[i] >> 1;            // 右移当前元素
            vec[i] |= (carry << 63);          // 将前一个元素的LSB作为当前元素的MSB
            carry = new_carry;                // 更新carry
        }
        return vec;
    }
    else {
        // 以3 int4为例   进入的顺序如下    4321 8765 xxx9---1011 0100 0001
        //如果位移量小于4
        //如果位移量=4*1+2
        unsigned int a = k / 64, b = k % 64;  // k=64*a+b, 之前的if保证了a < vec.size()

        if (a > 0) {
            // 将元素向左移动k位
            copy(vec.begin() + a, vec.end(), vec.begin());
            // 右侧补0
            fill(vec.end() - a, vec.end(), 0);
        }

        if (b > 0) {
            uint64_t mask = (1ULL << b) - 1;
            uint64_t carry = 0;
            for (int i = vec.size() - 1; i >= 0; i--) {
                uint64_t new_carry = vec[i] & mask;  // 保存当前元素的LSB
                vec[i] = vec[i] >> b;            // 右移当前元素
                vec[i] |= (carry << (64 - b));          // 将前一个元素的LSB作为当前元素的MSB
                carry = new_carry;                // 更新carry
            }
        }

        return vec;
    }
}

vector<Tuint192>& operator>>(vector<Tuint192>& vec, int k) {
    if (k == 0) return vec;
    else if (k < 0) return vec << (-k);
    else if (k >= 64 * vec.size()) {
        vec.assign(vec.size(), {0,0,0});  // 全部移出，补0
        return vec;
    }

    else {
        // 计算需要移动的完整192位块数和剩余的位数
        unsigned int a = k / 192, b = k % 192;

        if (a > 0) {
            // 将元素向左移动a个块
            copy(vec.begin() + a, vec.end(), vec.begin());
            // 右侧补0
            fill(vec.end() - a, vec.end(), Tuint192{ 0, 0, 0 });
        }

        if (b > 0) {
            if (b < 64) {
                shiftright64(vec, b);
                /*
                // 位移量小于64位，只需要处理相邻两个64位整数
                uint64_t mask = (1ULL << b) - 1;
                uint64_t carry = 0;
                for (int i = vec.size() - 1; i >= 0; i--) {

                    uint64_t new_carry = vec[i].z & mask;
                    vec[i].z = vec[i].z >> b;
                    vec[i].z |= (carry << (64 - b));
                    carry = new_carry;

                    new_carry = vec[i].y & mask;
                    vec[i].y = vec[i].y >> b;
                    vec[i].y |= (carry << (64 - b));

                    new_carry = vec[i].x & mask;
                    vec[i].x = vec[i].x >> b;
                    vec[i].x |= (carry << (64 - b));

                    carry = new_carry;
                }
                */
            }
            else if (b < 128) {
                // 位移量在64-128位之间

                for (size_t i = vec.size() - 1; i > 0; --i) {
                    vec[i].z = vec[i].y;
                    vec[i].y = vec[i].x;
                    vec[i].x = vec[i - 1].z;
                }

                // 处理第一个元素
                vec[0].z = vec[0].y;
                vec[0].y = vec[0].x;
                vec[0].x = 0;

                shiftright64(vec, b-64);

            }
            else {
                // 位移量在128-192位之间
                for (size_t i = vec.size() - 1; i > 0; --i) {
                    vec[i].z = vec[i].x;
                    vec[i].y = vec[i - 1].z;
                    vec[i].x = vec[i - 1].y;
                }

                // 处理第一个元素
                vec[0].z = vec[0].x;
                vec[0].y = 0;
                vec[0].x = 0;

                shiftright64(vec, b - 128);
            }
        }

        return vec;
    }

}

inline void shiftright64(vector<Tuint192>& vec, int b) {
    // 位移量小于64位，只需要处理相邻两个64位整数
    uint64_t mask = (1ULL << b) - 1;
    uint64_t carry = 0;
    for (int i = vec.size() - 1; i >= 0; i--) {

        uint64_t new_carry = vec[i].z & mask;
        vec[i].z = vec[i].z >> b;
        vec[i].z |= (carry << (64 - b));
        carry = new_carry;

        new_carry = vec[i].y & mask;
        vec[i].y = vec[i].y >> b;
        vec[i].y |= (carry << (64 - b));

        new_carry = vec[i].x & mask;
        vec[i].x = vec[i].x >> b;
        vec[i].x |= (carry << (64 - b));

        carry = new_carry;
    }
    //return vec;
}

//Tvui64& operator>>(Tvui64& vec, int k) {}
//Tvui64& operator>>(Tvui192& vec, int k) {}



vector<bool>& operator<<(vector<bool>& vec, int k) {
    //按照int来，应该写右移
    if (k == 0) return vec;
    else if (k < 0) return vec >> (-k);
    else if (k >= vec.size()) {
        vec.assign(vec.size(), false);
        return vec;
    }
    else {
        std::copy_backward(vec.begin() , vec.end()-k, vec.begin()+k);
        //OutputIt copy_backward( InputIt first, InputIt last, OutputIt d_first );copy_backward可以处理这种重叠
        std::fill(vec.end() , vec.end()-k, false);
        return vec;
    }
}

vector<char>& operator<<(vector<char>& vec, int k) {
    //按照int来，应该写右移
    if (k == 0) return vec;
    else if (k < 0) return vec >> (-k);
    else if (k >= vec.size()) {
        vec.assign(vec.size(), 0);
        return vec;
    }
    else {
        std::copy_backward(vec.begin(), vec.end() - k, vec.begin() + k);
        //OutputIt copy_backward( InputIt first, InputIt last, OutputIt d_first );copy_backward可以处理这种重叠
        std::fill(vec.end(), vec.end() - k, 0);
        return vec;
    }
}

vector<uint64_t>& operator<<(vector<uint64_t>& vec, int k) {
    if (k == 0) return vec;
    else if (k < 0) return vec >> (-k);
    else if (k >= 64 * vec.size()) {
        vec.assign(vec.size(), 0);
        return vec;
    }
    else if (k == 1) {
        uint64_t carry = 0;
        for (size_t i = 0; i < vec.size(); i++) {
            uint64_t new_carry = vec[i] >> 63;
            vec[i] = vec[i] << 1;
            vec[i] |= carry;
            carry = new_carry;
        }
        return vec;
    }
    else {
        unsigned int a = k / 64, b = k % 64;

        if (a > 0) {
            std::copy_backward(vec.begin(), vec.end() - a, vec.end());
            std::fill(vec.begin(), vec.begin() + a, 0);
        }

        if (b > 0) {
            uint64_t mask = ~((1ULL << (64 - b)) - 1);
            uint64_t carry = 0;
            for (size_t i = 0; i < vec.size(); i++) {
                uint64_t new_carry = (vec[i] & mask) >> (64 - b);
                vec[i] = vec[i] << b;
                vec[i] |= carry;
                carry = new_carry;
            }
        }

        return vec;
    }
}

vector<Tuint192>& operator<<(vector<Tuint192>& vec, int k) {
    if (k == 0) return vec;
    else if (k < 0) return vec >> (-k);
    else if (k >= 192 * vec.size()) {
        vec.assign(vec.size(), { 0, 0, 0 });
        return vec;
    }
    else {
        unsigned int a = k / 192, b = k % 192;

        if (a > 0) {
            std::copy_backward(vec.begin(), vec.end() - a, vec.end());
            std::fill(vec.begin(), vec.begin() + a, Tuint192{ 0, 0, 0 });
        }

        if (b > 0) {
            if (b < 64) {
                shiftleft64(vec, b);
            }
            else if (b < 128) {
                shiftleft64(vec, b - 64);

                for (size_t i = 0; i < vec.size() - 1; ++i) {
                    vec[i].x = vec[i].y;
                    vec[i].y = vec[i].z;
                    vec[i].z = vec[i + 1].x;
                }

                vec.back().x = vec.back().y;
                vec.back().y = vec.back().z;
                vec.back().z = 0;
            }
            else {
                shiftleft64(vec, b - 128);

                for (size_t i = 0; i < vec.size() - 1; ++i) {
                    vec[i].x = vec[i].z;
                    vec[i].y = vec[i + 1].x;
                    vec[i].z = vec[i + 1].y;
                }

                vec.back().x = vec.back().z;
                vec.back().y = 0;
                vec.back().z = 0;
            }
        }

        return vec;
    }
}

inline void shiftleft64(vector<Tuint192>& vec, int b) {
    uint64_t mask = ~((1ULL << (64 - b)) - 1);
    uint64_t carry = 0;
    for (size_t i = 0; i < vec.size(); i++) {
        uint64_t new_carry = (vec[i].x & mask) >> (64 - b);
        vec[i].x = vec[i].x << b;
        vec[i].x |= carry;

        carry = (vec[i].y & mask) >> (64 - b);
        vec[i].y = vec[i].y << b;
        vec[i].y |= new_carry;

        new_carry = (vec[i].z & mask) >> (64 - b);
        vec[i].z = vec[i].z << b;
        vec[i].z |= carry;

        carry = new_carry;
    }
}



vector<bool> operator&(const vector<bool>& v1, const vector<bool>& v2) {
    if ( v1.size() != v2.size() ) {
        cerr << "v1&v2 失败，长度不等";
    }
    int n = v1.size();
    vector<bool> v3(n);
    for (int i = 0; i < n; i++) {
        v3[i] = v1[i] && v2[i];
    }
    return v3;
}
vector<bool> operator|(const vector<bool>& v1, const vector<bool>& v2) {
    if (v1.size() != v2.size()) {
        cerr << "v1|v2 失败，长度不等";
    }
    int n = v1.size();
    vector<bool> v3(n);
    for (int i = 0; i < n; i++) {
        v3[i] = v1[i] || v2[i];
    }
    return v3;
}
vector<bool> operator^(const vector<bool>& v1, const vector<bool>& v2) {
    if (v1.size() != v2.size()) {
        cerr << "v1^v2 失败，长度不等";
    }
    int n = v1.size();
    vector<bool> v3(n);
    for (int i = 0; i < n; i++) {
        v3[i] = v1[i] ^ v2[i];
    }
    return v3;
}
vector<bool> operator~(const vector<bool>& v1) {

    int n = v1.size();
    vector<bool> v3(n);
    for (int i = 0; i < n; i++) {
        v3[i] = !v1[i] ;
    }
    return v3;
}

vector<char> operator&(const vector<char>& v1, const vector<char>& v2) {
    if (v1.size() != v2.size()) {
        cerr << "v1&v2 失败，长度不等";
    }
    int n = v1.size();
    vector<char> v3(n);
    for (int i = 0; i < n; i++) {
        v3[i] = v1[i] & v2[i];
    }
    return v3;
}
vector<char> operator|(const vector<char>& v1, const vector<char>& v2) {
    if (v1.size() != v2.size()) {
        cerr << "v1|v2 失败，长度不等";
    }
    int n = v1.size();
    vector<char> v3(n);
    for (int i = 0; i < n; i++) {
        v3[i] = v1[i] | v2[i];
    }
    return v3;
}
vector<char> operator^(const vector<char>& v1, const vector<char>& v2) {
    if (v1.size() != v2.size()) {
        cerr << "v1^v2 失败，长度不等";
    }
    int n = v1.size();
    vector<char> v3(n);
    for (int i = 0; i < n; i++) {
        v3[i] = v1[i] ^ v2[i];
    }
    return v3;
}
vector<char> operator~(const vector<char>& v1) {

    int n = v1.size();
    vector<char> v3(n);
    for (int i = 0; i < n; i++) {
        v3[i] = 1-v1[i];
    }
    return v3;
}

vector<uint64_t> operator&(const vector<uint64_t>& v1, const vector<uint64_t>& v2) {
    if (v1.size() != v2.size()) {
        cerr << "v1&v2 失败，长度不等";
    }
    int n = v1.size();
    vector<uint64_t> v3(n);
    for (int i = 0; i < n; i++) {
        v3[i] = v1[i] & v2[i];
    }
    return v3;
}
vector<uint64_t> operator|(const vector<uint64_t>& v1, const vector<uint64_t>& v2) {
    if (v1.size() != v2.size()) {
        cerr << "v1|v2 失败，长度不等";
    }
    int n = v1.size();
    vector<uint64_t> v3(n);
    for (int i = 0; i < n; i++) {
        v3[i] = v1[i] | v2[i];
    }
    return v3;
}
vector<uint64_t> operator^(const vector<uint64_t>& v1, const vector<uint64_t>& v2) {
    if (v1.size() != v2.size()) {
        cerr << "v1^v2 失败，长度不等";
    }
    int n = v1.size();
    vector<uint64_t> v3(n);
    for (int i = 0; i < n; i++) {
        v3[i] = v1[i] ^ v2[i];
    }
    return v3;
}
vector<uint64_t> operator~(const vector<uint64_t>& v1) {

    int n = v1.size();
    vector<uint64_t> v3(n);
    for (int i = 0; i < n; i++) {
        v3[i] = ~v1[i];
    }
    return v3;
}

vector<Tuint192> operator&(const vector<Tuint192>& v1, const vector<Tuint192>& v2) {
    if (v1.size() != v2.size()) {
        cerr << "v1&v2 失败，长度不等";
    }
    int n = v1.size();
    vector<Tuint192> v3(n);
    for (int i = 0; i < n; i++) {
        v3[i] = { v1[i].x & v2[i].x,  v1[i].y & v2[i].y,  v1[i].z & v2[i].z };
    }
    return v3;
}
vector<Tuint192> operator|(const vector<Tuint192>& v1, const vector<Tuint192>& v2) {
    if (v1.size() != v2.size()) {
        cerr << "v1|v2 失败，长度不等";
    }
    int n = v1.size();
    vector<Tuint192> v3(n);
    for (int i = 0; i < n; i++) {
        v3[i] = { v1[i].x | v2[i].x,  v1[i].y | v2[i].y,  v1[i].z | v2[i].z };
    }
    return v3;
}
vector<Tuint192> operator^(const vector<Tuint192>& v1, const vector<Tuint192>& v2) {
    if (v1.size() != v2.size()) {
        cerr << "v1^v2 失败，长度不等";
    }
    int n = v1.size();
    vector<Tuint192> v3(n);
    for (int i = 0; i < n; i++) {
        v3[i] = { v1[i].x ^ v2[i].x,  v1[i].y ^ v2[i].y,  v1[i].z ^ v2[i].z };
    }
    return v3;
}
vector<Tuint192> operator~(const vector<Tuint192>& v1) {

    int n = v1.size();
    vector<Tuint192> v3(n);
    for (int i = 0; i < n; i++) {
        v3[i] = { ~v1[i].x ,  ~v1[i].y ,  ~v1[i].z };
    }
    return v3;
}






Tuint192 operator&=(Tuint192& a, const Tuint192& b) {
    a.x &= b.x;
    a.y &= b.y;
    a.z &= b.z;
    return a;
}



/*
Tbitarr& operator>>(Tbitarr& vec, int k) {
    constexpr bool isTbitarr_vb = std::is_same_v<Tbitarr, std::vector<bool>>;
    constexpr bool isTbitarr_ui64 = std::is_same_v<Tbitarr, uint64_t>;//可能最佳
    constexpr bool isTbitarr_ui32 = std::is_same_v<Tbitarr, uint32_t>;
    constexpr bool isTbitarr_vi64 = std::is_same_v<Tbitarr, vector<uint64_t>>;
    constexpr bool isTbitarr_vi192 = std::is_same_v<Tbitarr, vector<Tuint192>>;
    constexpr bool isTbitarr_Tvui64 = std::is_same_v<Tbitarr, Tvui64>;
    constexpr bool isTbitarr_Tvui192 = std::is_same_v<Tbitarr, Tvui192>;
    
    //constexpr bool isTbitarr_bs64 = std::is_same_v<Tbitarr, bitset<64>>;//CUDA的thrust没有bitset
    //constexpr bool isTbitarr_bs128 = std::is_same_v<Tbitarr, bitset<128>>;
    //constexpr bool isTbitarr_bs256 = std::is_same_v<Tbitarr, bitset<256>>;

    if constexpr (isTbitarr_vb) {//编译期做判断
        //vector<bool>版本
        if (k == 0) return vec;  // 不移
        else if (k < 0) return vec << k;  //反向移动
        else if (k >= vec.size()) {
            vec.assign(vec.size(), false);  // 全部移出，补0
            return vec;
        }
        else {
            // 将元素向右移动k位
            //小 k 用 copy+fill，大 k 用 fill+rotate
            std::copy(vec.begin(), vec.end() - k, vec.begin() + k); //OutputIt copy( InputIt first, InputIt last, OutputIt d_first );
            // 左侧补0
            std::fill(vec.begin(), vec.begin() + k, false);

            return vec;
        }
    }

    else if constexpr (isTbitarr_ui64 || isTbitarr_ui32) {
        // uint64_t / uint32_t 版本
        if (k == 0) return vec;
        else if (k < 0) return vec << (-k);
        else {
            return vec >> k; //没问题，不要写>>=
        }
    }

    else if constexpr (isTbitarr_vi64) {
        //vector<uint64_t>版本
        // 以3 int4为例   进入的顺序如下    4321 8765 xxx9
        if (k == 0) return vec;
        else if (k < 0) return vec << (-k);
        else if (k >= 64*vec.size()) {
            vec.assign(vec.size(), 0);  // 全部移出，补0
            return vec;
        }
        else {

            //没写完！！！！！！

            return vec;
        }
    }

    else if constexpr (isTbitarr_vi192) {}

    else if constexpr (isTbitarr_Tvui64) {
        //Tvui64版本
        // 以3 int4为例   进入的顺序如下    1234 5678 9xxx
        Tvui64 result = static_cast<Tvui64>(vec);
        if (k == 0) return vec;
        else if (k < 0) return vec << (-k);
        //constexpr Tvui64 forward<T>(vec);
        else if (k >= result.length) {
            vec.assign(vec.size(), 0);  // 全部移出，补0
            return vec;
        }
        else {

            //没写完！！！！！！

            return vec;
        }
    }
    else if constexpr (isTbitarr_Tvui192) {}

    else {
        // 不支持的类型，编译时静态断言
        static_assert(
            isTbitarr_vb || isTbitarr_ui64 || isTbitarr_ui32,
            "Tbitarr must be std::vector<bool>, uint64_t, or uint32_t"
            );
        return vec;  // 避免编译警告（实际不会执行）
    }
}
*/



