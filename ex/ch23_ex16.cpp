// ch23_ex16.cpp —— 例 23-16：缓存友好性（AoS vs SoA）
// CPU 取数据是按"缓存行"（一般 64 字节）整块取的。
// 如果你只要 x 坐标，却把 y/z/vx/vy/vz 一起搬进来，就浪费了 5/6 的带宽。
// 这是"数据结构布局"比"算法微优化"更值钱的典型例子。
#include <chrono>
#include <cstdio>
#include <vector>

constexpr int N = 1000000;

// ---- AoS：Array of Structs，一个粒子一个结构体 ----
struct ParticleAoS {
    float x, y, z;
    float vx, vy, vz;
};

// ---- SoA：Struct of Arrays，每个分量一个数组 ----
struct ParticlesSoA {
    std::vector<float> x, y, z;
    std::vector<float> vx, vy, vz;

    explicit ParticlesSoA(int n)
        : x(n, 1.0f), y(n, 2.0f), z(n, 3.0f), vx(n, 0.5f), vy(n, 0.25f), vz(n, 0.125f) {}
};

int main() {
    std::vector<ParticleAoS> aos(N, ParticleAoS{1.0f, 2.0f, 3.0f, 0.5f, 0.25f, 0.125f});
    ParticlesSoA soa(N);

    std::printf("单个粒子：AoS = %zu 字节，只用到 x 时【有效带宽】只有 1/6\n", sizeof(ParticleAoS));
    std::printf("AoS 总内存 = %.1f MB，SoA 单个分量数组 = %.1f MB\n",
                N * sizeof(ParticleAoS) / 1048576.0, N * 4 / 1048576.0);

    const int ROUNDS = 50;

        // ---- AoS：只更新 x ----
    const auto t0 = std::chrono::steady_clock::now();
    for (int r = 0; r < ROUNDS; ++r) {
        for (int i = 0; i < N; ++i) {
            aos[i].x += aos[i].vx * 0.016f;
        }
    }
    const auto t1 = std::chrono::steady_clock::now();

        // ---- SoA：只更新 x ----
    for (int r = 0; r < ROUNDS; ++r) {
        for (int i = 0; i < N; ++i) {
            soa.x[i] += soa.vx[i] * 0.016f;
        }
    }
    const auto t2 = std::chrono::steady_clock::now();

    const double ms_aos = std::chrono::duration<double, std::milli>(t1 - t0).count();
    const double ms_soa = std::chrono::duration<double, std::milli>(t2 - t1).count();

    std::printf("\n只更新 x 分量，%d 个粒子 x %d 轮：\n", N, ROUNDS);
    std::printf("  AoS : %8.2f ms\n", ms_aos);
    std::printf("  SoA : %8.2f ms   快 %.2f 倍\n", ms_soa, ms_aos / ms_soa);
    std::printf("  结果抽查: aos[0].x=%.3f soa.x[0]=%.3f\n", aos[0].x, soa.x[0]);

        // ---- 反面：如果每次要用到全部 6 个分量，SoA 就没优势了 ----
    const auto t3 = std::chrono::steady_clock::now();
    for (int r = 0; r < ROUNDS; ++r) {
        for (int i = 0; i < N; ++i) {
            aos[i].x += aos[i].vx;
            aos[i].y += aos[i].vy;
            aos[i].z += aos[i].vz;
        }
    }
    const auto t4 = std::chrono::steady_clock::now();
    for (int r = 0; r < ROUNDS; ++r) {
        for (int i = 0; i < N; ++i) {
            soa.x[i] += soa.vx[i];
            soa.y[i] += soa.vy[i];
            soa.z[i] += soa.vz[i];
        }
    }
    const auto t5 = std::chrono::steady_clock::now();
    const double ms_aos3 = std::chrono::duration<double, std::milli>(t4 - t3).count();
    const double ms_soa3 = std::chrono::duration<double, std::milli>(t5 - t4).count();
    std::printf("\n三个分量一起更新时：\n");
    std::printf("  AoS : %8.2f ms\n", ms_aos3);
    std::printf("  SoA : %8.2f ms   快 %.2f 倍\n", ms_soa3, ms_aos3 / ms_soa3);

    std::printf("\n怎么选：\n");
    std::printf("  - 常常只访问少数字段（比如只要姿态角）  -> SoA\n");
    std::printf("  - 每次都要整条记录（比如存 Flash、发串口）-> AoS\n");
    std::printf("  - 嵌入式上还要考虑：SoA 更容易做 DMA 搬运（连续、定长）\n");

        // 坑 1：AoS 里塞了不常用的字段，会让缓存行利用率骤降。
        // 解决：把热字段和冷字段拆成两个结构体（"热冷分离"）。
        // 坑 2：std::vector< bool > 是特化版本，按位存，取一个元素要移位，
        // 既慢又不能取地址。需要位图就用 std::vector<uint8_t> 或自己写。
    std::printf("\n（/Od 下差距被压缩；-O2 下 SoA 的优势通常更明显）\n");
    return 0;
}
