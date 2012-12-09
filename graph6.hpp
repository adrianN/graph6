/*
	This provides a parser for the graph6 format by Brendan McKay as used in nauty.
*/
#include <vector>
#include <stdexcept>
#include <cassert>
#include <iterator>
#include <utility>
#include <iostream>


template<typename input_iterator>
struct read_graph6 : std::iterator<
	std::input_iterator_tag,
	std::pair<int, int>,
	ptrdiff_t,
	std::pair<int,int>*,
	std::pair<int,int>&>
{
	unsigned long num_nodes;
	const int packet_size;
	input_iterator* _in;
	bool eos;

	unsigned int column;
	unsigned int row;
	int bit_pos;
	unsigned char cur_byte;

	read_graph6() : eos(true),
		num_nodes(0),
		packet_size(6),
		column(0),
		row(0),
		bit_pos(-1) {}

	read_graph6(input_iterator& start) : 
		_in(&start), 
		eos(false),
		num_nodes(0),
		packet_size(6),
		column(0),
		row(0),
		bit_pos(-1)  
	{
		parse_num_nodes();
		++*this;

	}

	void parse_num_nodes() {
		input_iterator& in = *_in;
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
	}

	void next_edge() {
		unsigned int bit = 0;
		while(!bit && !eos) {
			if (bit_pos<0) {
				cur_byte = (*(*_in)) - 63;
				++(*_in);
				if(cur_byte>(1<<packet_size)) {
					throw std::invalid_argument(
						"The input is malformed. "
						"It contains a non-printable ascii-char in the matrix encoding.");
				}
				bit_pos = packet_size;
			}
			bit = cur_byte & (1<<(bit_pos--));
			row++;
			if (row>=column) {
				column+=1;
				row = 0;
			}
			if (column>num_nodes) {
				eos = true;
			}
		}
	}

	const std::pair<int,int> operator*() const {
		return std::pair<int,int>(column,row);
	} 

	read_graph6<input_iterator>& operator++() {
		next_edge();
	}

	read_graph6<input_iterator> operator++(int) {
		read_graph6 tmp = *this;
		++*this;
		return tmp;
	}

	bool operator==(read_graph6<input_iterator>& other) {
		return (this->eos && other.eos) || (this->_in == other._in);
	}

	bool operator!=(read_graph6<input_iterator>& other) {
		return !(*this==other);
	}

};

template<typename Input>
unsigned long it_read_matrix(Input& in, std::vector<bool>& matrix) {

	read_graph6<Input> edges(in);
	read_graph6<Input> end;

	while(edges != end) {
		std::cout << (*edges).first << " " << (*edges).second << std::endl;
		++edges;
	}
	return edges.num_nodes;

}

/* 	Reads a graph from in, stores its matrix in the vector (column wise, i.e. 
	(0,1),(0,2),(1,2),(0,3),(1,3),(2,3),...,(n-1,n)) and returns the number of nodes */ 
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


