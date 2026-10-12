// ch24_ex05.cpp —— 例 24-5：观察者模式（Observer）
// 场景：飞控里"解锁状态变了"要通知好几件事：点亮 LED、记录日志、发遥测、
// 允许/禁止电机输出。如果全塞在一个函数里，加一个通知方就要改一次。
// 观察者 = 让"事件源"只管发通知，"关心的人"自己来注册。
#include <cstdio>
#include <functional>
#include <string>
#include <vector>

// ---- 事件 + 订阅者 ----
enum class Event { Armed, Disarmed, Failsafe };

class EventBus {
public:
    using Handler = std::function<void(Event)>;

        // 注册：返回一个"订阅号"，之后可以用它取消
    int subscribe(Handler h) {
        handlers_.push_back(std::move(h));
        return static_cast<int>(handlers_.size()) - 1;
    }

    void unsubscribe(int id) {
        if (id >= 0 && id < static_cast<int>(handlers_.size())) {
            handlers_[static_cast<std::size_t>(id)] = nullptr;
        }
    }

    void publish(Event e) const {
        for (const auto& h : handlers_) {
            if (h) {
                h(e);  // 只负责调用，不关心谁在听
            }
        }
    }

private:
    std::vector<Handler> handlers_;
};

const char* name_of(Event e) {
    switch (e) {
        case Event::Armed:
            return "解锁";
        case Event::Disarmed:
            return "上锁";
        case Event::Failsafe:
            return "失控保护";
    }
    return "?";
}

int main() {
    EventBus bus;

        // 订阅方 1：LED
    const int led = bus.subscribe([](Event e) {
        std::printf("  [LED]     %s -> %s\n", name_of(e),
                    e == Event::Armed ? "常亮" : (e == Event::Disarmed ? "慢闪" : "快闪"));
    });

        // 订阅方 2：日志
    bus.subscribe([](Event e) { std::printf("  [LOG]     记录事件 %s\n", name_of(e)); });

        // 订阅方 3：遥测（带捕获，模拟一个上下文对象）
    std::string craft = "PX4-001";
    bus.subscribe([craft](Event e) {
        std::printf("  [TELEM]   %s 上报 %s\n", craft.c_str(), name_of(e));
    });

    std::printf("发布 解锁：\n");
    bus.publish(Event::Armed);
    std::printf("发布 失控保护：\n");
    bus.publish(Event::Failsafe);

    std::printf("取消 LED 订阅后再发布上锁：\n");
    bus.unsubscribe(led);
    bus.publish(Event::Disarmed);

    std::printf("\n好处：事件源不需要知道有谁在听；加一个新通知方不用改事件源。\n");
    std::printf("代价与坑：\n");
    std::printf("  1) 回调里**不要**做耗时的事（尤其不能阻塞），\n");
    std::printf("     如果是中断上下文触发的事件，回调必须短小且可重入。\n");
    std::printf("  2) 订阅者的生命周期。如果回调里捕获了局部变量的引用，\n");
    std::printf("     对象销毁后回调还在被调用 -> 野指针（本机 ASan 能抓，见例 24-18）。\n");
    std::printf("  3) 不要在 publish 过程中 unsubscribe（改容器会让迭代器失效）。\n");
    std::printf("     稳妥做法：先标记，publish 结束后统一清理。\n");
    std::printf("  4) 嵌入式上别用 std::function（可能堆分配）。\n");
    std::printf("     用【函数指针 + void* 上下文】，或者固定大小的回调数组。\n");
    return 0;
}
