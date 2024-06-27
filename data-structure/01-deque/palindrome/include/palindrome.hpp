#include <string>

#include "deque.hpp"

template<typename Deque>
class Palindrome {
public:
    bool is_palindrome(const std::string&);
    void reset_deque();

private:
    Deque deque;
};

template<typename Deque>
bool Palindrome<Deque>::is_palindrome(const std::string& s1) {
    // TODO
    bool ans = true;
    for(int i = 0; i < s1.length(); i++)
        deque.push_front(s1[i]);
    for(int i = 0; i < s1.length(); i++)
        if(deque.remove_front() != s1[i]) ans = false;
    return ans;
}

template<typename Deque>
void Palindrome<Deque>::reset_deque() {
    while (!deque.empty())
        deque.remove_front();
}
