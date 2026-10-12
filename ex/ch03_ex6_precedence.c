/**
 * ch03_ex6_precedence.c —— 优先级、结合性与常见的"看起来对其实错"
 */
#include <stdio.h>

int main(void)
{
                /**
         * ---- 1. 经典错误：位运算和比较的优先级 ----
         */
    printf("---- 1. 位与 vs 相等 ----\n");
    int flags = 0x04;
    int MASK  = 0x04;
        /* 下面两行是**故意**写成有歧义的样子，用来演示编译器怎么理解。 MSVC /W4 会报 warning C4554（检查运算符优先级可能存在的错误）， 这正是编译器在提醒你"这里最好加括号"。为了输出干净，这里压掉它； 你自己的代码遇到 C4554，应该去加括号，而不是压警告。 */
#pragma warning(push)
#pragma warning(disable : 4554)
    printf("flags & MASK == 0   -> %d\n", flags & MASK == 0);
                /* 实际被解析成 flags & (MASK == 0) = 0x04 & 0 = 0 */
    printf("  上面等于 flags & (MASK==0) = %d\n", flags & (MASK == 0));
    printf("正确写法 (flags & MASK) == 0 -> %d\n", (flags & MASK) == 0);
    printf("判断某位非 0 更该写 if (flags & MASK)  -> %d\n", (flags & MASK) != 0);

                /* ---- 2. 移位和算术的优先级 ---- */
    printf("---- 2. 移位 vs 加减 ----\n");
    printf("1 << 2 + 1  = %d   <-- + 比 << 优先级高，等于 1<<3 = 8\n",
           1 << 2 + 1);
#pragma warning(pop)
    printf("(1 << 2) + 1 = %d  <-- 想要 5 就得加括号\n", (1 << 2) + 1);
    printf("嵌入式里写波特率分频、地址计算时，这里最容易错。\n");

                /* ---- 3. 结合性：a - b - c 和 a = b = c ---- */
    printf("---- 3. 结合性 ----\n");
    int a2 = 10, b2 = 3, c2 = 2;
    printf("10 - 3 - 2   = %d   (左结合：(10-3)-2)\n", a2 - b2 - c2);
    printf("10 - (3 - 2) = %d   (加了括号就完全不同)\n", a2 - (b2 - c2));
    int p, q, r2;
    p = q = r2 = 7;                             /* 右结合：p = (q = (r2 = 7)) */
    printf("p = q = r = 7  -> p=%d q=%d r=%d\n", p, q, r2);
    printf("赋值表达式的值就是被赋的值，所以能这样链式写。\n");

                /* ---- 4. 指针与自增：*p++ 到底是什么意思 ---- */
    printf("---- 4. *p++ vs (*p)++ vs ++*p ----\n");
    int arr[3] = { 100, 200, 300 };
    int *pt = arr;
    printf("初始 *pt = %d\n", *pt);
    printf("*pt++ 的值 = %d  <-- 后缀 ++ 优先级高于 *，等价 *(pt++)\n", *pt++);
    printf("  取完值后 pt 指向 arr[1]，现在 *pt = %d\n", *pt);
    printf("(*pt)++ 会先取值再让**那个元素**加 1：\n");
    printf("  执行前 arr[1] = %d\n", arr[1]);
    int old = (*pt)++;                           /* 拆成两句，避免实参求值顺序带来的困惑 */
    printf("  (*pt)++ 的\"值\" = %d（是加之前的旧值）\n", old);
    printf("  执行后 arr[1] = %d\n", arr[1]);
    pt = arr;                                    /* 复位 */
    printf("++*pt 等价 ++(*pt)：先把元素加 1 再取值 = %d, arr[0]=%d\n",
           ++*pt, arr[0]);
    printf("记忆：后缀 ++ 只跟最近的那个东西结合；想改元素值必须加括号。\n");

                /* ---- 5. 结构体成员与指针：*p.x 是错的 ---- */
    printf("---- 5. . 和 -> 的优先级 ----\n");
    struct point { int x; int y; };
    struct point pt2 = { 3, 4 };
    struct point *pp = &pt2;
                /* . 的优先级比 * 高，所以 *pp.x 会被解析成 *(pp.x)，直接编译错误： error C2224: “.x”左边的操作数必须具有结构/联合类型 正确写法是 (*pp).x，或者用语法糖 pp->x。 */
    printf("(*pp).x = %d   pp->x = %d   （两种写法完全等价）\n",
           (*pp).x, pp->x);
    printf("-> 就是为省那一对括号发明的。\n");

                /* ---- 6. 用括号换取可读性：一个真实对比 ---- */
    printf("---- 6. 加括号的收益 ----\n");
    unsigned int reg = 0x1234u;
                /* 不好读：要三级心算优先级 */
    unsigned int hi1 = reg >> 8 & 0xFF | 0x10 << 8;
                /* 好读：意图一目了然 */
    unsigned int hi2 = (((reg >> 8) & 0xFFu) | (0x10u << 8));
    printf("两种写法结果都是 0x%04X（值相同，但第二种以后能看懂）\n", hi1);
    printf("hi1=0x%04X hi2=0x%04X\n", hi1, hi2);

                /* ---- 7. 常见"少写括号"的错误清单（每条都是真事）---- */
    printf("---- 7. 容易漏括号的地方 ----\n");
    printf("a) 宏：        #define SQ(x) x*x      -> SQ(1+2) = 5\n");
    printf("b) 位测试：    flags & MASK == 0      -> 恒假/恒真\n");
    printf("c) 移位：      1 << n + 1             -> 1 << (n+1)\n");
    printf("d) 三目：      a > b ? a : b + 1      -> 结果是 b+1 而不是 max+1\n");
    int aa = 5, bb = 3;
    printf("   实例：%d > %d ? %d : %d + 1  = %d\n",
           aa, bb, aa, bb, aa > bb ? aa : bb + 1);
    printf("   想表达 (max)+1 要写 ((a>b)?a:b) + 1 = %d\n",
           ((aa > bb) ? aa : bb) + 1);
    printf("e) 负数取模：  -1 %% 3 的结果是 -1 而不是 2\n");
    printf("   实例：-1 %% 3 = %d，想要正的模要写 ((-1 %% 3) + 3) %% 3 = %d\n",
           -1 % 3, ((-1 % 3) + 3) % 3);
    return 0;
}
