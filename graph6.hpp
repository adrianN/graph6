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
struct read_graph6_edges : std::iterator<
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

	int column;
	int row;
	int bit_pos;
	unsigned char cur_byte;

	read_graph6_edges() : eos(true),
		num_nodes(0),
		packet_size(5),
		column(1),
		row(-1),
		bit_pos(-1) {}

	~read_graph6_edges() {}

	read_graph6_edges(input_iterator& start) : 
		_in(&start), 
		eos(false),
		num_nodes(0),
		packet_size(5),
		column(1),
		row(-1),
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
				if(packet>(1<<(packet_size+1))) {
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
		++in;
	}

	void next_edge() {
		unsigned int bit = 0;
		while(!bit && !eos) {
			if (++row==column) {
				column+=1;
				row = 0;
			}
			if (column>=num_nodes) {
				eos = true;
				break;
			}
			if (bit_pos<0) {
				cur_byte = (*(*_in)) - 63;

				++(*_in);
				if(cur_byte>(1<<(packet_size+1))) {
					throw std::invalid_argument(
						"The input is malformed. "
						"It contains a non-printable ascii-char in the matrix encoding.");
				}
				bit_pos = packet_size;
			}
			bit = cur_byte & (1<<(bit_pos--));
		}
	}

	const std::pair<int,int> operator*() const {
		return std::pair<int,int>(column,row);
	} 

	read_graph6_edges<input_iterator>& operator++() {
		next_edge();
		return *this;
	}

	read_graph6_edges<input_iterator> operator++(int) {
		read_graph6_edges<input_iterator> tmp = *this;
		++*this;
		return tmp;
	}

	bool operator==(read_graph6_edges<input_iterator>& other) {
		return (this->eos && other.eos) || (this->_in == other._in);
	}

	bool operator!=(read_graph6_edges<input_iterator>& other) {
		return !(*this==other);
	}

};



