// ch22_ex04.cpp —— 例 22-4：模板是"懒"的，只实例化你真正用到的东西
// 这一点非常重要：它让你可以在一个类模板里写一堆"只对某些类型成立"的函数，
// 只要不用，就不会报错。反过来，错误也会被拖到很晚才暴露。
#include <cstdio>
#include <type_traits>
#include <typeinfo>

template <typename T>
struct Reporter {
    void fine() {
        T v{};
        std::printf("fine(): T = %s\n", typeid(v).name());
    }

        // 下面这个函数体对 T=Widget 根本编不过（Widget 里没有 no_such_thing），
        // 但它是模板的成员函数，只有被调用时才会真正生成代码。
    void broken() {
        T::no_such_thing();
    }

        // static_assert 也一样：依赖模板参数时，只在实例化那一刻才检查
    void only_for_int() {
        static_assert(std::is_same<T, int>::value, "only_for_int 只支持 int");
        std::printf("only_for_int(): T 是 int\n");
    }
};

struct Widget {
    int a = 1;
};

int main() {
    Reporter<Widget> rw;
    rw.fine();  // OK

        // rw.broken();        // 一旦取消注释：error C2039: "no_such_thing" 不是 "Widget" 的成员
        // rw.only_for_int();  // 一旦取消注释：error C2338: only_for_int 只支持 int
    std::printf("Reporter<Widget> 里 broken()/only_for_int() 都没被调用 -> 不实例化 -> 不报错\n");

    Reporter<int> ri;
    ri.only_for_int();  // T=int，static_assert 通过

        // 注意：显式实例化（下面这行取消注释）会把**所有成员函数**都生成一遍，
        // 于是 broken()/only_for_int() 立刻变成错误。
        // template struct Reporter<Widget>;
    std::printf("sizeof(Reporter<Widget>)=%zu（空类，1 字节）\n", sizeof(rw));
    return 0;
}
