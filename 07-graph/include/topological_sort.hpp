#ifndef __TOPOLOGICAL_SORT_H_
#define __TOPOLOGICAL_SORT_H_

#include <vector>
#include "graph.hpp"
/* Feel free to add more standard library headers */
#include <queue>
/* Return _a_ valid topologically sorted sequence of vertex descriptors */
std::vector<vertex_t> topological_sort(Graph& g) {
	std::vector<std::vector<std::tuple<vertex_t, edge_weight_t>>> adj_list = g.get_adj_list();
    std::size_t n = g.get_num_vertices();
	std::vector<std::size_t> indegree(n, 0);
	std::queue<vertex_t> q;
	std::vector<vertex_t> answer;
	for(std::size_t i = 0; i < n; i++)
		for(std::tuple<vertex_t, edge_weight_t> dw : adj_list[i])
			indegree[std::get<0>(dw)]++;
	for(std::size_t i = 0; i < n; i++)
		if(!indegree[i])
			q.push(i);
	while(!q.empty()){
		vertex_t v = q.front();
		answer.push_back(v);
		q.pop();
		for(std::tuple<vertex_t, edge_weight_t> dw : adj_list[v]){
			vertex_t dest = std::get<0>(dw);
			indegree[dest]--;
			if(!indegree[dest])
				q.push(dest);
		}
	}
	return answer;
}

#endif // __TOPOLOGICAL_SORT_H_
