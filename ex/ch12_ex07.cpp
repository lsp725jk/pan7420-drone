// ch12_ex07.cpp —— 析构函数与手动资源管理（RAII 的动机）
// 编译：check.bat ch12_ex07.cpp c++latest

// 这个例子用"文件名"假装占用一个资源，演示 C 风格与 C++ 风格的差别。
#include <cstdio>
#include <cstring>

// ---------------- C 风格：手动 open/close ----------------
typedef struct {
    char name[32];
    int  opened;
} CFile;

static int  c_open(CFile* f, const char* nm)
{
    std::snprintf(f->name, sizeof(f->name), "%s", nm);
    f->opened = 1;
    std::printf("  c_open(%s)\n", f->name);
    return 0;
}
static void c_close(CFile* f)
{
    if (f->opened) {
        std::printf("  c_close(%s)\n", f->name);
        f->opened = 0;
    }
}

static void c_style_work(void)
{
    CFile f;
    c_open(&f, "imu.log");
        // 如果这里 return 早了，或者中间 throw 了，就会忘记 close -> 泄漏
    c_close(&f);
}

// ---------------- C++ 风格：析构函数负责 close ----------------
class FileHandle {
public:
    explicit FileHandle(const char* nm)
    {
        std::snprintf(name_, sizeof(name_), "%s", nm);
        opened_ = true;
        std::printf("  FileHandle 构造 -> 打开 %s\n", name_);
    }
    ~FileHandle()
    {
        if (opened_) {
            std::printf("  FileHandle 析构 -> 关闭 %s\n", name_);
            opened_ = false;
        }
    }
    FileHandle(const FileHandle&) = delete;             // 不许拷贝（详见 12.8）
    FileHandle& operator=(const FileHandle&) = delete;
private:
    char name_[32];
    bool opened_ = false;
};

static void cpp_style_work(void)
{
    FileHandle f("imu.log");
    std::printf("  工作中...\n");
        // 无论怎么退出（正常 return、提前 return、抛异常），析构函数都会被调用
}

int main()
{
    std::printf("1) C 风格：谁打开谁关闭，靠人记\n");
    c_style_work();

    std::printf("2) C++ 风格：对象生命周期结束就自动关闭\n");
    cpp_style_work();

    std::printf("3) 这种模式叫 RAII：构造函数获取资源，析构函数释放资源\n");
    std::printf("   （Resource Acquisition Is Initialization，资源获取即初始化）\n");
    std::printf("   智能指针、文件流、锁都是这个套路，详见第 14/18 章\n");
    return 0;
}
