#include <cstddef>

/* Fill in the TODO sections in the following code. */

template <typename K, typename V>
class HashSlot
{
public:
    HashSlot(): _empty(true), _removed(false) {
    }
    
    K get_key() const {
        return _key;
    }

    V get_value() const {
        return _value;
    }

    void set_key_value(K key, V value) {
        // TODO
        _key = key; _value = value;
    }

    bool is_empty() const {
        // TODO                
        return _empty;
    }

    void set_empty(bool empty) {
        // TODO
        _empty = empty;
    }

    bool is_removed() const {
        // TODO
        return _removed;
    }

    void set_removed(bool removed) {
        // TODO
        _removed = removed;
    }
    
private:
    // key-value pair
    K _key;
    V _value;
    bool _empty;
    bool _removed;

    // disallow copy and assignment
    HashSlot(const HashSlot &);
    HashSlot & operator=(const HashSlot &);
};
