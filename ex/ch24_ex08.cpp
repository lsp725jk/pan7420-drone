// ch24_ex08.cpp —— 例 24-8：pimpl 惯用法（编译防火墙）
// 问题：头文件里放了多少东西，所有包含它的 .cpp 都要跟着重新编译；
// 而且头文件会暴露私有成员和第三方库的 include。
// pimpl = pointer to implementation：头文件里只留一个不完整类型的指针，
// 真正的成员藏在 .cpp 里。
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

// ==================================================================
// 头文件里应该长的样子（本文件里把"声明"和"实现"放在一起，方便单文件演示）
// ==================================================================
class SensorHub {
public:
    SensorHub();
    ~SensorHub();                       // 必须在 .cpp 里定义（因为 Impl 在这里还不完整）
    SensorHub(SensorHub&&) noexcept;    // 移动可以，拷贝不行
    SensorHub& operator=(SensorHub&&) noexcept;

    void add(const std::string& name);
    std::size_t count() const;
    const std::string& at(std::size_t i) const;

        // 假设这里还要暴露一个第三方类型：用前向声明就够了
        // class ThirdPartyType;
        // ThirdPartyType* raw();

private:
    struct Impl;                  // 只有声明，没有定义
    std::unique_ptr<Impl> impl_;  // 头文件里只需要知道"有这么个指针"
};

// ==================================================================
// .cpp 里才展开
// ==================================================================
struct SensorHub::Impl {
    std::vector<std::string> names;
    std::string cache;
        // 这里可以随便改：加字段、换容器、甚至换第三方库，
        // 都不会影响包含 SensorHub.h 的那些 .cpp
};

SensorHub::SensorHub() : impl_(std::make_unique<Impl>()) {
    std::printf("  [SensorHub] 构造\n");
}
SensorHub::~SensorHub() { std::printf("  [SensorHub] 析构\n"); }
SensorHub::SensorHub(SensorHub&&) noexcept = default;
SensorHub& SensorHub::operator=(SensorHub&&) noexcept = default;

void SensorHub::add(const std::string& name) { impl_->names.push_back(name); }
std::size_t SensorHub::count() const { return impl_->names.size(); }
const std::string& SensorHub::at(std::size_t i) const { return impl_->names.at(i); }

// ==================================================================
// 对比：没有 pimpl 的版本
// ==================================================================
class SensorHubPlain {
public:
    void add(const std::string& name) { names_.push_back(name); }
    std::size_t count() const { return names_.size(); }

private:
    std::vector<std::string> names_;  // 头文件里必须 include <vector> 和 <string>
    std::string cache_;
};

int main() {
    std::printf("pimpl 版：\n");
    {
        SensorHub hub;
        hub.add("ICM42688");
        hub.add("BMP390");
        hub.add("GPS");
        std::printf("  器件数 = %zu，第一个 = %s\n", hub.count(), hub.at(0).c_str());
    }

    std::printf("\n不用 pimpl 的版本：\n");
    {
        SensorHubPlain h;
        h.add("ICM42688");
        std::printf("  器件数 = %zu\n", h.count());
    }

    std::printf("\n两者的 sizeof 差很多：\n");
    std::printf("  sizeof(SensorHub)      = %zu 字节（只是一个指针）\n", sizeof(SensorHub));
    std::printf("  sizeof(SensorHubPlain) = %zu 字节（两个容器的全部成员）\n",
                sizeof(SensorHubPlain));

    std::printf("\npimpl 的取舍：\n");
    std::printf("  收益 1：编译防火墙。改 Impl 的私有成员，包含头文件的 .cpp 不用重编。\n");
    std::printf("          大项目里这一条能省掉大量编译时间。\n");
    std::printf("  收益 2：头文件干净。不用把 <vector>/<string>/第三方头文件带出去。\n");
    std::printf("  收益 3：二进制兼容（ABI）。发布闭源库时，改内部实现不用重新发头文件。\n");
    std::printf("  代价 1：多一次指针间接访问（每次访问成员都要解引用）。\n");
    std::printf("  代价 2：对象要堆分配，构造/析构变贵。\n");
    std::printf("  代价 3：要手工写析构/移动（因为 unique_ptr<Impl> 需要完整类型）。\n");
    std::printf("\n嵌入式上：如果对象是全局唯一的（比如相机/总线管理器），\n");
    std::printf("  可以不用 unique_ptr，改成【函数内 static 的 Impl + 返回引用】，\n");
    std::printf("  这样连堆分配都省了。\n");
    return 0;
}
