#include "graph6.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>
#include <stdexcept>
using namespace std;

int main() {
	ifstream file("./output.g6");
	istream_iterator<unsigned char> eos;
	istream_iterator<unsigned char> iit(file);
	istream_iterator<unsigned char> last;
	int graphs = 0;
	try {
		while(iit != eos) {
			vector<bool> v;
			last = iit;
			unsigned long n = read_graph6_matrix(iit, v);
			cout << "Read " << (++graphs) << " graphs, this one has " << n << endl;
			cout << "Next char " << *iit << endl;
		}
	} catch (invalid_argument e) {
		cout << e.what() << endl;
	}
	return 0;
}