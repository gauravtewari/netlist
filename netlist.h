/*
 * This header file contains all the data structures for representation of circuit/netlist
 */

#ifndef __NETLIST_H
#define __NETLIST_H


#include <string>
#include <vector>
#include <list>
#include <map>
#include <limits>
using namespace std;
typedef unsigned int u32;

/* Number and Type of gates and nets. */
#define NUM_TYPE_GATES 9
#define NUM_TYPE_NETS 6
enum net_t {INPUT_NET, OUTPUT_NET, IO_NET, HYPEREDGE, INTERNAL_NET, UNDEFINED}; /* Types of nets/edges in circuit */


struct net;

/* Structure for a gate of the circuit or virtex of the graph */
struct gate{
	u32			 		id;			/* Identifier or gate number, can be used to index into the gates adjacency list */
	u32 				type;		/* Type of the gate */
	u32			 		fan_in;		/* Fan in for this gate */
	u32			 		fan_out; 	/* Fan out for this gate */

	std::list<u32> 	nets;	/* List of output edges from this gate. By using sizeof() operator we can get the number of output edges */
	
	/* Constructor for initializing all values */
	gate():id(std::numeric_limits<u32>::max()), type(std::numeric_limits<u32>::max()), fan_in(std::numeric_limits<u32>::max()), 
								fan_out(std::numeric_limits<u32>::max()), nets(std::list<u32> ()){}
	gate(u32 id_val, u32 type_val, u32 fanin, u32 fanout, std::list<u32> &val):id(id_val), type(type_val), fan_in(fanin), fan_out(fanout), nets(val){}
};
	
/* Structure for nets of the circuit or edges of the graph */
struct net{
	u32					id;			/* Identifier or net number, can be used to index into the nets adjacency list */
	int					weight;		/* Weight of this edge */
	net_t				type;		/* Type of net. See net types above */
	
	std::list<u32>	gates;	/* Gates driven by this net */
									/* Number of gates driven by this net can be found by the size of this list */
									
	/* Constructor for initializing all values */
	net():id(std::numeric_limits<u32>::max()), weight(std::numeric_limits<int>::min()), type(UNDEFINED), gates(std::list<u32>()){}
	net(u32 &id_val, int &weight_val, net_t &type_val, list<u32> &val):id(id_val), 
												 weight(weight_val), type(type_val),  gates(val){}
};

/* 
 * This class abstracts complete circuit or netlist. It contains the adjacency lists
 * for gates and edges. It also contains the statistics of the complete netlist.
 * Note: Some of the statistics will be updated after calling the appropriate function in main.
 */
class circuit{
	public:
	u32			num_gates; 		/* Total Number of gates */
	u32 		num_nets; 			/* Total Number of edges */
	
	u32			gate_type[NUM_TYPE_GATES];		/* Number of Gates of each type */
	u32			net_type[NUM_TYPE_NETS]; 		/* Number of Nets of each type */
	
	/* 
	 * This data structure represents number of gates of type T that drive atleast one fanout of the same type
	 * Index represents the type of the gate as seen from gate_t, and the value at that index represents the number of gates
	 * This data structure is updated after calling 'update_same_faninout()' function.
	 */
	u32			same_fanout[NUM_TYPE_GATES];

	/* This data structure represents number of gates of type T that are driven by atleast one gate of the same type 
	 * index represents the type of the gate as seen from gate_t, and the value at that index represents the number of gates
	 * This data structure is updated after calling 'update_same_faninout()' function
	 */
	u32			same_fanin[NUM_TYPE_GATES];
	
	std::vector<struct gate> gate_list;		/* Adjacency list indexed by gates */
	std::vector<struct net> net_list;		/* Adjacency list indexed by nets */
	
	std::map<std::string, int> edge_map;	/* Maps the name of the edges with the corresponding index in edges adjacency list */
	std::map<std::string, u32> gate_t;		/* Maps the name of the gates with there types ( an integer for easy calculation ) */

	public:
	/* Constructor for initializing all values to zero */
	circuit():num_gates(0), num_nets(0){
		for(int i=0; i<NUM_TYPE_GATES; i++) {gate_type[i]=0; same_fanout[i]=0; same_fanin[i]=0;}
		for(int i=0; i<NUM_TYPE_NETS; i++) net_type[i]=0;
	}
	
	/* For description of these function see corresponding .cpp file */
	u32 add_output_gate();
	u32 add_gate(u32 type);
	u32 add_net(net_t type, string &tag);
	int add_outnet_to_gate(u32 gate_id, u32 net_id);
	int add_gate_to_net(u32 net_id, u32 gate_id);
	int update_fan_out();
	u32 parse_input(const char *file);
	int add_input_edge(string &str);
	int add_output_edge(string &str);
	int update_same_faninout();
	string get_edge_name(int id);
	u32 get_gate_type(string str);
	string get_gate_name(u32 id);
};

#endif
