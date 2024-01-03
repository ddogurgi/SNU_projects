// ECE 430.322: Computer Organization
// Lab 4: Memory System Simulation

#include "cache.h"
#include <cstring>
#include <list>
#include <cassert>
#include <iostream>
#include <cmath>

cache_c::cache_c(std::string name, int level, int num_set, int assoc, int line_size, int latency)
    : cache_base_c(name, num_set, assoc, line_size) {

  // instantiate queues
  m_in_queue   = new queue_c();
  m_out_queue  = new queue_c();
  m_fill_queue = new queue_c();
  m_wb_queue   = new queue_c();

  m_in_flight_wb_queue = new queue_c();

  m_id = 0;

  m_prev_i = nullptr;
  m_prev_d = nullptr;
  m_next = nullptr;
  m_memory = nullptr;

  m_latency = latency;
  m_level = level;

  // clock cycle
  m_cycle = 0;
  
  m_num_backinvals = 0;
  m_num_writebacks_backinval = 0;
}

cache_c::~cache_c() {
  delete m_in_queue;
  delete m_out_queue;
  delete m_fill_queue;
  delete m_wb_queue;
  delete m_in_flight_wb_queue;
}

/** 
 * Run a cycle for cache (DO NOT CHANGE)
 */
void cache_c::run_a_cycle() {
  // process the queues in the following order 
  // wb -> fill -> out -> in
  process_wb_queue();
  process_fill_queue();
  process_out_queue();
  process_in_queue();
  ++m_cycle;
}

void cache_c::configure_neighbors(cache_c* prev_i, cache_c* prev_d, cache_c* next, simple_mem_c* memory) {
  m_prev_i = prev_i;
  m_prev_d = prev_d;
  m_next = next;
  m_memory = memory;
}

/**
 *
 * [Cache Fill Flow]
 *
 * This function puts the memory request into fill_queue, so that the cache
 * line is to be filled or written-back.  When we fill or write-back the cache
 * line, it will take effect after the intrinsic cache latency.  Thus, this
 * function adjusts the ready cycle of the request; i.e., a new ready cycle
 * needs to be set for the request.
 *
 */
bool cache_c::fill(mem_req_s* req) {
  req->m_rdy_cycle += m_latency;
  return m_fill_queue->push(req);
}

/**
 * [Cache Access Flow]
 *
 * This function puts the memory request into in_queue.  When accessing the
 * cache, the outcome (e.g., hit/miss) will be known after the intrinsic cache
 * latency.  Thus, this function adjusts the ready cycle of the request; i.e.,
 * a new ready cycle needs to be set for the request .
 */
bool cache_c::access(mem_req_s* req) { 
  req->m_rdy_cycle += m_latency;
  return m_in_queue->push(req);
}

/** 
 * This function processes the input queue.
 * What this function does are
 * 1. iterates the requests in the queue
 * 2. performs a cache lookup in the "cache base" after the intrinsic access time
 * 3. on a cache hit, forward the request to the prev's fill_queue or the processor depending on the cache level.
 * 4. on a cache miss, put the current requests into out_queue
 */
void cache_c::process_in_queue() {
  for (auto I = m_in_queue->m_entry.begin(); I != m_in_queue->m_entry.end();){ // 1
    mem_req_s *req = *I;
    //printf("%lx\n", *I);
    if (m_cycle >= req->m_rdy_cycle){
      m_in_queue->pop(req);
      bool hit = cache_base_c::access(req->m_addr, req->m_type, false); // 2
      if (hit){ // cache hit
        if(m_level == MEM_L1)
          done_func(req);
        else if(req->m_type < INST_FETCH || m_prev_i == nullptr){
          m_prev_d->fill(req);
        }
        else if(req->m_type == INST_FETCH && m_prev_i != nullptr)
          m_prev_i->fill(req);
      }
      else{ // cache miss
        if (m_level == MEM_L1 && req->m_type == REQ_DSTORE){                           // STORE miss in L1
          req->m_dirty = true;      // Make request dirty (In order to update cacheline when fill)
          req->m_type = REQ_DFETCH; // STORE miss in L1 becomes READ request for L2
        }
        m_out_queue->push(req); // put request to out queue
      }
    }
    else{
      I++;
    }
  }
} 

/** 
 * This function processes the output queue.
 * The function pops the requests from out_queue and accesses the next-level's cache or main memory.
 * CURRENT: There is no limit on the number of requests we can process in a cycle.
 */
void cache_c::process_out_queue() {
  //m_out_queue
  while(!(m_out_queue->empty())){
    mem_req_s *req = m_out_queue->m_entry[0];
    m_out_queue->pop(req);
    if(m_next != nullptr)
      if(req->m_type != REQ_WB)
        m_next->access(req);
      else
        m_next->fill(req);
    else{
      m_memory->access(req);
    }
  }
}


/** 
 * This function processes the fill queue.  The fill queue contains both the
 * data from the lower level and the dirty victim from the upper level.
 */

void cache_c::process_fill_queue() {
  //m_fill_queue
  for (auto I = m_fill_queue->m_entry.begin(); I != m_fill_queue->m_entry.end();){
    mem_req_s *req = *I;
    //printf("%lx\n", *I);
    if(m_cycle >= req->m_rdy_cycle){
      m_fill_queue->pop(req);
      if(m_level == MEM_L1 && req->m_dirty){
        req->m_type = REQ_DSTORE;
      }
      bool hit = cache_base_c::access(req->m_addr, req->m_type, true);
      if(req->m_type == REQ_WB){
        m_in_flight_wb_queue->pop(req);
        if(req != NULL)
          free(req);
        continue;
      }
      if(m_level == MEM_L2 && get_addr_victim()){ //L2 eviction
        if(m_prev_d != nullptr){
          std::pair<bool, bool> backind = m_prev_d -> cache_base_c::back_invalidation(get_addr_victim());
          if(backind.first)
            m_prev_d->m_num_backinvals++;
          if(backind.second){ //back invalidation block is dirty
            m_prev_d->m_num_writebacks_backinval++;
            if(!get_addr_d_victim()){
              mem_req_s *binv_wb_req = new mem_req_s(get_addr_victim(), REQ_WB);
              binv_wb_req->m_in_cycle = m_cycle;
              binv_wb_req->m_rdy_cycle = m_cycle;
              binv_wb_req->m_done = false;
              binv_wb_req->m_dirty = true;
              m_wb_queue->push(binv_wb_req);
              //m_memory->m_in_flight_wb_queue->push(binv_wb_req);
            }
          }
        }
        if(m_prev_i != nullptr){
          std::pair<bool, bool> backind = m_prev_i -> cache_base_c::back_invalidation(get_addr_victim());
          if(backind.first)
            m_prev_i->m_num_backinvals++;
        }
      }
      if (get_addr_d_victim()){ // put dirty victim to write back queue
        mem_req_s *wb_req = new mem_req_s(get_addr_d_victim(), REQ_WB);
        wb_req->m_in_cycle = m_cycle;
        wb_req->m_rdy_cycle = m_cycle;
        wb_req->m_done = false;
        wb_req->m_dirty = true;
        m_wb_queue->push(wb_req);
        if(m_next != nullptr)
          m_next->m_in_flight_wb_queue->push(wb_req);
        //else
          //m_memory->m_in_flight_wb_queue->push(wb_req);
      }
      if(m_level == MEM_L1)
        done_func(req);
      else if(req->m_type == INST_FETCH && m_prev_i != nullptr)
        m_prev_i->fill(req);
      else{
        m_prev_d->fill(req);
      }
    }
    else{
      I++;
    }
  }
}

/** 
 * This function processes the write-back queue.
 * The function basically moves the requests from wb_queue to out_queue.
 * CURRENT: There is no limit on the number of requests we can process in a cycle.
 */
void cache_c::process_wb_queue() {
  //m_wb_queue
  while(!(m_wb_queue->empty())){
    mem_req_s *req = m_wb_queue->m_entry[0];
    m_wb_queue->pop(req);
    m_out_queue->push(req);
  }
}

/**
 * Print statistics (DO NOT CHANGE)
 */
void cache_c::print_stats() {
  cache_base_c::print_stats();
  std::cout << "number of back invalidations: " << m_num_backinvals << "\n";
  std::cout << "number of writebacks due to back invalidations: " << m_num_writebacks_backinval << "\n";
}
