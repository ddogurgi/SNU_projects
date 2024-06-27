#ifndef __FHEAP_H_
#define __FHEAP_H_

#include <iostream>
#include <initializer_list>
#include <optional>
#include <vector>
#include <cmath>
#include <memory>
#include <queue>

template <typename T>
class PriorityQueue {
    public:
        virtual void insert(const T& item) = 0;
        virtual std::optional<T> extract_min() = 0;
        virtual bool is_empty() const = 0;
};

template <typename T>
class FibonacciNode {
    public:
        // constructors
        FibonacciNode()
            :key(std::nullopt), degree(0), child(nullptr), right(nullptr) {}
        FibonacciNode(const T& item)
            :key(item), degree(0), child(nullptr), right(nullptr) {}

        // destructor
        ~FibonacciNode() = default;

        T key;
        size_t degree;

        std::shared_ptr<FibonacciNode<T>> right;
        std::shared_ptr<FibonacciNode<T>> child;
        // NOTE: If you set left/parent pointer to shared_ptr, circular reference may cause!
        // So, left/parent pointer should be set to weak_ptr.
        std::weak_ptr<FibonacciNode<T>> left;
        std::weak_ptr<FibonacciNode<T>> parent;
};

template <typename T>
class FibonacciHeap : public PriorityQueue<T> {
    public:
        // Default Constructor
        FibonacciHeap()
            : min_node(nullptr), size_(0) {}

        // Constructor with Value
        FibonacciHeap(const T& item)
            : min_node(nullptr), size_(0) { insert(item); }

        // Disable copy constructor.
        FibonacciHeap(const FibonacciHeap<T> &);

        // Constructor with initializer_list
        // ex) FibonacciHeap<int> fheap = {1, 2, 3, 4};
        FibonacciHeap(std::initializer_list<T> list): min_node(nullptr), size_(0) {
            for(const T& item : list) {
                insert(item);
            }
        }

        // Destructor
        ~FibonacciHeap();

        // Insert item into the heap.
        void insert(const T& item) override;

        // Return raw pointer of the min node.
        // If the heap is empty, return nullptr.
        FibonacciNode<T>* get_min_node() { return min_node.get(); }

        // Return minimum value of the min node.
        // If the heap is empty, return std::nullopt.
        std::optional<T> get_min() const;

        // 1. Return minimum value of the min node
        // 2. Remove the node which contains minimum value from the heap.
        // If the heap is empty, return std::nullopt;
        std::optional<T> extract_min() override;

        // Return true if the heap is empty, false if not.
        bool is_empty() const override { return !size_; }

        // Return the number of nodes the heap contains.
        size_t size() const { return size_; }


    private:
        // Points to the node which has minimum value.
        std::shared_ptr<FibonacciNode<T>> min_node;

        // Value that represents how many nodes the heap contains.
        size_t size_;

        void insert(std::shared_ptr<FibonacciNode<T>>& node);

        // After extract, clean up the heap.
        void consolidate();

        // Combine two nodes.
        void merge(std::shared_ptr<FibonacciNode<T>>& x, std::shared_ptr<FibonacciNode<T>>& y);

        // Function for Destructor : Cutting circular reference
        void cut_circular_reference(std::shared_ptr<FibonacciNode<T>>& node);
};

template <typename T>
void FibonacciHeap<T>::cut_circular_reference(std::shared_ptr<FibonacciNode<T>>& node){
    std::shared_ptr<FibonacciNode<T>> p = node;
    do{
        if(p->child != nullptr)
            cut_circular_reference(p->child); 
        p = p->right;
    }while(p != node);
    node->right = nullptr;
}

template <typename T>
FibonacciHeap<T>::~FibonacciHeap() {
    // TODO
    if(min_node) cut_circular_reference(min_node);
}

template <typename T>
std::optional<T> FibonacciHeap<T>::get_min() const {
    // TODO
    //return is_empty() ? std::nullopt : T(min_node->key);
    if(is_empty()) return std::nullopt;
    return T(min_node->key);
}

template <typename T>
void FibonacciHeap<T>::insert(const T& item) {
    // TODO
    std::shared_ptr<FibonacciNode<T>> new_node = std::make_shared<FibonacciNode<T>>(item);
    insert(new_node);
    size_++;
}

template <typename T>
void FibonacciHeap<T>::insert(std::shared_ptr<FibonacciNode<T>>& node) {
    // TODO
    if(min_node == nullptr){
        min_node = node;
        node->left = node;
        node->right = node;
        return;
    }
    node->right = min_node;
    node->left = min_node->left;
    //(min_node->left.lock())->right = node;
    (node->left.lock())->right = node;
    //min_node->left = node;
    (node->right)->left = node;
    if(node->key < min_node->key)
        min_node = node;
}

template <typename T>
std::optional<T> FibonacciHeap<T>::extract_min() {
    // TODO
    if(min_node == nullptr)
        return std::nullopt;
    std::optional<T> extracted_key = T(min_node->key);
    std::shared_ptr<FibonacciNode<T>> init_min_node = min_node;
    if(min_node->right != min_node){
        (min_node->left.lock())->right = min_node->right;
        (min_node->right)->left = min_node->left;
        if(min_node->child != nullptr){
            std::shared_ptr<FibonacciNode<T>> c = min_node->child;
            (c->left.lock())->right = min_node->right;
            (min_node->right)->left = c->left;
            c->left = min_node->left;
            (min_node->left.lock())->right = c;
        }
        min_node = min_node->right;
    }
    else min_node = min_node->child;
    init_min_node->child=nullptr;
    init_min_node->right=nullptr;
    size_--;
    consolidate();
    return extracted_key;
}

template <typename T>
void FibonacciHeap<T>::consolidate() {
    // TODO
    if(min_node == nullptr){
        return;
    }
    const double phi = 1.61803399;
    const int array_size = (int)(log2(double(size()) / log2(phi))) + 1;
    std::shared_ptr<FibonacciNode<T>> array[array_size] = {nullptr, };
    std::vector<std::shared_ptr<FibonacciNode<T>>> root_list;
    std::shared_ptr<FibonacciNode<T>> init_min_node = min_node;
    std::shared_ptr<FibonacciNode<T>> to_right = min_node;
    std::shared_ptr<FibonacciNode<T>> parent;
    std::shared_ptr<FibonacciNode<T>> child;
    do {
        root_list.push_back(to_right);
        to_right->parent.reset();
        if(min_node->key > to_right->key)
            min_node = to_right;
        to_right = to_right->right;
    } while(to_right != init_min_node);
    for(std::shared_ptr<FibonacciNode<T>> root : root_list){
        if(array[root->degree] == nullptr)
            array[root->degree] = root;
        else{
            if(array[root->degree]->key >= root->key)
                {parent = root; child = array[root->degree];}
            else
                {child = root; parent = array[root->degree];}
            array[parent->degree] = nullptr;
            merge(parent, child);
            while(array[parent->degree] != nullptr){
                if(parent->key > array[parent->degree]->key)
                    {child = parent; parent = array[parent->degree];}
                else
                    child = array[parent->degree];
                array[parent->degree] = nullptr;
                merge(parent, child);
            }
            array[parent->degree] = parent;
        }
    }
}

template <typename T>
void FibonacciHeap<T>::merge(std::shared_ptr<FibonacciNode<T>>& x, std::shared_ptr<FibonacciNode<T>>& y) {
    // TODO
    if(y == min_node) min_node = x;
    (y->left.lock())->right = y->right;
    (y->right)->left = y->left;
    y->left = y;
    y->right = y;
    y->parent = x;
    
    if(x->child != nullptr){
        y->right = (x->child);
        y->left = (x->child)->left;
        (y->left.lock())->right = y;
        (y->right)->left = y;
    }
    else x->child = y;
    x->degree++;
}

#endif // __FHEAP_H_
