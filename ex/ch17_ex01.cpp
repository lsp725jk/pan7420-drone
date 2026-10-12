// ch17_ex01.cpp —— vector vs C 手写动态数组：容量、越界、边界，谁在替我操心
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <vector>

// ---------------- C 风格手写动态数组 ----------------
struct IntVec {
    int* data;
    int  size;
    int  cap;
};

void cvec_init(IntVec* v, int cap)
{
    v->data = static_cast<int*>(malloc(sizeof(int) * static_cast<size_t>(cap)));
    if (!v->data) { std::printf("malloc 失败\n"); std::exit(1); }
    v->size = 0;
    v->cap  = cap;
}

void cvec_push(IntVec* v, int x)
{
    if (v->size == v->cap) {                 // 容量不够要自己扩
        int  newcap = v->cap * 2;
        int* p = static_cast<int*>(realloc(v->data, sizeof(int) * static_cast<size_t>(newcap)));
        if (!p) { std::printf("realloc 失败\n"); std::exit(1); }
        v->data = p;
        v->cap  = newcap;
        std::printf("  [C] 扩容到 %d\n", newcap);
    }
    v->data[v->size++] = x;
}

void cvec_free(IntVec* v)
{
    free(v->data);
    v->data = nullptr;
    v->size = v->cap = 0;
}

int main()
{
    std::printf("--- 1) C：手写动态数组，扩容要自己写 ---\n");
    {
        IntVec v;
        cvec_init(&v, 2);
        for (int i = 1; i <= 5; ++i) {
            cvec_push(&v, i * i);
            std::printf("  [C] push %d 后 size=%d cap=%d\n", i * i, v.size, v.cap);
        }
        std::printf("  [C] 内容：");
        for (int i = 0; i < v.size; ++i) std::printf("%d ", v.data[i]);
        std::printf("\n");
        cvec_free(&v);
        std::printf("  [C] 释放完成。注意：所有越界、扩容、释放都得我自己保证\n");
    }

    std::printf("--- 2) C++：vector 自己管容量和释放 ---\n");
    {
        std::vector<int> v;
        for (int i = 1; i <= 5; ++i) {
            v.push_back(i * i);
            std::printf("  [C++] push %d 后 size=%zu cap=%zu\n", i * i, v.size(), v.capacity());
        }
        std::printf("  [C++] 内容：");
        for (int x : v) std::printf("%d ", x);
        std::printf("\n");
        std::printf("  [C++] 离开作用域自动释放，一行释放代码都不用写\n");
    }

    std::printf("--- 3) 差别一：size 和 capacity 是两个概念 ---\n");
    {
        std::vector<int> v;
        std::printf("  空 vector：size=%zu capacity=%zu\n", v.size(), v.capacity());
        v.reserve(100);
        std::printf("  reserve(100) 后：size=%zu capacity=%zu\n", v.size(), v.capacity());
        v.push_back(1);
        std::printf("  push_back(1) 后：size=%zu capacity=%zu\n", v.size(), v.capacity());
        std::printf("  reserve 只改容量，不改大小 —— 这一点和 C 里 malloc 一块大 buffer 很像\n");
    }

    std::printf("--- 4) 差别二：at() 会检查越界，operator[] 不会 ---\n");
    {
        std::vector<int> v{ 10, 20, 30 };
        std::printf("  v[1]     = %d\n", v[1]);
        std::printf("  v.at(1)  = %d\n", v.at(1));
        std::printf("  v[10] 是未定义行为（不检查）；v.at(10) 会抛异常：\n");
        try {
            std::printf("  v.at(10) = %d\n", v.at(10));
        }
        catch (const std::out_of_range& e) {
            std::printf("  捕获到 std::out_of_range: %s\n", e.what());
        }
        std::printf("  嵌入式建议：默认用 operator[]（零开销），调试期改用 at()\n");
    }

    std::printf("--- 5) 差别三：赋值/拷贝是值语义 ---\n");
    {
        std::vector<int> a{ 1, 2, 3 };
        std::vector<int> b = a;        // 深拷贝
        b[0] = 100;
        std::printf("  b = a 之后 b[0]=%d, a[0]=%d（独立的两份数据）\n", b[0], a[0]);
        std::printf("  对比 C：int* b = a; 只是复制了指针，改 b[0] 会连带改 a[0]\n");
        int  ca[3] = { 1, 2, 3 };
        int* cb = ca;
        cb[0] = 100;
        std::printf("  C 里 int* cb = ca; cb[0]=100 之后 ca[0]=%d\n", ca[0]);
    }

    std::printf("--- 6) 差别四：删除中间元素 ---\n");
    {
        std::vector<int> v{ 1, 2, 3, 4, 5 };
        v.erase(v.begin() + 2);                  // 删掉 3
        std::printf("  erase 掉第 3 个元素后：");
        for (int x : v) std::printf("%d ", x);
        std::printf("（后面的元素自动前移）\n");

        std::printf("  C 里要自己写：\n");
        std::printf("    for (int i = 2; i < size - 1; ++i) data[i] = data[i + 1];\n");
        std::printf("    --size;\n");
    }

    return 0;
}
