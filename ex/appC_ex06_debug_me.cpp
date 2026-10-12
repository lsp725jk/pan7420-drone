// 附录C 例 6：用来演示调试器的小程序（故意留一个空指针解引用）
// 用命令行调试器 cdb 打开它：
// "C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\cdb.exe" -o -g -G appC_ex06_debug_me.exe
#include <cstdio>

struct Sensor {
    int  id;
    int  value;
};

// 返回 nullptr 表示"没找到"，调用方必须先判空
static Sensor* find_sensor(int id) {
    static Sensor table[2] = {{1, 100}, {2, 200}};
    for (int i = 0; i < 2; ++i) {
        if (table[i].id == id) return &table[i];
    }
    return nullptr;
}

int main(int argc, char** argv) {
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    int wanted = (argc > 1) ? 99 : 1;          // 默认查存在的 1 号

    Sensor* s = find_sensor(wanted);
    std::printf("查找 id=%d -> %s\n", wanted, s ? "找到" : "没找到");

        // 这里的判空是"故意写错"的：应当写成 if (s == nullptr) ...
    if (s != nullptr) {
        std::printf("  id=%d value=%d\n", s->id, s->value);
    } else {
        std::printf("  走这里就安全\n");
    }
    return 0;
}
