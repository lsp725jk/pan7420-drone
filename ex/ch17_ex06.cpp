// ch17_ex06.cpp —— 【必做实验】remove_if 不配 erase：元素没删掉
#include <algorithm>
#include <cstdio>
#include <vector>

bool is_odd(int x) { return x % 2 != 0; }

int main()
{
    std::printf("--- 1) 【必做实验】remove_if 之后容器大小没变 ---\n");
    {
        std::vector<int> v{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        std::printf("    原始：       ");
        for (int x : v) std::printf("%d ", x);
        std::printf("（size=%zu）\n", v.size());

        auto new_end = std::remove_if(v.begin(), v.end(), is_odd);
        std::printf("    remove_if 后：");
        for (int x : v) std::printf("%d ", x);
        std::printf("（size=%zu）\n", v.size());

        std::printf("    ^^^ 注意：size 还是 10！奇数并没有被删除！\n");
        std::printf("    remove_if 返回的新\"逻辑末尾\"距离 begin 是 %td\n", new_end - v.begin());
        std::printf("    也就是说：前 %td 个是保留下来的偶数，后面 %td 个是\"残余垃圾\"\n",
                    new_end - v.begin(), v.end() - new_end);
        std::printf("    这段残余里的内容是**未指定的**，不要读它\n");
        std::printf("    → 这就是经典的坑：以为 remove_if 会删元素，其实它只是\"挪动\"\n");
    }

    std::printf("--- 2) 【正确写法】remove_if + erase（erase-remove 惯用法）---\n");
    {
        std::vector<int> v{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        std::printf("    原始：       ");
        for (int x : v) std::printf("%d ", x);
        std::printf("（size=%zu）\n", v.size());

        v.erase(std::remove_if(v.begin(), v.end(), is_odd), v.end());
        std::printf("    erase+remove：");
        for (int x : v) std::printf("%d ", x);
        std::printf("（size=%zu）← 这次真的删掉了\n", v.size());
    }

    std::printf("--- 3) C++20 起的简写：std::erase_if ---\n");
    {
        std::vector<int> v{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        std::erase_if(v, is_odd);
        std::printf("    std::erase_if(v, is_odd) 后：");
        for (int x : v) std::printf("%d ", x);
        std::printf("（size=%zu）← 一行搞定，不会忘\n", v.size());
    }

    std::printf("--- 4) 为什么这样设计？因为 remove_if 要能用于链表 ---\n");
    {
        std::printf("    remove_if 的接口是\"迭代器 + 返回新末尾\"，它不知道容器是什么\n");
        std::printf("    对 vector：它把保留的元素依次往前挪（O(n)，元素被赋值/移动）\n");
        std::printf("    对 list：它有专门的成员函数 l.remove_if()，直接改指针（真正的删除）\n");
        std::printf("    所以标准把\"删除\"留给了容器的 erase —— 这就是两段式的原因\n");
    }

    std::printf("--- 5) 其他几个\"看着像删除、其实不删\"的算法 ---\n");
    {
        std::vector<int> v{ 1, 1, 2, 2, 2, 3, 4, 4 };
        std::printf("    原始（有相邻重复）：        ");
        for (int x : v) std::printf("%d ", x);
        std::printf("（size=%zu）\n", v.size());

        (void)std::unique(v.begin(), v.end());    // 只去掉**相邻**重复，而且不删元素
        std::printf("    unique 后（未 erase）：     ");
        for (int x : v) std::printf("%d ", x);
        std::printf("（size=%zu）← size 没变！\n", v.size());
        std::printf("    前 4 个 [1 2 3 4] 才是\"该保留的\"，第 5 个往后是**未指定的残余**\n");
        std::printf("    （上面打印出来的 2 3 4 4 是残余值，换编译器可能不一样，不要读）\n");

        v.erase(std::unique(v.begin(), v.end()), v.end());
        std::printf("    unique + erase 后：         ");
        for (int x : v) std::printf("%d ", x);
        std::printf("（size=%zu）← 这次才真的删掉\n", v.size());
        std::printf("    注意 unique 只处理**相邻**的重复元素，所以一般要先 sort\n");

        std::vector<int> w{ 3, 1, 4, 1, 5, 9, 2, 6 };
        std::sort(w.begin(), w.end());
        w.erase(std::unique(w.begin(), w.end()), w.end());
        std::printf("    sort + unique + erase 后：  ");
        for (int x : w) std::printf("%d ", x);
        std::printf("（真正的去重）\n");
    }

    std::printf("--- 6) 对比：partition 也是\"划分\"而不是\"删除\" ---\n");
    {
        std::vector<int> v{ 1, 2, 3, 4, 5, 6 };
        auto mid = std::partition(v.begin(), v.end(), is_odd);
        std::printf("    partition(is_odd) 后：");
        for (int x : v) std::printf("%d ", x);
        std::printf("\n");
        std::printf("    奇数在前（%td 个），偶数在后 —— 元素一个都没少（size=%zu）\n",
                    mid - v.begin(), v.size());
    }

    std::printf("--- 7) 症状 / 原因 / 修法 ---\n");
    std::printf("    症状：写了 remove_if，遍历时元素还在（只是顺序变了），size 不变\n");
    std::printf("    原因：remove_if 是**算法**，它只拿得到迭代器，拿不到容器；\n");
    std::printf("          它只能\"覆盖赋值 + 返回新末尾\"，删不了\"容器的 size\"\n");
    std::printf("    修法：v.erase(std::remove_if(...), v.end());\n");
    std::printf("          或者 C++20 起 std::erase_if(v, pred);\n");
    std::printf("          链表用成员函数 l.remove_if(pred);\n");

    return 0;
}
