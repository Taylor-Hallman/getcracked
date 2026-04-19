#include "../stop_token/StopToken.h"
#include <thread>
#include <type_traits>

namespace getcracked {
    class jthread {
    public:
        ~jthread() {
            request_stop();
            if (t.joinable())
                t.join();
        }

        jthread(const jthread&) = delete;
        jthread& operator=(const jthread&) = delete;

        jthread(jthread&&) = delete;
        jthread& operator=(jthread&&) = delete;

        template <class F, class... Args>
        explicit jthread(F&& f, Args&&... args) {
            if constexpr (std::is_invocable_v<F, stop_token, Args...>) {
                stop_token stoken = stopSource.get_token();
                t = std::thread(f, stoken, args...);
            }
            else {
                t = std::thread(f, args...);
            }
        }

        bool joinable() const {
            return t.joinable();
        }
        void join() {
            t.join();
        }
        void detach() {
            t.detach();
        }

        void request_stop() {
            stopSource.request_stop();
        }
    private:
        std::thread t;
        stop_source stopSource;
    };
}
