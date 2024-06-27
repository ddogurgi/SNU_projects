#ifndef __PRIM_MINIMUM_SPANNING_TREE_H_
#define __PRIM_MINIMUM_SPANNING_TREE_H_

#include <optional>
#include <vector>
#include "graph.hpp"
/* Feel free to add more standard library headers */

#include <limits>
#include <queue>
/* Returns the vector of edges in the MST, or std::nullopt if MST does
 * not exist */

std::optional<edges_t>
prim_minimum_spanning_tree(Graph& g, vertex_t src) {
	std::vector<std::vector<std::tuple<vertex_t, edge_weight_t>>> adj_list = g.get_adj_list();
    std::size_t n = g.get_num_vertices();
	const edge_weight_t inf = std::numeric_limits<edge_weight_t>::infinity();
	std::vector<edge_weight_t> distance(n, inf); distance[src] = 0.0f;
	std::vector<bool> visited(n, false);
	std::priority_queue<std::tuple<edge_weight_t, vertex_t, edge_t>> q;
	std::vector<edge_t> answer;
	q.push(std::make_tuple(distance[src], src, std::make_tuple(0,0,0.0f)));
	while(!q.empty()){
		edge_weight_t cur_dist; vertex_t cur_v; edge_t edge_v;
		std::tie(cur_dist, cur_v, edge_v) = q.top();
		q.pop();
		if(visited[cur_v]) continue;
		if(cur_v != src)
			answer.push_back(edge_v);
		visited[cur_v] = true;
		for(std::tuple<vertex_t, edge_weight_t> dw : adj_list[cur_v]){
			if(std::get<1>(dw) < distance[std::get<0>(dw)]){
				distance[std::get<0>(dw)] = std::get<1>(dw);
				q.push(std::make_tuple(-distance[std::get<0>(dw)], std::get<0>(dw), std::make_tuple(cur_v, std::get<0>(dw), std::get<1>(dw))));
			}
		}
	}
	if(answer.size() != n - 1)
		return std::nullopt;
	return answer;
}
#endif // __PRIM_MINIMUM_SPANNING_TREE_H_
