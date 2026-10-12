// 附录C 例 7：故意崩溃的程序，用来练习"调试器怎么告诉你崩在哪"
// 用 cdb 打开：
// set CDB=C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\cdb.exe
// "%CDB%" -o -g -G appC_ex07_crash_demo.exe
#include <cstdio>

struct Sensor { int id; int value; };

static Sensor* find_sensor(int id) {
    static Sensor table[2] = {{1, 100}, {2, 200}};
    for (int i = 0; i < 2; ++i)
        if (table[i].id == id) return &table[i];
    return nullptr;                 // 没找到
}

int main(int argc, char**) {
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    int wanted = (argc > 1) ? 99 : 1;
    Sensor* s = find_sensor(wanted);
    std::printf("查找 id=%d -> %s\n", wanted, s ? "找到" : "没找到");
    std::printf("即将读 s->value（没判空）...\n");
    std::printf("value = %d\n", s->value);     // 空指针解引用：崩溃点在这一行
    return 0;
}
