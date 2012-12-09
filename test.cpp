#include "graph6.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>
#include <stdexcept>
#include <utility>
using namespace std;

int main() {
	ifstream file("./output.g6");
	istream_iterator<unsigned char> eos;
	istream_iterator<unsigned char> iit(file);
	typedef read_graph6_edges<istream_iterator<unsigned char> > edge_reader;

	edge_reader end;

	for(int i=0; i<3; i++) {
		edge_reader edges(iit);
		for(; edges!=end; ++edges) {
			pair<int,int> edge = *edges;
			cout << edge.first << " " << edge.second << endl;
		}
		cout << "==========" << endl;
	}

	return 0;
}