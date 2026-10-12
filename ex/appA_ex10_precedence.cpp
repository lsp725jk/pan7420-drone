// 附录A 探针 10：运算符优先级与结合性——用可观测结果验证"到底怎么结合"
#include <cstdio>

int main() {
    std::puts("=== 1. 位运算低于相等比较（最经典的坑）===");
    {
        int a = 0, b = 1, c = 0;
                // 若按 (a & b) == c 理解 -> (0&1)==0 -> 1
                // 若按 a & (b == c) 理解 -> 0 & (1==0) -> 0
        int r = a & b == c;
        std::printf("  a=0,b=1,c=0;  a & b == c  结果为 %d  -> 实际是 a & (b == c)\n", r);
    }

    std::puts("=== 2. 移位低于加法 ===");
    std::printf("  1 << 2 + 3 = %d   （= 1<<5 = 32，不是 (1<<2)+3 = 7）\n", 1 << 2 + 3);
    std::printf("  (1 << 2) + 3 = %d\n", (1 << 2) + 3);

    std::puts("=== 3. 后缀 ++ 高于一元 * ===");
    {
        int a[3] = {10, 20, 30};
        int* p = a;
        int  x = *p++;      // 等价 *(p++)：先取值 10，再让 p 前进
        std::printf("  *p++ = %d, 之后 *p = %d  -> 是 *(p++)\n", x, *p);
        p = a;
        int  y = (*p)++;    // 先取 10，再把 a[0] 变成 11
        std::printf("  (*p)++ = %d, 之后 a[0] = %d\n", y, a[0]);
    }

    std::puts("=== 4. 赋值与三目：结合方向不同 ===");
    {
        int a, b, c;
        a = b = c = 5;                       // 赋值右结合
        std::printf("  a=b=c=5  ->  a=%d b=%d c=%d\n", a, b, c);
        int t = 0 ? 1 : 2;
        std::printf("  0 ? 1 : 2 = %d\n", t);
    }

    std::puts("=== 5. 逗号运算符优先级最低 ===");
    {
        int a = (1, 2, 3);                   // 逗号表达式取最后一个
        int b;
        b = 1, 2;                            // 等价 (b = 1), 2
        std::printf("  (1,2,3) = %d ;  b = 1, 2 之后 b = %d\n", a, b);
    }

    std::puts("=== 6. 逻辑运算与位运算混用时必须加括号 ===");
    {
        int flags = 0b0110;
                // 想表达"第 1 位或第 2 位置位"
        bool bad  = flags & 0b0010 || flags & 0b0100;   // & 高于 ||，其实是对的
        bool good = (flags & 0b0010) != 0 || (flags & 0b0100) != 0;
        std::printf("  bad=%d good=%d （这里恰好一致，但把 !=0 写出来才不会被误读）\n",
                    (int)bad, (int)good);
    }

    std::puts("=== 7. 复合赋值与自增的组合顺序 ===");
    {
        int a[3] = {1, 2, 3};
        int i = 0;
        a[i] += 10;                          // 等价 a[i] = a[i] + 10
        std::printf("  a[i] += 10 后 a = %d %d %d\n", a[0], a[1], a[2]);
        i = 0;
        a[i++] = 99;                         // 先算下标 0，再让 i 自增
        std::printf("  a[i++] = 99 后 a = %d %d %d, i = %d\n", a[0], a[1], a[2], i);
    }

    std::puts("=== 8. 三目返回左值（C++ 特有）===");
    {
        int x = 1, y = 2;
        (x < y ? x : y) = 100;               // C++ 里合法：C 里不合法
        std::printf("  (x<y ? x : y) = 100  ->  x=%d y=%d\n", x, y);
    }

    std::puts("=== 9. 关系运算不可连写 ===");
    {
        int a = 3;
        std::printf("  a=3: (1 < a < 5) = %d   （先算 1<3 得 1，再算 1<5 得 1）\n", (1 < a < 5));
        std::printf("  正确写法 (1 < a && a < 5) = %d\n", (1 < a && a < 5));
    }
    return 0;
}
