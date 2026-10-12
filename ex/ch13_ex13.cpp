// ch13_ex13.cpp —— 虚继承：解决菱形继承的二义性
// 编译：check.bat ch13_ex13.cpp c++latest
#include <cstdio>

class Base {
public:
    int value = 1;
};

class A : virtual public Base { };     // 虚继承：共享同一个 Base 子对象
class B : virtual public Base { };

class C : public A, public B { };      // 现在 C 里只有一份 Base

int main()
{
    C c;

        // 1) 不再有二义性：value 只有一份
    std::printf("1) c.value = %d（不用写 c.A::value 了）\n", c.value);

        // 2) 证明只有一份：改一处，处处都变
    c.value = 100;
    std::printf("2) 改 c.value 之后: c.A::value=%d  c.B::value=%d（同一份数据）\n",
                c.A::value, c.B::value);

        // 3) 尺寸：虚继承要额外存"虚基类偏移"，所以对象会变大
    std::printf("3) sizeof(Base)=%llu sizeof(A)=%llu sizeof(C)=%llu\n",
                (unsigned long long)sizeof(Base),
                (unsigned long long)sizeof(A),
                (unsigned long long)sizeof(C));
    std::printf("   对照 ch13_ex12.cpp（非虚继承）看尺寸差别\n");

        // 4) 代价与建议
    std::printf("4) 虚继承的代价：\n");
    std::printf("   - 对象更大（虚基类偏移指针）\n");
    std::printf("   - 最派生类必须负责初始化虚基类（构造函数规则更绕）\n");
    std::printf("   - 访问虚基类成员多一层间接\n");
    std::printf("5) 建议：\n");
    std::printf("   - 现实中菱形继承很少见，需要多继承时几乎都是继承多个接口（ch13_ex11）\n");
    std::printf("   - 能不用虚继承就不用；用的时候尽量只用于接口基类\n");
    return 0;
}
