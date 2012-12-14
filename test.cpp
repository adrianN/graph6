#include "graph6.hpp"
#include "boost_helper.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>
#include <stdexcept>
#include <utility>
#include <boost/graph/adjacency_list.hpp>

using namespace std;


typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> Graph;
typedef Graph::vertex_descriptor Vertex;

int main() {
	ifstream file("./output.g6");
	istream_iterator<unsigned char> eos;
	istream_iterator<unsigned char> iit(file);
	typedef read_graph6_edges<istream_iterator<unsigned char> > edge_reader;

	edge_reader end;

	while(iit!=eos) {
		Graph g = read_graph6<Graph>(iit);
		cout << boost::num_vertices(g) << endl;
		write_graph6(g, boost::edges(g).first, boost::edges(g).second, cout);
		cout << "==========" << endl;
	}

	return 0;
}