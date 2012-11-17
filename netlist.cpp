#include "netlist.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdlib.h>
using namespace std;

char *trim(char *);

/* parse_input - takes a filename which contains the complete circuit/netlist representation and 
 * generates the corresponding data structures for graph representation.
 * 
 * @file : the input file name containing complete circuit description
 * 
 * Returns 0 on success
 */
u32 circuit::parse_input(const char *file){
	
	char in_seps[]="(=";		/* For normal lines the first separater encountered will be either one of these two */
	char *str, *str_ptr, *token;
	string line;
	u32 inputs =0, outputs=0, gates=0;
	
	/* Open the input file and check for error */
	ifstream in_file(file);
	if(!in_file.is_open()){
		cerr<<"Can't open input file: "<<file<<endl;
		return 1;
	}
	
	/* Read lines from the file */
	while(getline(in_file, line)){
		
		/* Leave empty lines */
		if(line=="") continue;				
		
		/* Leave comments */
		if(line[0] == '#') continue;
		
		/* get c type string for further processing */
		str_ptr = new char [line.size()+1];
		str = str_ptr;									/* Since the pointer is moved while removing the spaces, we might lost the count of allocated memory
															two pointers are kept. One can be moved around	 while the other can be used to free memory. */
		strncpy (str, line.c_str(), line.size()+1);

		/* Check if string is all spaces */
		while(isspace(*str)) str++;
			if(*str == 0) { delete []str_ptr; continue;}	

		/* Now we got rid of all empty and blank lines */
		/* Extract the first tokens from the input line */
		token = strtok( str, in_seps );
		token=trim(token);
	
		if(strncmp(token,"INPUT",6)==0){				/* An input net is found */
			token = strtok( NULL, ")" );
			token=trim(token);
			string t(token);
			if(add_input_edge(t) != 0){
				cerr<<"Error Adding Input Gate after parsing\n";
				exit(-1);
			}
			++inputs;
		}else if(strncmp(token,"OUTPUT",7)==0){ 		/* An output net if found */
				token = strtok( NULL, ")" );
				token=trim(token);
				string t(token);
				if(add_output_edge(t) != 0){
					cerr<<"Error Adding Output Gate after parsing\n";
					exit(-1);
				}
			++outputs;
		}else{											
			/* if neither input nor output then gate description is found */
				
				/* find an entry in net_list for this output net of the gate */
				string t(token);
				u32 onet = add_net(INTERNAL_NET, t);

				/* Getting the gate name (type) */
				token=strtok(NULL, "(");
				token=trim(token);

				/* Add an entry for this gate */
				u32 _gate = add_gate(get_gate_type(token));
				
				/* Add the net as output to the gate */
				add_outnet_to_gate(_gate, onet);

				/* This logic extracts all the input nets from the line */
				/* It can handle some spaces if they are inserted in between */
				/* Now the tokens are separated by comma, last token will either have a ')' character or some spaces followed by ')' at the end*/
				while((token = strtok( NULL, "," )) != NULL ){
					token=trim(token);
					if(token[strlen(token)-1]==')') token[strlen(token)-1]=0;
					token=trim(token);

					/* Get an edge for this net and add the gate to its gates list*/
					string t(token);
					u32 innet = add_net(INTERNAL_NET, t);
					add_gate_to_net(innet, _gate);
				}
				++gates;
		}
		delete []str_ptr;
	}
	in_file.close();
	return 0;
}

/*
 * update_same_faninout - this function updates the number of gates of type T that drive at least one fanout of the same type and
 * the number of gates of type T that has at least one fanin of the same type.
 * 
 * Returns 0 on success
 * Note: After this function is called the corresponding entries are updated in 'same_fanin' and 'same_fanout' 
 * data structures of circuit class.
 */
int circuit::update_same_faninout(){
	u32 input_type = get_gate_type("INPUT");
	u32 output_type = get_gate_type("OUTPUT");
	u32 gtype;
	u32 *fin = new u32[num_gates];
	u32 *fout = new u32[num_gates];
	for(u32 i=0; i<num_gates; i++){ fin[i]=0; fout[i]=0;}
	for(vector<struct gate>::const_iterator i = gate_list.begin(); i != gate_list.end(); i++){
		/* For each gate (except input and output gates )find the edges that are driven by this gate */
		if((i->type != input_type) && (i->type != output_type)){
			gtype = i->type;	/* Note the gate type for comparison */
			for(list<u32>::const_iterator j = i->nets.begin(); j != i->nets.end(); j++){ /* For each edge driven by this gate */
				/* For each edge find the gate it drives */
				for(list<u32>::const_iterator k = net_list[*j].gates.begin(); k != net_list[*j].gates.end(); k++){ /* For each gate driven by this edge */
					if(gate_list[*k].type == gtype){
						fout[i->id]=1;
						fin[*k]=1;
					}
					
				}
			}
		}
	}
	for(u32 i = 0; i < num_gates; i++){
		if(fout[i] == 1){
			same_fanout[gate_list[i].type]++;
		}
		if(fin[i] == 1){
			same_fanin[gate_list[i].type]++;
		}
	}
	delete []fin;
	delete []fout;
	return 0;
}

/*
 * add_gate_to_net - adds a new gate entry to the linked list of the adjacency list of the edges.
 * @net_id: the index of the net entry, in whose linked list the gate is to added
 * @gate_id: the id of the gate entry to be added
 * 
 * Returns 0 on success
 */
int circuit::add_gate_to_net(u32 net_id, u32 gate_id){
	
	/* We assume that gate_id and net_id are valid and the corresponding entries exist */
	if(!net_list[net_id].gates.empty()){
		net_type[net_list[net_id].type]--;
		net_list[net_id].type = HYPEREDGE;
		net_type[net_list[net_id].type]++;
	}
	
	net_list[net_id].gates.push_back(gate_id);
	/* We defer the fan_out updation till later */
	/* Fan in is updated here */
	gate_list[gate_id].fan_in ++;
	return 0;
}

/* add_outnet_to_gate - Adds a new entry to the edges list of the gate entry in the adjacency list
 * @gate_id: the index of the gate entry, where to add the new outgoing edge (net)
 * @net_id: the index of the new net to be added
 * 
 * Returns 0 on success
 */
int circuit::add_outnet_to_gate(u32 gate_id, u32 net_id){
	/* We assume that gate_id and net_id are valid and the corresponding entries exist */
	gate_list[gate_id].nets.push_back(net_id);
	
	/* We can update the fan_out of the gate here but we leave it for later */
	return 0;
}

/*
 * add_net - checks whether the net(edge) with name 'tag' already exists.
 * If no such net exists then it creates new edge with the given type and name
 * @type: the type of the net
 * @tag: the name of the net
 * 
 * Returns the index of the net in the net_list adjacency list
 */
u32 circuit::add_net(net_t type, string &tag){
	struct net temp;

	/* Check if the edge already exists */
	if(edge_map.find(tag) == edge_map.end()){
		/* Allocate a new net entry */
		struct net temp;
		edge_map[tag]=num_nets;
		temp.id = num_nets;
		temp.weight = 0;
		temp.type = type;
		temp.gates = list<u32>();
		net_list.push_back(temp);
		net_type[temp.type]++;
		num_nets++;
		return temp.id;
	}
	else
	 return edge_map[tag];
}

/*
 * add_gate - adds a struct gate entry to the gate_list adjacency list
 * @type: type of the new gate
 * 
 * Returns the index (in the gate_list adjacency list) of the new added gate
 */ 
u32 circuit::add_gate(u32 type){
	/* we have to add a new gate to the gates adjacency list */
	struct gate temp;
	temp.id = num_gates;
	temp.type = type;
	temp.fan_out = 0;
	temp.fan_in = 0;
	temp.nets = list<u32>();
	gate_list.push_back(temp);
	
	num_gates++;
	gate_type[temp.type]++;
	return temp.id;
}

/*
 * add_output_edge - Updates the graph data structures corresponding to OUTPUT(...) line encountered in input file.
 * @str : name of the encountered output edge
 * 
 * Returns 0 for success, other values for failure
 */ 
int circuit::add_output_edge(string &str){
	if(str.empty())
		return -1;
	u32 gate_id, net_id;
	gate_id = add_gate(get_gate_type("OUTPUT"));
	gate_list[gate_id].fan_in = 1;

	net_id = add_net(OUTPUT_NET, str);
	if(net_list[net_id].type == INPUT_NET){
		net_list[net_id].type = IO_NET;
		net_type[INPUT_NET]--;
		net_type[IO_NET]++;
	}
	net_list[net_id].gates.push_back(gate_id);
	
	return 0;
}

/* 
 * add_input_edge - adds an input edge read from the input file to the
 * graphs adjacency lists.
 * @str : the name of the input edge read from input file
 * 
 * Returns 0 on success and other values of error
 */
int circuit::add_input_edge(string &str){
	if(str.empty())
		return -1;

	/* Add the input edge to edge map */
	edge_map[str]=num_nets;
	
	/* Create a new struct net node for this edge */
	struct net temp;
	temp.id = num_nets;
	temp.weight = 0;
	temp.type = INPUT_NET;
	temp.gates = list<u32>();
	net_list.push_back(temp);
	
	/* Add an input gate in the gate_list with an entry in the nets for above input net */
	struct gate gtemp;
	gtemp.id = num_gates;
	gtemp.type = get_gate_type("INPUT");
	gtemp.fan_out = 0;
	gtemp.fan_in = 0;
	gtemp.nets.push_back(temp.id);
	gate_list.push_back(gtemp);
	
	/* Update the bookkeeping data structures */
	num_nets++;
	num_gates++;
	gate_type[gtemp.type]++;
	net_type[temp.type]++;
	return 0;
}

/*
 * update_fan_out - updates the fanout of all the gates once the adjacency lists have been build.
 * Returns 0 on success
 */
int circuit::update_fan_out(){
	
	/* for each gate */
	for(vector<struct gate>::iterator i = gate_list.begin(); i != gate_list.end(); i++){
		/* Iterate over the edges that this gate drives, for nets that don't have any out edges (output gate) this loop wont run */
		for(list<u32>::const_iterator j = i->nets.begin(); j != i->nets.end(); j++){
			/* add the number of gates each edge drives to fan out */
			i->fan_out += net_list[*j].gates.size();
		}
	}
	return 0;
}

/*
 * This function takes the edge numeric identifier and returns the name/label of the edge
 * read from the input file.
 * @id : Numeric id of the edge
 * 
 * Returns name of the edge
 */
string circuit::get_edge_name(int id){
	for(map<string, int>::const_iterator ci=edge_map.begin(); ci!=edge_map.end(); ci++)
		if(ci->second == id) return ci->first;
	return "";
	
}

/* 
 * trims whitespaces from either side of the token.
 * @token : token to be trimmed
 * Returns the pointer to the trimmed token
 * 
 * Note: The idea of this function is taken from stackoverflow.com
 */
char *trim(char *token){
		char *last;
		
		/* Remove leading spaces from token */
		while(isspace(*token)) token++;
		if(*token == 0) return token;	/* Check if its all spaces */
			
		/* Remove trailing spaces from token */
		last = token + strlen(token) - 1;				/* Got to last non null character of string */
		while(last > token && isspace(*last)) last--;		/* Check all the characters that are space by going back up */
		*(last+1) = '\0';									/* Null terminate the token */
		return token;
}

/*
 * get_gate_type - For a particular gate string like 'and' 'nand' etc, this function returns
 * the corresponding integer type identifier
 * @str : gate type string like 'and'
 * 
 * Returns unsigned integer type identifier corresponding the to the gate name
 */
u32 circuit::get_gate_type(string str){
	u32 size = (u32)gate_t.size();
	if(gate_t.find(str) == gate_t.end())
		gate_t[str]=size;
	return gate_t[str];
}

/*
 * This function returns the string name for given gate type identifier value
 * @id : unsigned integer type identifier or gate type
 * 
 * Returns the string representation of gates type e.g. 'or' 'nand' etc
 */
string circuit::get_gate_name(u32 id){
	for(map<string, u32>::const_iterator ci=gate_t.begin(); ci != gate_t.end(); ci++)
		if(ci->second == id) return ci->first;
	return "";
}
