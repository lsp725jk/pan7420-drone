// ch19_ex10_jthread_stop_token.cpp —— std::jthread（C++20）与 std::stop_token
// 编译: check.bat ch19_ex10_jthread_stop_token.cpp c++latest
#include <iostream>
#include <thread>
#include <stop_token>
#include <condition_variable>
#include <mutex>
#include <string>
#include <vector>
#include <chrono>
#include <atomic>

std::mutex g_ioMutex;
void say(const std::string& s)
{
    std::lock_guard<std::mutex> lk(g_ioMutex);
    std::cout << s << "\n";
}

std::atomic<int> g_ticks{0};

// ---------- 1) jthread 最省事的地方：析构自动 join ----------
void autoJoinDemo()
{
    say("  进入作用域，创建 jthread");
    {
        std::jthread jt([] {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            say("    [jthread] 干完活了");
        });
        say("  即将离开作用域 —— 什么都不用写");
    }   // jthread 析构 = request_stop() + join()
    say("  离开作用域成功：jthread 自动 join 了（换成 std::thread 这里会 terminate）");
}

// ---------- 2) stop_token：协作式取消 ----------
void cancellableWorker(std::stop_token st, int id)
{
    int n = 0;
        // 每轮都问一句"还要继续吗？"
    while (!st.stop_requested()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        ++n;
        g_ticks.fetch_add(1);
    }
    say("    [worker " + std::to_string(id) + "] 收到停止请求，我已经跑了 "
        + std::to_string(n) + " 轮，主动退出");
}

// ---------- 3) 用 stop_callback 做清理 ----------
void callbackDemo()
{
    std::jthread jt([](std::stop_token st) {
        std::stop_callback cb(st, [] {
            say("    [stop_callback] 停止请求刚发出，我立刻被调用（在请求线程里执行）");
        });
        while (!st.stop_requested()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
        say("    [worker] 看到 stop_requested() 为真，退出");
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    say("  main: 调用 request_stop()");
    jt.request_stop();
        // jt 析构时自动 join
}

// ---------- 4) 条件变量也要能被打断：condition_variable_any + stop_token ----------
void interruptibleWait()
{
    std::mutex m;
    std::condition_variable_any cv;
    std::jthread jt([&](std::stop_token st) {
        std::unique_lock<std::mutex> lk(m);
                // 关键：普通的 condition_variable::wait 不接受 stop_token，会一直睡；
                // condition_variable_any 有 stop_token 重载，停止请求能"叫醒"它。
        bool woke = cv.wait(lk, st, [] { return false; });   // 故意永远不为真
        say(std::string("    [worker] wait 返回，woke = ") + (woke ? "true（是谓词成立）" : "false（是被停止请求打断）"));
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    say("  main: request_stop()，看它能不能从 wait 里醒过来");
    jt.request_stop();
}

int main()
{
    say("=== A. jthread 自动 join：忘 join 不再 terminate ===");
    autoJoinDemo();

    say("");
    say("=== B. stop_token：协作式取消（不是强杀）===");
    {
        std::jthread w1(cancellableWorker, 1);
        std::jthread w2(cancellableWorker, 2);
        say("  main: 让它们跑 200ms");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        int before = g_ticks.load();
        say("  main: 发停止请求（request_stop）");
        w1.request_stop();
        w2.request_stop();
                // 析构时还会再 join 一次（已请求停止，很快返回）
        say("  停止请求发出后，ticks 还在涨吗？请看下面数字是否与停止前相同");
                // join 由析构完成，这里手动 join 以便比较
        w1.join();
        w2.join();
        int after = g_ticks.load();
        say("  停止前 ticks = " + std::to_string(before)
            + "，join 之后 ticks = " + std::to_string(after)
            + (after == before ? "   <- 已经停了，没有再涨" : "   <- 又涨了一点（停止前刚好在睡）"));
    }

    say("");
    say("=== C. stop_callback：停止时立刻执行一段清理代码 ===");
    callbackDemo();

    say("");
    say("=== D. condition_variable_any + stop_token：让 wait 也能被打断 ===");
    interruptibleWait();

    say("");
    say("=== E. std::thread vs std::jthread 对照 ===");
    say("  析构行为   : thread 若 joinable 则 std::terminate；jthread 自动 request_stop + join");
    say("  取消机制   : thread 没有；jthread 内置 stop_token / stop_source");
    say("  取消语义   : 协作式 —— 线程必须自己定期检查 stop_requested()，");
    say("               语言层面故意不提供\"强杀线程\"（强杀会留下锁、堆、栈的不一致状态）");
    say("  传参       : 线程函数第一个参数若是 std::stop_token，jthread 会自动注入");
    say("  能 join 吗 : 都能（jthread 也有 join / detach / joinable）");
    say("  用哪个     : 新代码默认用 jthread；需要交给 C 接口或要 detach 时用 thread");

    say("");
    say("=== F. 嵌入式视角 ===");
    say("  Cortex-M0+ 裸机上根本没有 std::thread：没有 OS 内核、没有 8KB RAM 装线程栈。");
    say("  \"取消\"这件事在单片机上靠 volatile/atomic 标志位 + 主循环检查实现，");
    say("  思路和 stop_token 完全一样（协作式），只是没有库封装。详见第 21 章。");

    say("");
    say("main 正常结束");
    return 0;
}
