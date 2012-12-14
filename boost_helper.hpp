#ifndef BOOST_HELPER_HPP
#define BOOST_HELPER_HPP

#include <boost/graph/graph_traits.hpp>
#include "graph6.hpp"
#include <stdexcept>
#include <utility>
#include <iostream>
#include <cassert>

template<typename Graph, typename Input>
Graph read_graph6(Input& in) {
	read_graph6_edges<Input> edges(in);
	read_graph6_edges<Input> last;
	Graph g(edges, last, edges.num_nodes);
	if (*in++ != '\n')
		throw std::invalid_argument("I expected a newline after this graph, but didn't get any");
	return g;
}

template<typename Graph, typename EdgeIterator, typename Output>
void write_graph6(const Graph& g, EdgeIterator start, EdgeIterator end, Output& out) {
	typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
	const unsigned int n = num_vertices(g);
	std::vector<unsigned int> adj_matrix(n*(n-1)/2, 0);
	for(; start!=end; ++start) {
		Vertex s = source(*start,g);
		Vertex t = target(*start,g);

		if (s==t) {
			throw std::invalid_argument("Graphs with loops are not supported");
		}
		if (s<t) {
			std::swap(s,t);
		}

		const unsigned int position = (s-1)*s/2 + t;
		adj_matrix[position] = 1;
	}
	// std::cout << std::endl;
	// for(int i = 0; i<adj_matrix.size(); ++i) {
	// 	std::cout << adj_matrix[i] << " ";
	// }
	// std::cout << std::endl;
	write_graph6(out, n, adj_matrix.begin(), adj_matrix.end());
}

#endif