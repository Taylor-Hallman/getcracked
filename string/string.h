#include <cstring>
#include <algorithm>
#include <stdexcept>
namespace getcracked {

    struct string_long {
        char *m_buffer_ptr; // 8 bytes
        size_t m_size;      // 8 bytes
        size_t m_capacity;  // 8 bytes
    };

    struct string_short {
        static constexpr size_t capacity{23uz};
        char m_buffer[capacity]; // 23 bytes
        unsigned char m_size;    // 1 byte
    };

    class string {
        using iterator = char*;
        using const_iterator = const char*;

    private:
        union {
            string_short s;
            string_long l;
            unsigned char bytes[24];
        } m_data;

        bool is_short() const {
            return m_data.bytes[23] & 0x01;
        }

        void set_size(size_t new_size) {
            if (is_short())
                m_data.s.m_size = static_cast<unsigned char>((new_size << 0x01) | 0x01);
            else
                m_data.l.m_size = new_size;
        }

        static constexpr size_t growth_factor{2uz};

    public:
        // ---------------------------- constructors -------------------------

        // default constructor
        string() : m_data(string_short{ .m_size = 0x01 }) {
            m_data.s.m_buffer[0] = '\0';
        }

        // from literal constructor
        string(const char* chars_array) {
            size_t size = strlen(chars_array);
            if (size < string_short::capacity) {
                m_data.s = string_short{ .m_size = static_cast<unsigned char>((size << 0x01) | 0x01) }; // force low bit to 1 for small strings
                strcpy(m_data.s.m_buffer, chars_array);
            }
            else {
                size_t capacity = size & 0x01 ? size + 1 : size + 2;
                m_data.l = string_long{
                    .m_buffer_ptr = static_cast<char*>(operator new(capacity)),
                    .m_size = size,
                    .m_capacity = capacity
                };
                strcpy(m_data.l.m_buffer_ptr, chars_array);
            }
        }

        // copy constructor
        string(const string& other) {
            if (other.is_short()) {
                m_data.s = other.m_data.s;
            }
            else {
                m_data.l.m_size = other.m_data.l.m_size;
                m_data.l.m_capacity = other.m_data.l.m_capacity;
                m_data.l.m_buffer_ptr = static_cast<char*>(operator new(other.m_data.l.m_capacity));
                strcpy(m_data.l.m_buffer_ptr, other.m_data.l.m_buffer_ptr);
            }
        }

        // move constructor
        string(string&& other) noexcept {
            if (other.is_short()) {
                m_data.s = other.m_data.s;
                other.erase(other.begin(), other.end());
            } 
            else {
                m_data.l.m_size = other.m_data.l.m_size;
                m_data.l.m_capacity = other.m_data.l.m_capacity;
                m_data.l.m_buffer_ptr = other.m_data.l.m_buffer_ptr;
                other.m_data.l.m_buffer_ptr = nullptr;
            }
        }


        friend void swap(string& lhs, string& rhs) {
            std::swap(lhs.m_data.bytes, rhs.m_data.bytes);
        }

        // copy assignment
        string& operator=(const string& other) {
            string tmp(other);
            swap(*this, tmp);
            return *this;
        }

        // move assignment
        string& operator=(string&& other) noexcept {
            if (this == &other)
                return *this;
            if (!is_short())
                operator delete(m_data.l.m_buffer_ptr);
            if (other.is_short()) {
                m_data.s = other.m_data.s;
                other.erase(other.begin(), other.end());
            }
            else {
                m_data.l.m_size = other.m_data.l.m_size;
                m_data.l.m_capacity = other.m_data.l.m_capacity;
                m_data.l.m_buffer_ptr = other.m_data.l.m_buffer_ptr;
                other.m_data.l.m_buffer_ptr = nullptr;
            }
            return *this;
        }

        // construct a string from an arbitrary range [first, last)
        template<typename InputIt>
        string(InputIt first, InputIt last) {
            size_t len = std::distance(first, last);
            if (len < string_short::capacity) {
                m_data.s = string_short{ .m_size = static_cast<unsigned char>((len << 0x01) | 0x01) };
                char* buf = m_data.s.m_buffer;
                size_t i{};
                for (auto itr = first; itr != last; ++itr, ++i) {
                    buf[i] = *itr;
                }
                return;
            }
            size_t capacity = len & 0x01 ? len + 1 : len + 2;
            m_data.l = string_long{ .m_buffer_ptr = static_cast<char*>(operator new(capacity)), .m_size = len, .m_capacity = capacity };
            char* buf = m_data.l.m_buffer_ptr;
            size_t i{};
            for (auto itr = first; itr != last; ++itr, ++i) {
                buf[i] = *itr;
            }
        }

        // construct a string from an initializer list
        string(std::initializer_list<char> list) {
            size_t len = list.size();
            if (len < string_short::capacity) {
                m_data.s = string_short{ .m_size = static_cast<unsigned char>((len << 0x01) | 0x01) };
                size_t i{};
                for (const char& c : list) {
                    m_data.s.m_buffer[i++] = c;
                }
                return;
            }
            size_t capacity = len & 0x01 ? len + 1 : len + 2;
            m_data.l = string_long{ .m_buffer_ptr = static_cast<char*>(operator new(capacity)), .m_size = len, .m_capacity = capacity };
            size_t i{};
            for (const char& c : list) {
                m_data.l.m_buffer_ptr[i++] = c;
            }
        }

        // constructs a string with count copies of the character ch
        string(size_t count, char ch) {
            if (count < string_short::capacity) {
                m_data.s = string_short{ .m_size = static_cast<unsigned char>((count << 0x01) | 0x01) };
                for (size_t i{}; i < count; ++i)
                    m_data.s.m_buffer[i] = ch;
                return;
            }
            size_t capacity = count & 0x01 ? count + 1 : count + 2;
            m_data.l = string_long{ .m_buffer_ptr = static_cast<char*>(operator new(capacity)), .m_size = count, .m_capacity = capacity };
            for (size_t i{}; i < count; ++i)
                m_data.l.m_buffer_ptr[i] = ch;
        }

        // destructor
        ~string() {
            if (!is_short())
                operator delete(m_data.l.m_buffer_ptr);
        }

        // ---------------------- capacity functions -------------------------
        // size of the string
        size_t size() {
            return is_short() ? m_data.s.m_size >> 0x01 : m_data.l.m_size; // not sure what the point of the non-const version is?
        }

        // const version
        size_t size() const {
            return is_short() ? m_data.s.m_size >> 0x01 : m_data.l.m_size;
        }

        // const version
        size_t capacity() const {
            return is_short() ? string_short::capacity - 1 : m_data.l.m_capacity - 1;
        }

        // checks if string is empty
        bool empty() const {
            return !size();
        }

        // allocate new storage
        void reserve(size_t new_capacity) {
            if ((new_capacity < string_short::capacity) || (!is_short() && new_capacity <= m_data.l.m_capacity))
                return;
            new_capacity += new_capacity & 0x01 ? 1 : 2;
            char* new_buf = static_cast<char*>(operator new(new_capacity));
            if (is_short()) {
                strcpy(new_buf, m_data.s.m_buffer);
                size_t size = this->size();
                m_data.l = string_long{ .m_buffer_ptr = new_buf, .m_size = size, .m_capacity = new_capacity };
            }
            else {
                strcpy(new_buf, m_data.l.m_buffer_ptr);
                operator delete(m_data.l.m_buffer_ptr);
                m_data.l.m_buffer_ptr = new_buf;
                m_data.l.m_capacity = new_capacity;
            }
        }

        // ---------------------------- Element Access ------------------------

        // data() return the pointer to the first character of the string
        char* data() {
            return is_short() ? m_data.s.m_buffer : m_data.l.m_buffer_ptr;
        }

        // const version
        const char* data() const {
            return is_short() ? m_data.s.m_buffer : m_data.l.m_buffer_ptr;
        }

        // Array subscript operator[]
        char& operator[](size_t pos) {
            return data()[pos];
        }

        // const version
        const char& operator[](size_t pos) const {
            return data()[pos];
        }

        // indexed access with bounds checking
        char& at(size_t pos) {
            if (pos >= capacity())
                throw std::out_of_range("Index out of range");
            return data()[pos];
        }

        // Returns a reference to the first character of the string
        char& front() {
            return data()[0];
        }

        // Returns a reference to the last character of the string
        char& back() {
            return data()[size() - 1];
        }

        // ---------------------------------- Iterators -----------------------------

        char* begin() {
            return data();
        }
        const char* begin() const {
            return data();
        }
        char* end() {
            return data() + size();
        }
        const char* end() const {
            return data() + size();
        }

        // ------------------------------- Modifiers -----------------------------

        // Appends the given character ch to the end of the string
        void push_back(char ch) {
            size_t capacity = this->capacity(), size = this->size();
            if (size == capacity) {
                size_t new_capacity = capacity * growth_factor;
                reserve(new_capacity);
            }
            data()[size] = ch;
            set_size(size + 1);
        }

        // removes a character from the end of the string
        void pop_back() {
            size_t size = this->size();
            data()[size - 1] = '\0';
            set_size(size - 1);
        }

        // inserts the characters from the range [first, last)
        // before the element (if any) pointed to by pos
        template<typename InputIt>
        void insert(const_iterator pos, InputIt first, InputIt last) {
            size_t size = this->size();
            size_t len = std::distance(first, last);
            size_t new_size = size + len;
            const_iterator begin = this->begin();
            const_iterator f = &*first, l = &*last;
            iterator end = this->end();
            string buf;
            if (new_size > capacity()) {
                std::less<const_iterator> lt;
                if (lt(f, end) && lt(begin, l)) { // buffer before moving the data in case of self-referential insertion
                    buf = string(first, last);
                    f = buf.begin();
                    l = buf.end();
                }
                reserve(new_size * growth_factor);
            }
            for (auto itr = f; itr != l; ++itr, ++pos) {
                size_t index = std::distance(begin, pos);
                if (index < size)
                    data()[index + len] = data()[index];
                data()[index] = *itr;
            }
            set_size(new_size);
        }

        // Removes the characters in the range [first, last)
        iterator erase(const_iterator first, const_iterator last) {
            size_t len = std::distance(first, last);
            size_t size = this->size();
            size_t new_size = size - len;
            const_iterator begin = this->begin();
            iterator end = this->end();
            if (std::distance(begin, first) >= size)
                return end;
            for (auto itr = first; itr != end; ++itr) {
                size_t index = std::distance(begin, itr);
                auto data = this->data();
                data[index] = index + len < size ? data[index + len] : '\0';
            }
            set_size(new_size);
            return const_cast<iterator>(first);
        }
    };

} // namespace getcracked
