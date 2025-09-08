/*
给定三根柱子，记为 A、B 和 C 。起始状态下，柱子 A 上套着 
 个圆盘，它们从上到下按照从小到大的顺序排列。我们的任务是要把这 
 个圆盘移到柱子 C 上，并保持它们的原有顺序不变（如图 12-10 所示）。在移动圆盘的过程中，需要遵守以下规则。

圆盘只能从一根柱子顶部拿出，从另一根柱子顶部放入。
每次只能移动一个圆盘。
小圆盘必须时刻位于大圆盘之上。
*/

#include <format>
#include <vector>
#include <string>
#include "../util/util.h"

namespace hanota {

struct MyVec{
public:
    std::vector<int> vec_;
    std::string name_;

    MyVec(std::string name, std::vector<int>& vec): name_(std::move(name)), vec_(std::move(vec)){}

    void push_back(const int & val){
        return vec_.push_back(val);
    }

    void pop_back(){
        return vec_.pop_back();
    }

    auto back(){
        return vec_.back();
    }

    [[nodiscard]]auto size() const{
        return vec_.size();
    }

    auto& name(){
        return name_;
    }

    std::string to_string(){
        std::string s = std::format("{}: ", name_);
        for(const auto& num : vec_){
            s +=  std::to_string(num) + ",";
        }
        s.pop_back();
        return s;
    }
};

using vec = MyVec;

void move(vec& A, vec& B){
    static int cnt = 0;
    auto val = A.back();
    A.pop_back();
    B.push_back(val);
    ++cnt;
    log("cnt: {}: {} -> {}: {}", cnt, A.name(), B.name(), val);
}

void solve(vec& A, vec& B, vec& C, int n){
    if(n == 1){
        move(A, C);
        return;
    }
    solve(A, C, B, n - 1);
    move(A, C);
    solve(B, A, C, n-1);
}

void hanota(vec& A, vec& B, vec& C){
    solve(A, B, C, static_cast<int>(A.size()));
}

void test(){
    std::vector<int> A{6,5,4,3,2,1}, B{}, C{};
    vec MyA("A", A), MyB("B", B), MyC("C", C);
    hanota(MyA, MyB, MyC);
    log("final res:\n{}\n{}\n{}", MyA.to_string(), MyB.to_string(), MyC.to_string());
}

}