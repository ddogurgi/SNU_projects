#include <algorithm>
#include <iostream>
#include <vector>
#include <functional>
#include <iterator>
#include <memory>

#define INITIAL_TABLE_SIZE 64

#include "hash_slot.hpp"
#include "hash_funcs.hpp"

template <typename K, typename V>
class HashTable {
public:
    HashTable(HashFunc *hash_func);
    ~HashTable();
    int get(const K &key, V &value);
    int put(const K &key, const V &value);
    int remove(const K &key);
    size_t get_table_size();
    size_t get_size();
    double get_load_factor();

protected:
    size_t table_size;
    
private:
    HashFunc *hash_func;
    size_t size;
    HashSlot<K, V> *table;

    // Should be overriden by the derived class
    virtual unsigned long get_next_pos(unsigned long pos,
                                       unsigned long step) = 0;
    unsigned long get_pos(const K key);
    void enlarge_table();
};

template <typename K, typename V>
HashTable<K, V>::HashTable(HashFunc *hash_func): table(), hash_func(hash_func),
                                                 size(0), table_size(INITIAL_TABLE_SIZE) {
    table = new HashSlot<K, V>[table_size];
}

template <typename K, typename V>
HashTable<K, V>::~HashTable() {
    delete[] table;
}

template <typename K, typename V>
void HashTable<K, V>::enlarge_table() {
    HashSlot<K, V>* prev_table = new HashSlot<K, V>[table_size];
    for(unsigned long i = 0; i < table_size; i++){
        prev_table[i].set_key_value(table[i].get_key(), table[i].get_value()); 
        prev_table[i].set_empty(table[i].is_empty());
    }
    delete[] table;
    table = new HashSlot<K, V>[2*table_size];
    size = 0;
    size_t prev_table_size = table_size;
    table_size *= 2;
    for(unsigned long i = 0; i < prev_table_size; i++)
        if(!prev_table[i].is_empty()) put(prev_table[i].get_key(), prev_table[i].get_value());
    
    delete[] prev_table;
}

template <typename K, typename V>
unsigned long HashTable<K, V>::get_pos(const K key) {
    // TODO
    return hash_func->hash(key) % table_size;
}

template <typename K, typename V>
int HashTable<K, V>::get(const K &key, V &value) {
    // TODO
    unsigned long init_pos = get_pos(key);
    unsigned long pos = init_pos;
    unsigned long step = 1;
    while((!table[pos].is_empty() || table[pos].is_removed()) && table[pos].get_key() != key && step < table_size){
        pos = get_next_pos(init_pos, step++);
    }
    if(!table[pos].is_empty() && table[pos].get_key() == key){
        value = table[pos].get_value();
        return (int)step;
    }
    return -1;
}

template <typename K, typename V>
int HashTable<K, V>::put(const K &key, const V &value) {
    // TODO
    unsigned long init_pos = get_pos(key);
    unsigned long pos = init_pos;
    unsigned long step = 1;
    while(!table[pos].is_empty() && table[pos].get_key() != key && step < table_size){
        pos = get_next_pos(init_pos, step++);
    }
    if(table[pos].is_empty()){
        table[pos].set_key_value(key, value);
        table[pos].set_empty(false);
        table[pos].set_removed(false);
        size++;
        if(get_load_factor() > 0.5)
            enlarge_table();
        return (int)step;
    }

    return -1;
}

template <typename K, typename V>
int HashTable<K, V>::remove(const K &key) {
    // TODO
    unsigned long init_pos = get_pos(key);
    unsigned long pos = init_pos;
    unsigned long step = 1;
    while(!table[pos].is_empty() && table[pos].get_key() != key && step < table_size){
        pos = get_next_pos(init_pos, step++);
    }
    if(!table[pos].is_empty() && table[pos].get_key() == key){
        table[pos].set_empty(true);
        table[pos].set_removed(true);
        size--;
        return (int)step;
    }
    return -1;
}

template <typename K, typename V>
size_t HashTable<K, V>::get_table_size() {
    return table_size;
}

template <typename K, typename V>
size_t HashTable<K, V>::get_size() {
    return size;
}

template <typename K, typename V>
double HashTable<K, V>::get_load_factor() {
    return (double)size/table_size;
}


template <typename K, typename V>
class LinearProbeHashTable: public HashTable<K, V> {
public:
    LinearProbeHashTable(HashFunc *hash_func): HashTable<K, V>(hash_func) {
    }
    
private:
    virtual unsigned long get_next_pos(unsigned long pos, unsigned long step) {
        // TODO
        return (pos + step) % HashTable<K, V>::get_table_size();
    }
};

template <typename K, typename V>
class QuadProbeHashTable: public HashTable<K, V> {
public:
    QuadProbeHashTable(HashFunc *hash_func): HashTable<K, V>(hash_func) {
    }
private:
    virtual unsigned long get_next_pos(unsigned long pos, unsigned long step) {
        // TODO
        return (pos + step * (step + 1) / 2) % HashTable<K, V>::get_table_size();
    }
};

