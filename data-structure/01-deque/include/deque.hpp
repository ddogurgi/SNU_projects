#ifndef _DEQUE_H
#define _DEQUE_H

#include <string>
#include <iostream>
#include <type_traits>
#include <optional>
#include <iostream>
#include <memory>
#include <cassert>

/* NOTE: Deque, ArrayDeque, ListDeque Declaration modification is not allowed.
 * Fill in the TODO sections in the following code. */
template <typename T>
class Deque {
public:
    virtual ~Deque() = default;

    /* NOTE: We won't implement push functions that take rvalue references. */
    virtual void push_front(const T&) = 0;
    virtual void push_back(const T&) = 0;

    /* NOTE: Unlike STL implementations which have separate `front` and
       pop_front` functions, we have one unified method for removing an elem. */
    virtual std::optional<T> remove_front() = 0;
    virtual std::optional<T> remove_back() = 0;

    virtual bool empty() = 0;
    virtual size_t size() = 0;

    virtual T& operator[](size_t) = 0;
};

template <typename T>
class ArrayDeque : public Deque<T> {
public:
    ArrayDeque();
    ~ArrayDeque() = default;

    void push_front(const T&) override;
    void push_back(const T&) override;

    std::optional<T> remove_front() override;
    std::optional<T> remove_back() override;

    bool empty() override;
    size_t size() override;
    size_t capacity();

    T& operator[](size_t) override;

private:
    std::unique_ptr<T[]> arr;
    size_t front;
    size_t back;
    size_t size_;
    size_t capacity_;

    void resize();
};

template <typename T>
ArrayDeque<T>::ArrayDeque() :
    front{63 /* You can change this */},
    back{0 /* You can change this */},
    size_{0}, capacity_{64} {
    arr = std::make_unique<T[]>(capacity_);
}

template <typename T>
void ArrayDeque<T>::push_front(const T& item) {
    // TODO
    if(front == back){
        resize();
    }
    arr[front] = item;
    front--;
    if(front == -1)
        front += capacity_;
    size_++;

}

template <typename T>
void ArrayDeque<T>::push_back(const T& item) {
    // TODO
    if(front == back){
        resize();
    }
    arr[back] = item;
    back++;
    if(back == capacity_)
        back = 0;
    size_++;
}

template <typename T>
std::optional<T> ArrayDeque<T>::remove_front() {
    // TODO
    if(empty())
        return std::nullopt;
    front++;
    if(front == capacity_)
        front = 0;
    std::optional<T> ret = arr[front];
    arr[front] = T();
    size_--;
    return ret;
}

template <typename T>
std::optional<T> ArrayDeque<T>::remove_back() {
    // TODO
    if(empty())
        return std::nullopt;
    back--;
    if(back == -1)
        back = capacity_ - 1;
    std::optional<T> ret = arr[back];
    arr[back] = T();
    size_--;
    return ret;
}

template <typename T>
void ArrayDeque<T>::resize() {
    // TODO
    std::unique_ptr<T[]> arr2 = std::make_unique<T[]>(2*capacity_);
    for(int i = 0; i < front; i++)
        arr2[i] = arr[i];
    for(int i = front; i < capacity_; i++)
        arr2[i + capacity_] = arr[i];
    arr = std::move(arr2);
    arr2.reset();
    front += capacity_;
    capacity_ *= 2;
}

template <typename T>
bool ArrayDeque<T>::empty() {
    // TODO
    if(size_) return false; else return true;
}

template <typename T>
size_t ArrayDeque<T>::size() {
    // TODO
    return size_;
}

template <typename T>
size_t ArrayDeque<T>::capacity() {
    // TODO
    return capacity_;
}

template <typename T>
T& ArrayDeque<T>::operator[](size_t idx) {
    // TODO
    if (idx >= size_ || idx < 0) 
        throw std::out_of_range("Index out of range");
    return arr[(idx+front+1)%capacity_];
}

template<typename T>
struct ListNode {
    std::optional<T> value;
    ListNode* prev;
    ListNode* next;

    ListNode() : value(std::nullopt), prev(this), next(this) {}
    ListNode(const T& t) : value(t), prev(this), next(this) {}

    ListNode(const ListNode&) = delete;
};

template<typename T>
class ListDeque : public Deque<T> {
public:
    ListDeque();
    ~ListDeque();

    void push_front(const T&) override;
    void push_back(const T&) override;

    std::optional<T> remove_front() override;
    std::optional<T> remove_back() override;

    bool empty() override;
    size_t size() override;

    T& operator[](size_t) override;

    size_t size_ = 0;
    ListNode<T>* sentinel = nullptr;
};

template<typename T>
ListDeque<T>::ListDeque() : sentinel(new ListNode<T>{}), size_(0) {}

template<typename T>
void ListDeque<T>::push_front(const T& t) {
    // TODO
    ListNode<T> *new_node = new ListNode<T>(t);
    if(sentinel->next != nullptr){
        ListNode<T> *frt = sentinel->next;
        new_node->next = frt;
        frt->prev = new_node;
    }
    sentinel->next = new_node;
    new_node->prev = sentinel;
    size_++;
}

template<typename T>
void ListDeque<T>::push_back(const T& t) {
    // TODO
    ListNode<T> *new_node = new ListNode<T>(t);
    if(sentinel->prev != nullptr){
        ListNode<T> *bck = sentinel->prev;
        new_node->prev = bck;
        bck->next = new_node;
    }
    sentinel->prev = new_node;
    new_node->next = sentinel; 
    size_++;
}

template<typename T>
std::optional<T> ListDeque<T>::remove_front() {
    // TODO
    
    if(sentinel->next == nullptr)
        return std::nullopt;
    ListNode<T> *frt = sentinel->next;
    if(frt->next != nullptr){
        sentinel->next = frt->next;
        (frt->next)->prev = sentinel;
    }
    std::optional<T> ret = frt->value;
    delete frt;
    frt = nullptr;
    size_--;
    return ret;
}

template<typename T>
std::optional<T> ListDeque<T>::remove_back() {
    // TODO
    if(sentinel->prev == nullptr)
        return std::nullopt;
    ListNode<T> *bck = sentinel->prev;
    if(bck->prev != nullptr){
        sentinel->prev = bck->prev;
        (bck->prev)->next = sentinel;
    }
    std::optional<T> ret = bck->value;
    delete bck;
    bck = nullptr;
    size_--;
    return ret;
}

template<typename T>
bool ListDeque<T>::empty() {
    // TODO
    if(size_) return false; else return true;
    //return false;
}

template<typename T>
size_t ListDeque<T>::size() {
    // TODO
    return size_;
    //return 0;
}

template<typename T>
T& ListDeque<T>::operator[](size_t idx) {
    // TODO
    if (idx >= size_ || idx < 0) 
        throw std::out_of_range("Index out of range");
    ListNode<T> *cur = sentinel->next;
    for(int i = 0; i < idx; i++){
        cur = cur->next;
    }
    return cur->value.value();
    //return *new T{};
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const ListNode<T>& n) {
    if (n.value)
        os << n.value.value();

    return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const ListDeque<T>& l) {
    auto np = l.sentinel->next;
    while (np != l.sentinel) {
        os << *np << ' ';
        np = np->next;
    }

    return os;
}

template<typename T>
ListDeque<T>::~ListDeque() {
    // TODO
    ListNode<T> *cur = sentinel->next;
    while(cur != nullptr && cur != sentinel){
        ListNode<T> *remove = cur;
        cur = cur->next;
        delete remove;
        remove = nullptr;
    }
    delete cur;
    cur = nullptr;
}

#endif // _DEQUE_H
