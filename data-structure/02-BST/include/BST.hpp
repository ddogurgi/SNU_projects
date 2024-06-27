#include <algorithm>
#include <iostream>
#include <vector>
#include <functional>
#include <iterator>
#include <memory>


template <typename T>
class TreeNode
{
    public:
        T element;
        std::unique_ptr<TreeNode<T>> left;
        std::unique_ptr<TreeNode<T>> right;

        TreeNode<T>(const T& e)
            :element{e}, left{nullptr}, right{nullptr} {}

        ~TreeNode() {}

};


template <typename T>
struct BST
{
    public:
        std::unique_ptr<TreeNode<T>> root = nullptr;

        ~BST() {}

        bool insert(const T& key);
        bool search(const T& key);
        bool remove(const T& key);

    private:
        bool insert(std::unique_ptr<TreeNode<T>>& t, const T& key);
        bool search(std::unique_ptr<TreeNode<T>>& t, const T& key);
        bool remove(std::unique_ptr<TreeNode<T>>& t, const T& key);

        T find_rightmost_key(std::unique_ptr<TreeNode<T>>& t);
};

template <typename T>
bool BST<T>::insert(const T& key) {
    return insert(root, key);
}

template <typename T>
bool BST<T>::search(const T& key) {
    return search(root, key);
}

template <typename T>
bool BST<T>::remove(const T& key) {
    return remove(root, key);
}

template <typename T>
bool BST<T>::insert(std::unique_ptr<TreeNode<T>>& t, const T& key) {

    // TODO
    // if insertion fails (i.e. if the key already exists in tree), return false
    // otherwise, return true
    if(t){
        if(t->element == key) return false;
        return t->element > key ? insert(t->left, key) : insert(t->right, key);
    }
    else t = std::make_unique<TreeNode<T>>(key);
    return true;
}

template <typename T>
bool BST<T>::search(std::unique_ptr<TreeNode<T>>& t, const T& key) {
    // TODO
    if(t){
        if(t->element == key) return true;
        return t->element > key ? search(t->left, key) : search(t->right, key);
    }
    return false;
}

template <typename T>
T BST<T>::find_rightmost_key(std::unique_ptr<TreeNode<T>>& t) {
    // TODO
    if(t)
        return t->right ? find_rightmost_key(t->right) : t->element;
    return T();
}

template <typename T>
bool BST<T>::remove(std::unique_ptr<TreeNode<T>>& t, const T& key) {
    // TODO
    // if key does not exist in tree, return false
    // otherwise, return true
    if(t){
        if(t->element == key){
            if(t->left == nullptr && t->right == nullptr)
                t.reset();
            else if(t->left != nullptr && t->right != nullptr){
                T rightmost_key = find_rightmost_key(t->left);
                t->element = rightmost_key;
                remove(t->left, rightmost_key);
            }
            else
                t = std::move(t->left != nullptr ? t->left : t->right);
            return true;
        }
        else return t->element > key ? remove(t->left, key) : remove(t->right, key);
    }
    return false;
}
