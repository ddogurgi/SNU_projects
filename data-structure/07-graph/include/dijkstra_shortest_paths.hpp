#ifndef __DIJKSTRA_SHORTEST_PATHS_H_
#define __DIJKSTRA_SHORTEST_PATHS_H_

#include <unordered_map>
#include <optional>
#include <tuple>
/* Feel free to add more standard library headers */

#include "graph.hpp"
#include <limits>
#include <queue>

/* Given a vertex `v`, `dijkstra_shortest_path` is:
 * - `nullopt` if `v` is not reacheble from `src`.
 * - {`u`, `d`} where `u` is the predecessor of `v` and `d` is the distance
 *   from `src`
 */

std::unordered_map<vertex_t,
    std::optional<std::tuple<vertex_t, edge_weight_t>>>
dijkstra_shortest_path(Graph& g, vertex_t src) {
    std::vector<std::vector<std::tuple<vertex_t, edge_weight_t>>> adj_list = g.get_adj_list();
    std::size_t n = g.get_num_vertices();
    const edge_weight_t inf = std::numeric_limits<edge_weight_t>::infinity();
	std::vector<edge_weight_t> distance(n, inf); distance[src] = 0.0f;
	std::vector<bool> visited(n, false);
	std::priority_queue<std::tuple<edge_weight_t, vertex_t, vertex_t>> q;
    std::unordered_map<vertex_t, std::optional<std::tuple<vertex_t, edge_weight_t>>> answer;
    for(size_t i = 0; i < n; i++)
        answer[i] = std::nullopt;
    q.push(std::make_tuple(distance[src], src, n));
    while(!q.empty()){
		edge_weight_t cur_dist; vertex_t cur_v; vertex_t parent_v;
		std::tie(cur_dist, cur_v, parent_v) = q.top(); cur_dist = -cur_dist;
        q.pop();
		if(visited[cur_v]) continue;
		answer[cur_v] = std::make_tuple(parent_v, cur_dist);
		visited[cur_v] = true;
		for(std::tuple<vertex_t, edge_weight_t> dw : adj_list[cur_v]){
			if(std::get<1>(dw) + cur_dist < distance[std::get<0>(dw)]){
				distance[std::get<0>(dw)] = std::get<1>(dw) + cur_dist;
				q.push(std::make_tuple(-distance[std::get<0>(dw)], std::get<0>(dw), cur_v));
			}
		}
	}

    return answer;
}

#endif // __DIJKSTRA_SHORTEST_PATHS_H_
