#include <cstddef>

namespace aux {

    static constexpr size_t ptr_size = sizeof(char*);

    template <typename T, bool on_stack = sizeof(T) <= ptr_size>
    class optional;

    template<typename T>
    class optional<T, true> {
        public:
            optional() : m_has_value{ false } {
                ::new (&m_value.no_value) unsigned char(0);
            }

            optional(T other) : m_has_value{ true } {
                ::new (&m_value.value) T(other);
            }

            [[nodiscard]] operator bool() const {
                return has_value();
            }
            [[nodiscard]] bool has_value() const { 
                return m_has_value;
            };
            [[nodiscard]] const T& operator*() const {
                return m_value.value;
            }
            T& value() {
                return m_value.value;
            }
            const T& value() const { return m_value.value; }

            ~optional() {
                if (m_has_value)
                    m_value.value.~T();
            };

            optional(const optional&) = delete;
            optional& operator=(const optional&) = delete;
            optional(optional&&) = delete;
            optional& operator=(optional&&) = delete;
        private:
            union Storage {
                T value;
                unsigned char no_value;
                Storage() {}
                ~Storage() {}
            } m_value;
            bool m_has_value;
    };

    template<typename T>
    class optional<T, false> {
        public:
            optional() : m_value{ nullptr } {}

            optional(T other) : m_value{ new T(other) } {}

            [[nodiscard]] operator bool() const {
                return has_value();
            }
            [[nodiscard]] bool has_value() const { 
                return m_value != nullptr;
            };
            [[nodiscard]] T& operator*() const {
                return value();
            }
            T& value() const {
                return *m_value;
            }

            ~optional() {
                delete m_value;
            }
            
            optional(const optional&) = delete;
            optional& operator=(const optional&) = delete;
            optional(optional&&) = delete;
            optional& operator=(optional&&) = delete;
        private:
            T* m_value;
    };
}
