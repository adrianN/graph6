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
	vector<bool> v;
	try {
	cout << it_read_matrix(iit,v) << endl;

	cout << file.tellg() << endl;
	file.seekg(0,ios::beg);
	iit = istream_iterator<unsigned char>(file);
	read_graph6_matrix(iit, v);
	for(vector<bool>::iterator it = v.begin(); it!=v.end(); ++it) {
		cout << *it << " ";
	}
	cout << file.tellg() << endl;
	} catch (invalid_argument e) {
		cout << e.what() << endl;
		cout << file.tellg() << endl;
	}

	// int graphs = 0;
	// try {
	// 	while(iit != eos) {
	// 		vector<bool> v;
	// 		unsigned long n = read_graph6_matrix(iit, v);
	// 		cout << "Read " << (++graphs) << " graphs, this one has " << n << endl;
	// 		cout << "Next char " << *iit << endl;
	// 	}
	// } catch (invalid_argument e) {
	// 	cout << e.what() << endl;
	// }
	return 0;
}