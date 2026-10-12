// ch07_ex12.cpp —— 结构体数组与结构体做映射表（真实场景）
// 编译：check.bat ch07_ex12.cpp c++latest
#include <cstdio>
#include <cstring>

typedef struct {
    int         id;
    const char* name;
    double      scale;
} SensorInfo;

// 静态映射表：放在只读数据区，不占栈
static const SensorInfo kSensors[] = {
    { 0, "temperature", 0.1  },
    { 1, "pressure",    0.01 },
    { 2, "humidity",    0.5  },
};
static const int kSensorCount = (int)(sizeof(kSensors) / sizeof(kSensors[0]));

// 按 id 查表：返回 const 指针，调用者不能改表内容
const SensorInfo* find_sensor(int id)
{
    for (int i = 0; i < kSensorCount; ++i) {
        if (kSensors[i].id == id) return &kSensors[i];
    }
    return nullptr;                    // 找不到返回空指针（第 5 章的约定）
}

int main()
{
    printf("[1] 结构体数组的遍历（三种写法）:\n");
    for (int i = 0; i < kSensorCount; ++i)                 // 下标
        printf("    [%d] id=%d %s scale=%.2f\n",
               i, kSensors[i].id, kSensors[i].name, kSensors[i].scale);

    printf("\n    用指针遍历:\n");
    for (const SensorInfo* p = kSensors; p != kSensors + kSensorCount; ++p)
        printf("    id=%d %s\n", p->id, p->name);
    printf("    ← p++ 每次前进 sizeof(SensorInfo)=%zu 字节\n", sizeof(SensorInfo));

    printf("\n[2] 查表函数（嵌入式里最常见的「配置表」用法）:\n");
    int query = 2;
    const SensorInfo* found = find_sensor(query);
    if (found != nullptr) {
        printf("    find_sensor(%d) → \"%s\" scale=%.2f\n", query, found->name, found->scale);
    } else {
        printf("    find_sensor(%d) → 没找到\n", query);
    }
    const SensorInfo* missing = find_sensor(99);
    printf("    find_sensor(99) → %s\n", missing ? missing->name : "(nullptr)");

    printf("\n[3] 结构体数组的内存布局：一整块连续内存\n");
    printf("    sizeof(kSensors)      = %zu 字节（%d 个元素 × %zu）\n",
           sizeof(kSensors), kSensorCount, sizeof(SensorInfo));
    printf("    &kSensors[0] = %p\n", (const void*)&kSensors[0]);
    printf("    &kSensors[1] = %p  差 %td 字节\n",
           (const void*)&kSensors[1],
           (const char*)&kSensors[1] - (const char*)&kSensors[0]);
    printf("    &kSensors[2] = %p  差 %td 字节\n",
           (const void*)&kSensors[2],
           (const char*)&kSensors[2] - (const char*)&kSensors[0]);

    printf("\n[4] 为什么用 const 和 static:\n");
    printf("    const  → 表放在只读段，写它会崩（等于免费的错误检查）\n");
    printf("    static → 文件内可见，不会和别的模块的 kSensors 撞名\n");
    printf("    （kSensors 是文件级 static，这里能访问是因为都在同一个 .cpp 里）\n");
    return 0;
}
