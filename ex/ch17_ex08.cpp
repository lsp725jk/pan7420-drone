// ch17_ex08.cpp —— lambda：捕获方式、mutable、泛型 lambda、IIFE
#include <algorithm>
#include <cstdio>
#include <functional>
#include <string>
#include <vector>

int main()
{
    std::printf("--- 1) lambda 的基本结构与捕获方式 ---\n");
    {
        int base = 100;

        auto by_value = [base](int x) { return x + base; };        // 值捕获：拷贝一份
        auto by_ref   = [&base](int x) { return x + base; };       // 引用捕获
        auto by_all_v = [=](int x) { return x + base; };           // 全部按值
        auto by_all_r = [&](int x) { return x + base; };           // 全部按引用
        auto no_cap   = [](int x) { return x * 2; };               // 不捕获

        std::printf("    by_value(1) = %d\n", by_value(1));
        std::printf("    by_ref(1)   = %d\n", by_ref(1));
        std::printf("    by_all_v(1) = %d\n", by_all_v(1));
        std::printf("    by_all_r(1) = %d\n", by_all_r(1));
        std::printf("    no_cap(5)   = %d\n", no_cap(5));

        base = 200;
        std::printf("    base 改成 200 之后：\n");
        std::printf("      by_value(1) = %d（值捕获，还是 101 —— 拷贝是捕获那一刻的）\n", by_value(1));
        std::printf("      by_ref(1)   = %d（引用捕获，跟着变成 201）\n", by_ref(1));
        std::printf("      by_all_v(1) = %d\n", by_all_v(1));
        std::printf("      by_all_r(1) = %d\n", by_all_r(1));
    }

    std::printf("--- 2) 初始化捕获（C++14）：把值搬到 lambda 里 ---\n");
    {
        std::string big = "很长很长的字符串";
        auto owns = [s = std::move(big)]() { return s.size(); };   // 把 big 移进 lambda
        std::printf("    移动捕获后 owns() = %zu，原对象 big = \"%s\"（被搬空）\n",
                    owns(), big.c_str());
        std::printf("    C++14 起可以给捕获起名字、做初始化：\n");
        auto calc = [x = 1 + 2, y = 10]() { return x * y; };
        std::printf("    [x = 1+2, y = 10]() = %d\n", calc());
    }

    std::printf("--- 3) mutable：值捕获的变量默认是 const 的 ---\n");
    {
        int counter = 0;
        auto plain = [counter]() { return counter; };
                // auto bad = [counter]() { ++counter; };   // 编译错误：不能修改 const 的捕获变量
        auto mut = [counter]() mutable { return ++counter; };      // 加 mutable 才能改

        std::printf("    plain()      = %d\n", plain());
        std::printf("    mut()        = %d（第一次）\n", mut());
        std::printf("    mut()        = %d（第二次，lambda 内部自己记着）\n", mut());
        std::printf("    mut()        = %d（第三次）\n", mut());
        std::printf("    外面的 counter 一直是 %d（mutable 改的是 lambda 内部的副本）\n", counter);

        auto ref = [&counter]() { return ++counter; };
        const int got = ref();
        std::printf("    ref()        = %d\n", got);
        std::printf("    外面 counter 现在 = %d（引用捕获才会改到外面）\n", counter);
        std::printf("    注意上面那行的打印顺序：printf 的参数求值顺序不确定，\n");
        std::printf("    所以这里先把 ref() 的结果存进 got 再打印 —— 这是个常见的坑\n");
    }

    std::printf("--- 4) 泛型 lambda（C++14）：参数写 auto ---\n");
    {
        auto print = [](const auto& v) {
            std::printf("      %s\n", std::to_string(v).c_str());
        };
        std::printf("    同一个 lambda 处理多种类型：\n");
        print(42);
        print(3.14);

        auto add = [](auto a, auto b) { return a + b; };
        std::printf("    add(1, 2)       = %d\n", add(1, 2));
        std::printf("    add(1, 2.5)     = %.2f\n", add(1, 2.5));
        std::printf("    泛型 lambda 本质上是一个带 operator() 模板的类\n");
    }

    std::printf("--- 5) lambda 做算法的比较/谓词（最常用的场景）---\n");
    {
        std::vector<int> v{ 5, 2, 8, 1, 9, 3 };
        std::sort(v.begin(), v.end(), [](int a, int b) { return a > b; });
        std::printf("    降序排序：");
        for (int x : v) std::printf("%d ", x);
        std::printf("\n");

        int threshold = 4;
        auto n = std::count_if(v.begin(), v.end(),
                               [threshold](int x) { return x > threshold; });
        std::printf("    大于 %d 的有 %td 个（阈值被捕获进 lambda）\n", threshold, n);

        std::printf("    对比 C 的 qsort：\n");
        std::printf("      qsort(arr, n, sizeof(int), cmp);   // cmp 是独立函数，没法捕获 threshold\n");
        std::printf("      要么用全局变量，要么用 qsort_r 传 context —— lambda 直接解决了这个问题\n");
    }

    std::printf("--- 6) 立即调用的 lambda（IIFE）：初始化复杂对象 ---\n");
    {
        const std::vector<int> data{ 3, 1, 4 };
        const int computed = [&data]() {              // 立刻执行，结果给 computed
            int sum = 0;
            for (int x : data) sum += x * x;
            return sum;
        }();
        std::printf("    IIFE 算出平方和 = %d（computed 可以是 const）\n", computed);
        std::printf("    好处：把\"一串计算\"写成表达式，避免污染外面的命名空间\n");
    }

    std::printf("--- 7) 【重要坑】引用捕获的悬空问题 ---\n");
    {
        std::printf("    危险写法：\n");
        std::printf("      std::function<int()> make() {{\n");
        std::printf("          int local = 42;\n");
        std::printf("          return [&local]() {{ return local; }};   // local 已销毁！\n");
        std::printf("      }}\n");
        std::printf("    [&] 捕获所有局部变量，非常容易踩这个坑\n");
        std::printf("    规则：lambda 活得比被捕获的变量久时，绝不能按引用捕获\n");
        std::printf("    修法：按值捕获（[=] 或 [local]），或者用初始化捕获把东西搬进去\n");

                // 正确写法：按值捕获
        auto make_ok = []() -> std::function<int()> {
            int local = 42;
            return [local]() { return local; };
        };
        std::printf("    正确版本：make_ok()() = %d（按值捕获，安全）\n", make_ok()());
    }

    std::printf("--- 8) 捕获方式的性能影响 ---\n");
    {
        std::string s(1000, 'x');
        auto cap_value = [s]() { return s.size(); };       // 拷贝 1000 字节（可能堆分配）
        auto cap_ref   = [&s]() { return s.size(); };      // 只存一个指针
        auto cap_move  = [s2 = std::move(s)]() { return s2.size(); };

        std::printf("    sizeof(值捕获 lambda)   = %zu（要把整个 string 放进 lambda 对象）\n",
                    sizeof(cap_value));
        std::printf("    sizeof(引用捕获 lambda) = %zu（只存一个指针）\n", sizeof(cap_ref));
        std::printf("    sizeof(移动捕获 lambda) = %zu\n", sizeof(cap_move));
        std::printf("    → 大对象不要按值捕获；[=] 尤其危险（它会悄悄拷贝所有用到的东西）\n");
        std::printf("    C++20 起有 [=, &x] 这类混合写法，但很多团队干脆禁用 [=]\n");
    }

    return 0;
}
