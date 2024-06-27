// ECE 430.322: Computer Organization
// Lab 4: Memory System Simulation

#ifndef __CACHE_BASE_H__
#define __CACHE_BASE_H__

#include <cstdint>
#include <string>
#include <list>

typedef enum request_type_enum {
  READ = 0,
  WRITE = 1,
  INST_FETCH = 2,
  WRITE_BACK = 3
} request_type;

using addr_t = uint64_t;

///////////////////////////////////////////////////////////////////
class cache_entry_c
{
public:
  cache_entry_c() {} ;
  bool   m_valid;    // valid bit for the cacheline
  bool   m_dirty;    // dirty bit 
  addr_t m_tag;      // tag for the line
  friend class cache_base_c;
};

///////////////////////////////////////////////////////////////////
class cache_set_c 
{
public:
    cache_set_c(int assoc);
    ~cache_set_c();

    cache_entry_c *m_entry;  // array of cache entries. 
    int m_assoc;             // number of cache blocks in a cache set
    std::list<int> LRU;      // True LRU stack in a cache set(+)
  ///////////////////////////////////////////////////////////////////
  // TODO: Maintain the LRU stack for this set
  ///////////////////////////////////////////////////////////////////
};

///////////////////////////////////////////////////////////////////
class cache_base_c 
{
public:
  cache_base_c();
  cache_base_c(std::string name, int num_set, int assoc, int line_size);
  ~cache_base_c();

  bool access(addr_t address, int access_type, bool is_fill);
  std::pair<bool, bool> back_invalidation(addr_t address);
  void print_stats();
  void dump_tag_store(bool is_file);  // false: dump to stdout, true: dump to a file
  addr_t get_addr_d_victim();
  addr_t get_addr_victim();

private:
  std::string m_name;     // cache name
  int m_num_sets;         // number of sets
  int m_line_size;        // cache line size
  
  cache_set_c **m_set;    // cache data structure
  addr_t m_addr_d_victim; // address of dirty victim(+)
  addr_t m_addr_victim;   // address of victim for back_invalidation(+)
  // cache statistics
  int m_num_accesses; 
  int m_num_hits; 
  int m_num_misses; 
  int m_num_writes;
  int m_num_writebacks;
};

#endif // !__CACHE_BASE_H__ 
