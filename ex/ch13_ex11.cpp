// ch13_ex11.cpp —— 多重继承：一个类有多个基类
// 编译：check.bat ch13_ex11.cpp c++latest
#include <cstdio>

class Printable {                     // 能力 1：可打印
public:
    virtual void print() const = 0;
    virtual ~Printable() = default;
};

class Serializable {                  // 能力 2：可序列化
public:
    virtual void save() const = 0;
    virtual ~Serializable() = default;
};

class DataObject {                    // 能力 3：有数据
public:
    explicit DataObject(int id) : id_(id) {}
    int id() const { return id_; }
private:
    int id_;
};

// 多重继承：同时具备三种能力
class Record : public DataObject, public Printable, public Serializable {
public:
    explicit Record(int id) : DataObject(id) {}
    void print() const override { std::printf("  Record::print() id=%d\n", id()); }
    void save()  const override { std::printf("  Record::save()  id=%d\n", id()); }
};

int main()
{
    Record r(7);

        // 1) 通过不同基类指针访问同一个对象
    Printable*    p = &r;
    Serializable* s = &r;
    DataObject*   d = &r;
    p->print();
    s->save();
    std::printf("1) id=%d\n", d->id());

        // 2) 不同基类子对象有不同的地址（多重继承的地址偏移）
    std::printf("2) 地址对比: &r=%p  (DataObject*)&r=%p  (Printable*)&r=%p\n",
                (void*)&r, (void*)d, (void*)p);
    std::printf("   注意：基类子对象地址可能不同，这是多重继承的正常现象\n");

        // 3) 用途：多继承主要用来"实现多个接口"
    std::printf("3) 多继承最正当的用途：一个基类提供实现/数据，其余是接口\n");

        // 4) 坑：两个基类有同名成员 -> 二义性，必须显式指明
    std::printf("4) 坑：如果两个基类都有叫 x 的成员，r.x 会编译报错（含义不明确），\n");
    std::printf("   必须写 r.A::x 或 r.B::x\n");
    return 0;
}
