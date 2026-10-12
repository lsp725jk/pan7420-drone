// ch12_ex21.cpp —— 嵌套类（nested class）与其访问权限
// 编译：check.bat ch12_ex21.cpp c++latest
#include <cstdio>

class Queue {
public:
    Queue() : head_(0), tail_(0), count_(0) {}

        // 嵌套类：定义在 Queue 里面，名字是 Queue::Node
    class Node {
    public:
        int  value = 0;
        void dump(const char* who) const { std::printf("   %s: Node{value=%d}\n", who, value); }
    };

        // 更常见的形式：私有嵌套类 + 公开别名（把实现藏起来）
    struct Impl;                       // 只声明，定义放到 .cpp 里（pimpl 惯用法）

    void push(int v)
    {
        if (count_ >= 4) { std::printf("   队列满了\n"); return; }
        data_[tail_].value = v;
        tail_ = (tail_ + 1) % 4;
        ++count_;
    }
    int pop()
    {
        if (count_ == 0) return -1;
        int v = data_[head_].value;
        head_ = (head_ + 1) % 4;
        --count_;
        return v;
    }
    int size() const { return count_; }

        // 外层类可以直接访问嵌套类的私有成员，反过来也一样（C++11 起双向）
    int head_value() const { return data_[head_].value; }

private:
    Node data_[4];
    int  head_, tail_, count_;
};

int main()
{
        // 1) 从外面用嵌套类：要写全名 Queue::Node
    Queue::Node n;
    n.value = 7;
    n.dump("1) 外部构造的嵌套类对象");

        // 2) 嵌套类不自动拥有外层类的 this，也不共享外层对象
    std::printf("2) 嵌套类只是命名分组，sizeof(Queue::Node)=%llu\n",
                (unsigned long long)sizeof(Queue::Node));

        // 3) 实际用途：容器把"节点类型"藏在里面
    Queue q;
    q.push(1); q.push(2);
    std::printf("3) 入队两个, size=%d, 队首值=%d, 出队=%d, 出队后 size=%d\n",
                q.size(), q.head_value(), q.pop(), q.size());

        // 4) 嵌套类也能用在模板/枚举里做命名分组
    std::printf("4) 嵌套类的作用：把只服务于本类的辅助类型关在类内部，\n");
    std::printf("   避免污染外层命名空间（和命名空间的动机一样）\n");
    return 0;
}
