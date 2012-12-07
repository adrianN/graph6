/*
	This provides a parser for the graph6 format by Brendan McKay as used in nauty.
*/
#include <vector>
#include <stdexcept>
#include <cassert>


template<typename Input>
unsigned long read_graph6_matrix(Input& in, std::vector<bool>& matrix) {
	unsigned long num_nodes;
	const int packet_size = 6;

	//Read the header
	if (*in=='>') { //We have the optional >>graph6<< header
		unsigned char h[] = ">>graph6<<";
		for(unsigned int i=0; i<(sizeof(h)/sizeof(unsigned char))-1; ++i) {
			if (h[i] != *in++) {
				throw std::invalid_argument(
					"The input is malformed. "
					"It seems to start with >>graph6<< but really doesn't."
					);
			}
		}
	}
	//now we're one past the optional header
	//parse the number of nodes
	if (*in<126) {
		//6-bit encoding
		num_nodes = *in-63;
	} else if (*in++ == 126) {
		unsigned int packets = 3;
		if (*in == 126) {
			//36-bit encoding
			packets++;
			in++;
		}
		for(unsigned int i =0; i<packets*packet_size; ++i) {
			unsigned char packet = (*in++) - 63;
			if(packet>(1<<packet_size)) {
				throw std::invalid_argument(
					"The input is malformed. "
					"It contains a non-printable ascii-char in the number of nodes encoding.");
			}
			num_nodes += packet;
			num_nodes *= (1<<packet_size);
		}
	} else {
		assert(*in>126);
		throw std::invalid_argument(
			"The input is malformed. "
			"The number of nodes is not correctly encoded, the first byte is >126."
			);
	}
	//now we parse the matrix
	unsigned int expected_column_entries = 1;
	unsigned int read_column_entries = 0;
	long read = 0;
	while(true) {
		unsigned char packet = (*in++) - 63;
		read++;
		if(packet>(1<<packet_size)) {
			throw std::invalid_argument(
				"The input is malformed. "
				"It contains a non-printable ascii-char in the matrix encoding.");
		}
		for(int i=packet_size; i>=0; i--) {
			matrix.push_back(packet & (1<<i));
			read_column_entries++;
			if (expected_column_entries == read_column_entries) {
				read_column_entries = 0;
				expected_column_entries += 1;
			}
			if (expected_column_entries>num_nodes) {
				in++; //skip newline, in case we'll be called again
				return num_nodes;
			}
		}
	}
}
