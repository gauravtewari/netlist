/*
 * This file contains the main function.
 * This function calls the function for parsing input file and building graph data structures.
 * Then it calls some functions for generating statistics
 * The output file is stored in .out file
 * 
 */

#include "netlist.h"
#include <iostream>
#include <vector>
#include <list>
#include <cstring>
#include <fstream>

using namespace std;


int main(int argc, char **argv){
	class circuit c;

	if(argc != 2){
		cout<<"Error: No input file specified.\n";
		cout<<"Usage: ./parser <input_file_name>\n";
		return 1;
	}

	/* Parse the input file and generate the graph data structures */
	if(c.parse_input(argv[1]) != 0){
		cerr<<"Error while Parsing input \n";
		return 1;
	}

	/* Figuring the name of the outfile */
	char *ofile = strtok(argv[1],".");
	char *out_file = new char[strlen(ofile)+5];
	strncpy(out_file, ofile, strlen(ofile));
	strncpy(out_file+strlen(ofile), ".out",4);
	out_file[strlen(ofile)+4]='\0';

	/* Opening the output file for storing result */
	ofstream fout(out_file);
	if(!fout.is_open()){
		cerr<<"Can't open output file for writing\n";
		return -1;
	}
	
	/* Calculate the fan out for all the gates in the netlist */
	c.update_fan_out();
	
	/* Calculating the maximum fanout */
	u32 max_fout=0;
	for(vector<struct gate>::const_iterator i = c.gate_list.begin(); i != c.gate_list.end(); i++)
		if(i->fan_out > max_fout)
			max_fout = i->fan_out;
	fout<<max_fout<<endl;
	
	
	/* find the number of gates with similar fanouts */
	int *fan_count = new int[max_fout+1];
	for(u32 i=0; i<max_fout+1; i++) fan_count[i]=0;
	for(vector<struct gate>::const_iterator i = c.gate_list.begin(); i != c.gate_list.end(); i++)
		fan_count[i->fan_out]++;
	
	/* Printing result for number of gates with similar fanout */
	for(u32 i=0; i<max_fout+1; i++){
		fout<<fan_count[i]<<endl;
	}
	
	/* Generating the index of adjacency list entries for INPUT gates */
	u32 gtype = c.get_gate_type("INPUT");
	fout<<c.gate_type[gtype]<<" ";
	for(vector<struct gate>::const_iterator i = c.gate_list.begin(); i != c.gate_list.end(); i++)
		if(i->type==gtype) {
			fout<<i->id<<" ";
		}
	fout<<endl;

	/* Generating the index of adjacency list entries for OUTPUT gates */
	gtype=c.get_gate_type("OUTPUT");
	fout<<c.gate_type[gtype]<<" ";
	for(vector<struct gate>::const_iterator i = c.gate_list.begin(); i != c.gate_list.end(); i++)
		if(i->type==gtype){
			fout<<i->id<<" ";
		}
	fout<<endl;
	
	/* Calculate the number of gate types that drive the same type gate, and the fanin counterpart */
	c.update_same_faninout();
	for (u32 i=0; i<c.gate_t.size(); i++){
		if((c.gate_type[i]!=0) && (i != c.get_gate_type("INPUT")) && (i != c.get_gate_type("OUTPUT"))){
			fout<<c.get_gate_name(i)<<" "<<c.gate_type[i]<<" "<<c.same_fanout[i]<<" "<<c.same_fanin[i]<<endl;

		}
	}
	
	/* Clean up */
	fout.close();
	delete []out_file;
	delete []fan_count;
	cout<<"Statistics Generated, check output file\n";
	cout<<"Output file generated: "<<ofile<<".out\n";
	return 0;
}

