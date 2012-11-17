=======================================================================================================
Phase 3:

The programming assignment is complete. The program is passing the three provided benchmarks. The histogram is included both as a pdf file 
and as png file.
The source files for generating the histogram are also included.


The program can be built by typing 'make' at the programs root directory. It generates an executable named 'parser'
The program can be run by typing './parser input_file_name' at the command line.
After the program finishes execution it generates a .out file which contains the result in the desired form.
'make clean' can be used to cleanup the generated files.

The program is profiled for memory leaks using valgrind tool. The performance can be checked by building the program with debug
information as follows:
make debug=1
and then running
make memprof in_file=<input_file_name>

This will generate memory information and statistics etc.

======================================================================================================
Description of Source code

The data structures are build using the input file that describes the ciruit. After the data structures are built, 
the statistics are generated.

The complete circuit is captured in a class named as 'circuit'. To handle the hyperedges, it contains two adjacency lists:
1) One is the vector array of gates known as gate_list, where each gate has a linked list for the output edges from that gate. This linked list contains the indices
   of edge object in the vector array of edges (net_list). 
2) Second is the vector array of edges (or nets) known as net_list, where each net has the linked list of the gates driven by this edge. This linked is 
   are only the indices of gate objects in the gate_list data structure.

In addition, the circuit class also contains maps for gates and nets. The edge_map maps the edge names with the corresponding
indices in the net_list vector, whereas the gate_t map maps the string representation of gate type with the corresponding integer type.
For e.g. in a particular case 'and' may be given type id 0, 'not' as 1 etc.

There are some data structures which contain some information like the ones asked for the final statistics generation.
This data structures are defined in 'netlist.h' file

The netlist.cpp file contains the function definition for the functions used in this project. The details of each function
is provided before the function definition in the netlist.cpp file. Comments are also added within the function body for the
steps performed in the function.


=====================================================================================================
Some details of the project

The gate functionality and information is kept is 'struct gate' data structure
The net/edge functionality and information is captured in 'struct net' data structure

Two adjacency lists are build as described above. One is the vector of 'struct edge' and one is the 
vector of 'struct net'

Complete information about the netlist and all the data structures are kept inside class 'circuit' and 
all the operations are performed using the member functions of this class.

The main function calls the 'parse_input' function in the netlist.cpp file which reads the circuit description 
from the file and performs the following steps:
 1) If an input gate is found then corresponding input gate and input edge is build and added to the corresponding
    data structures with proper connections.
 2) If an output gate is found then corresponding output gate and edge entry is made and the data structures
    are updated.
 3) If a gate description is found then again the corresponding edge and gate entries are made and the data
    structures are updated. All the gates are unique, but the edges may be repeated, so whenever an edge 
    is encountered, it is first checked whether a corresponding entry exists, if not then new space is allocated.
This function returns when the complete graph representation is done using the two adjacency lists.

Once the graph data structures are generated the main calls the 'update_fan_out' function which updates the fan out
value for each gate. 

Maximum fanout is calculated and number of gates with similar fanouts are found, corresponding statistics are 
written in the output file.

The number and indices of input gates are logged in the output file.
The number and indices of the output gates are logged in the output file.

'update_same_faninout' function is called which calculates  the number of gates of type T that drive at least one fanout 
of the same type, and then the number of gates of type T that has at least one fanin of the same type.

The number of logic gates of type T, followed by the number of gates of type T and the above information is again
logged in the output file.

Allocated memories are freed up and the main function exits.
