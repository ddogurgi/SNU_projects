// ECE 430.322: Computer Organization
// Lab 4: Memory System Simulation

/**
 *
 * This is the base cache structure that maintains and updates the tag store
 * depending on a cache hit or a cache miss. Note that the implementation here
 * will be used throughout Lab 4. 
 */

#include "cache_base.h"

#include <cmath>
#include <string>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
/**
 * This allocates an "assoc" number of cache entries per a set
 * @param assoc - number of cache entries in a set
 */
cache_set_c::cache_set_c(int assoc) {
  m_entry = new cache_entry_c[assoc];
  m_assoc = assoc;
}

// cache_set_c destructor
cache_set_c::~cache_set_c() {
  delete[] m_entry;
}

/**
 * This constructor initializes a cache structure based on the cache parameters.
 * @param name - cache name; use any name you want
 * @param num_sets - number of sets in a cache
 * @param assoc - number of cache entries in a set
 * @param line_size - cache block (line) size in bytes
 *
 * @note You do not have to modify this (other than for debugging purposes).
 */
cache_base_c::cache_base_c(std::string name, int num_sets, int assoc, int line_size) {
  m_name = name;
  m_num_sets = num_sets;
  m_line_size = line_size;

  m_set = new cache_set_c *[m_num_sets];

  for (int ii = 0; ii < m_num_sets; ++ii) {
    m_set[ii] = new cache_set_c(assoc);

    // initialize tag/valid/dirty bits
    for (int jj = 0; jj < assoc; ++jj) {
      m_set[ii]->m_entry[jj].m_valid = false;
      m_set[ii]->m_entry[jj].m_dirty = false;
      m_set[ii]->m_entry[jj].m_tag   = 0;
    }
  }

  // initialize stats
  m_num_accesses = 0;
  m_num_hits = 0;
  m_num_misses = 0;
  m_num_writes = 0;
  m_num_writebacks = 0;
}

// cache_base_c destructor
cache_base_c::~cache_base_c() {
  for (int ii = 0; ii < m_num_sets; ++ii) { delete m_set[ii]; }
  delete[] m_set;
}

/** 
 * This function looks up in the cache for a memory reference.
 * This needs to update all the necessary meta-data (e.g., tag/valid/dirty) 
 * and the cache statistics, depending on a cache hit or a miss.
 * @param address - memory address 
 * @param access_type - read (0), write (1), instruction fetch (2) or write back(3)
 * @param is_fill - if the access is for a cache fill
 * @param return "true" on a hit; "false" otherwise.
 */
bool cache_base_c::access(addr_t address, int access_type, bool is_fill) {
  ////////////////////////////////////////////////////////////////////
  // TODO: Write the code to implement this function
  ////////////////////////////////////////////////////////////////////
  //address가 32비트, tag/index/offset bit가 각각 32-log2(num_sets)-log2(line_size)/log2(num_sets)/log2(line_size)일 것
  addr_t tag; int index, offset; bool hit = false;
  int invalididx = -1; int updateidx;
  m_addr_victim = 0; m_addr_d_victim = 0;
  offset = address & ((1 << int(log2(m_line_size))) - 1);
  tag = address >> int(log2(m_num_sets) + log2(m_line_size));
  index = (address >> int(log2(m_line_size))) & ((1 << int(log2(m_num_sets))) - 1);
  for(int jj = 0; jj < m_set[index]->m_assoc; jj++){
    if(m_set[index]->m_entry[jj].m_tag == tag && m_set[index]->m_entry[jj].m_valid == true){ //cache hit
      hit = true;
      if(hit){
        if(access_type != WRITE_BACK){
          std::list<int>::iterator itr = find(m_set[index]->LRU.begin(), m_set[index]->LRU.end(), jj);
          m_set[index]->LRU.erase(itr);
          m_set[index]->LRU.push_front(jj);
        }
        if(access_type == WRITE || access_type == WRITE_BACK)//write hit or write back
          m_set[index]->m_entry[jj].m_dirty = true;
      }
      break;
    }
    if(m_set[index]->m_entry[jj].m_valid == false)
      invalididx = jj;
  }
  if(!hit){ //cache miss
    if(is_fill){
      /*
      if(access_type == WRITE_BACK)
        std::cout << "WB MISS" << '\n';
      */
      if(invalididx != -1){ //with invalid bit
        updateidx = invalididx;
        m_set[index]->LRU.push_front(updateidx);
      }
      else{ //with no invalid bit and tag mismatch
        updateidx = m_set[index]->LRU.back();
        m_set[index]->LRU.pop_back();
        m_set[index]->LRU.push_front(updateidx);
        m_addr_victim = (m_set[index]->m_entry[updateidx].m_tag << int(log2(m_num_sets) + log2(m_line_size))) | addr_t(index << int(log2(m_line_size))); //address of victim for back-invalidation(part 3)
        //std::cout << m_set[index]->m_entry[updateidx].m_tag << ' ' << index << ' ' << m_addr_victim << '\n';
        //printf("%lx %d %lx\n", m_set[index]->m_entry[updateidx].m_tag, index, m_addr_victim);
        if(m_set[index]->m_entry[updateidx].m_dirty == true){
          m_num_writebacks++;
          m_addr_d_victim = m_addr_victim; //address of dirty victim for write-back(part 2)
        }
      }
      m_set[index]->m_entry[updateidx].m_valid = true;
      m_set[index]->m_entry[updateidx].m_dirty = false;
      m_set[index]->m_entry[updateidx].m_tag = tag;
      if(access_type == WRITE)//write miss
        m_set[index]->m_entry[updateidx].m_dirty = true;
    }
  }

  m_num_accesses += (hit && !(is_fill))||(is_fill && (access_type != WRITE_BACK));
  m_num_hits += hit && ((hit && !(is_fill))||(is_fill && (access_type != WRITE_BACK)));
  m_num_misses += !(hit) && ((hit && !(is_fill))||(is_fill && (access_type != WRITE_BACK)));
  m_num_writes += (access_type == 1) && ((hit && !(is_fill))||(is_fill && (access_type != WRITE_BACK)));
  return hit;
}

std::pair<bool, bool> cache_base_c::back_invalidation(addr_t address){
  //back invalidation in L1 when L2 cache block is evicted
  bool hit = false;
  bool dirty = false;
  addr_t tag;
  int index, offset;
  tag = address >> int(log2(m_num_sets) + log2(m_line_size));
  index = (address >> int(log2(m_line_size))) & ((1 << int(log2(m_num_sets))) - 1);
  for(int jj = 0; jj < m_set[index]->m_assoc; jj++){
    if(m_set[index]->m_entry[jj].m_tag == tag && m_set[index]->m_entry[jj].m_valid == true){
      m_set[index]->m_entry[jj].m_valid = false;
      hit = true;
      if(m_set[index]->m_entry[jj].m_dirty == true){
        dirty = true;
        m_set[index]->m_entry[jj].m_dirty = false;
      }
      break;
    }
  }
  return std::make_pair(hit, dirty);
}
addr_t cache_base_c::get_addr_d_victim(){
  return m_addr_d_victim;
}

addr_t cache_base_c::get_addr_victim(){
  return m_addr_victim;
}

/**
 * Print statistics (DO NOT CHANGE)
 */
void cache_base_c::print_stats() {
  std::cout << "------------------------------" << "\n";
  std::cout << m_name << " Hit Rate: "          << (double)m_num_hits/m_num_accesses*100 << " % \n";
  std::cout << "------------------------------" << "\n";
  std::cout << "number of accesses: "    << m_num_accesses << "\n";
  std::cout << "number of hits: "        << m_num_hits << "\n";
  std::cout << "number of misses: "      << m_num_misses << "\n";
  std::cout << "number of writes: "      << m_num_writes << "\n";
  std::cout << "number of writebacks: "  << m_num_writebacks << "\n";
}


/**
 * Dump tag store (for debugging) 
 * Modify this if it does not dump from the MRU to LRU positions in your implementation.
 */
void cache_base_c::dump_tag_store(bool is_file) {
  auto write = [&](std::ostream &os) { 
    os << "------------------------------" << "\n";
    os << m_name << " Tag Store\n";
    os << "------------------------------" << "\n";

    for (int ii = 0; ii < m_num_sets; ii++) {
      for (int jj = 0; jj < m_set[0]->m_assoc; jj++) {
        os << "[" << (int)m_set[ii]->m_entry[jj].m_valid << ", ";
        os << (int)m_set[ii]->m_entry[jj].m_dirty << ", ";
        os << std::setw(10) << std::hex << m_set[ii]->m_entry[jj].m_tag << std::dec << "] ";
      }
      os << "\n";
    }
  };

  if (is_file) {
    std::ofstream ofs(m_name + ".dump");
    write(ofs);
  } else {
    write(std::cout);
  }
}