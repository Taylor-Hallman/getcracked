#include <atomic>
#include <memory>
namespace getcracked {

    class stop_source; 

    class stop_token {
    public:
        stop_token() : state(std::make_shared<bool>(true)) {}

        // Has the stop been requested?
        bool stop_requested() const {
            return !(*state);
        }

    private:

        // Set the cancelled state, returns the previously existing state.
        // You can only call cancel through stop_source.
        bool cancel() {
            bool prev = *state;
            *state = false;
            return prev;
        }

        std::shared_ptr<bool> state;

        friend class stop_source;
    };

    class stop_source {
    public:
        stop_source() {}

        // Asks for cancellation, returns the previous cancellation state.
        bool request_stop() {
            return token.cancel();
        }

        // Returns whether a cancellation has been requested.
        bool stop_requested() const {
            return token.stop_requested();
        }

        stop_token get_token() {
            return token;
        }
    private:
        stop_token token;
    };
}
