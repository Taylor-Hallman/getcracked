#include "string/string.h"
#include <cassert>

void test_move_assignment() {
    // 1) SSO to SSO Case
    {
        getcracked::string s1("Original");
        getcracked::string s2("New");
        s1 = std::move(s2);
        
        assert(s1.size() == 3);
        assert(std::strcmp(s1.data(), "New") == 0);
        // source (s2) should now be in a valid empty state
        assert(s2.size() == 0); 
    }

    // 2) Long to Long Case (The "Pointer Steal")
    {
        getcracked::string s1("This is a very long string currently occupying the heap memory.");
        getcracked::string s2("Another extremely long string that we will move into the first one.");
        
        const char* s2_heap_ptr = s2.data();
        size_t s2_size = s2.size();

        s1 = std::move(s2);

        assert(s1.size() == s2_size);
        // CRITICAL: Ownership transfer. s1 should now point to s2's old memory.
        assert(s1.data() == s2_heap_ptr);
        
        // s2 should be safely nulled out
        assert(s2.data() == nullptr || s2.size() == 0);
    }

    // 3) Long to Short Case
    {
        getcracked::string s1("A long string that will be overwritten by a short moved string.");
        getcracked::string s2("Short");
        
        s1 = std::move(s2);
        
        assert(s1.size() == 5);
    }

    // 4) Embedded Nulls Case
    {
        char data[] = {'m', '\0', 'v', 'e'};
        getcracked::string s1(data, data + 4);
        getcracked::string s2("Temporary");
        
        s2 = std::move(s1);
        assert(s2.size() == 4);
        assert(s2[1] == '\0');
    }

    // 5) Self-Move Assignment
    {
        getcracked::string s1("Don't break me");
        getcracked::string* ptr = &s1;
        s1 = std::move(*ptr);
        
        // After a self-move, the object must remain in a valid state.
        assert(s1.size() == 14);
        assert(std::strcmp(s1.data(), "Don't break me") == 0);
    }

}

void test_erase() {
    // 1) Short (SSO) Case: Middle Erase
    {
        getcracked::string s("ABXCD");
        // Erase the 'X'
        auto it = s.erase(s.begin() + 2, s.begin() + 3);
        
        assert(s.size() == 4);
        assert(std::strcmp(s.data(), "ABCD") == 0);
        // Standard requires erase to return an iterator to the next element
        assert(*it == 'C');
        assert(s.data()[4] == '\0');
    }

    // 2) Long (Heap) Case: Erase to End
    {
        getcracked::string s("This is a long string that we will truncate.");
        size_t original_size = s.size();
        
        // Erase from "that" onwards
        s.erase(s.begin() + 26, s.end());
        
        assert(s.size() == 26);
        assert(std::strcmp(s.data(), "This is a long string that") == 0);
        assert(s.data()[26] == '\0');
    }

    // 3) Embedded Nulls Case
    {
        char data[] = {'a', 'b', '\0', 'c', 'd'};
        getcracked::string s(data, data + 5);
        
        // Erase the null character
        s.erase(s.begin() + 2, s.begin() + 3);
        
        assert(s.size() == 4);
        assert(s[2] == 'c');
        assert(std::memcmp(s.data(), "abcd", 4) == 0);
    }

    // 4) Full Erase (Clear)
    {
        getcracked::string s("Clear me");
        s.erase(s.begin(), s.end());
        
        assert(s.size() == 0);
        assert(s.empty());
        assert(s.data()[0] == '\0');
    }
}

void test_insert() {
    // 1) Short SSO Case (External Range)
    {
        getcracked::string s("AC");
        char b = 'B';
        s.insert(s.begin() + 1, &b, &b + 1);
        assert(s.size() == 3);
        assert(std::strcmp(s.data(), "ABC") == 0);
    }

    // 2) Long Heap Case (External Range)
    {
        getcracked::string s("This is a long string.");
        const char* extra = " Indeed!";
        s.insert(s.end() - 1, extra, extra + std::strlen(extra));
        assert(s.size() == 22 + 8);
        assert(std::strstr(s.data(), "Indeed!."));
    }

    // 3) Embedded Nulls Case
    {
        getcracked::string s("a");
        char nulls[] = {'\0', 'b'};
        s.insert(s.end(), nulls, nulls + 2);
        assert(s.size() == 3);
        assert(s[1] == '\0' && s[2] == 'b');
    }

    // 4) Transition Case (SSO to Long)
    // Inserting enough characters to force a move from stack to heap
    {
        getcracked::string s("Small");
        getcracked::string long_val(50, 'z'); // External source
        s.insert(s.begin() + 1, long_val.begin(), long_val.end());
        
        assert(s.size() == 55);
        assert(s[0] == 'S');
        for(auto i{1uz}; i <=50; ++i){
            assert(s[i] == 'z');
        }
        assert(s[51] == 'm');
        assert(s[52] == 'a');
        assert(s[53] == 'l');
        assert(s[54] == 'l');
        assert(s[55] == '\0');
    }

    // 5) Self-Referential Insertion (Critical for Standard Compliance)
    // Inserting a substring of itself into itself. 
    // This is tricky because reallocation might move the source data!
    {
        getcracked::string s("Hello");
        // Insert "Hello" at the beginning to get "HelloHello"
        s.insert(s.begin(), s.begin(), s.end());
        assert(s.size() == 10);
        assert(std::strcmp(s.data(), "HelloHello") == 0);
        
        // Test self-insertion that triggers reallocation
        getcracked::string s2("Trigger"); // Currently SSO
        // Create a range from itself that will force it to go Long
        getcracked::string padding(20, '!');
        s2.insert(s2.end(), padding.begin(), padding.end()); 
        // Now s2 is likely near capacity. Force a self-insert that exceeds it.
        s2.insert(s2.begin(), s2.begin(), s2.end()); 
        
        assert(s2.size() == 54);
    }
}

void test_empty_function() {
    // 1) Truly empty
    getcracked::string s1;
    assert(s1.empty() == true);

    // 2) Not empty (SSO)
    getcracked::string s2("a");
    assert(s2.empty() == false);

    // 3) Not empty (Long)
    getcracked::string s3("This is a very long string");
    assert(s3.empty() == false);

    // 4) Not empty (Embedded Null)
    // A string containing only a null character is NOT empty
    char null_char = '\0';
    getcracked::string s4(&null_char, &null_char + 1);
    assert(s4.size() == 1);
    assert(s4.empty() == false);
}

void test_copy_constructor() {
    // 1) Short
    getcracked::string s1("Short");
    getcracked::string c1(s1);
    assert(c1.size() == 5);
    // 2) Long
    getcracked::string s2("Long string for copying over the heap");
    getcracked::string c2(s2);
    assert(c2.size() == s2.size());
    // 3) Embedded Null
    char data[] = {'a', '\0', 'b'};
    getcracked::string s3(data, data + 3);
    getcracked::string c3(s3);
    assert(c3.size() == 3 && c3[1] == '\0');
}

void test_pop_back() {
    // 1) Short (SSO) Case
    // Verify simple removal within the stack buffer
    getcracked::string s1("ABC");
    s1.pop_back();
    assert(s1.size() == 2);
    assert(s1[0] == 'A' && s1[1] == 'B');
    assert(s1.data()[2] == '\0'); // Crucial: internal null termination

    // 2) Long (Heap) Case
    // Verify removal in a string that stays on the heap
    getcracked::string s2("123456789012345678901234");
    size_t original_size = s2.size();
    size_t original_capacity = s2.capacity();
    
    for(auto i{1uz}; i <= 23; ++i)
    {
        s2.pop_back();
        assert(s2.size() == original_size - i);
        assert(s2.capacity() == original_capacity);
        assert(s2.data()[s2.size()] == '\0');
    }
    
    

    // 3) Embedded Nulls Case
    // Verify that popping a char after a null doesn't confuse the size logic
    char null_data[] = {'a', 'b', '\0', 'c'};
    getcracked::string s3(null_data, null_data + 4);
    s3.pop_back(); // Remove 'c'
    assert(s3.size() == 3);
    assert(s3.back() == '\0');
    assert(s3.data()[3] == '\0');
}

int main() {
    test_erase();
    test_insert();
    test_move_assignment();
    test_empty_function();
    test_copy_constructor();
    test_pop_back();
}
