//I'm also attaching the data.txt file that was used for input (just for checking it out)
#include <vector> //i'm using the c++ library for vectors
#include <iostream> //i'm using the standard input/output library of c++
#include <algorithm> //i'm using the c++ library for algorithms, such as sort()
#include <ctime> //i'm using the c++ library for timing
#include <fstream> //i'm using the c++ library for file streaming
#include <numeric> //i'm using the c++ library for arithmetics
using namespace std; //i'm using the standard namespace
typedef vector<int> vi; //instead of writing "vector<int>" all the time, i'll just type "vi"
typedef vector<bool> vb; //likewise instead of "vector<bool>", i'll be typing "vb"
const int INFINITE=999; //practically this means that there is no edge from one node to another

class graph //this is a new type for the requirements of this assignment, with the following specifications
{
private:
	vector< vi > node_list; //a two dimensional vector to be used as our graph represantation
	vi tree; //the minimum spanning tree
	vi data; //a vector used to save the data of the input file
	vb closed; //this is the closed set
	int nodes; //the number of nodes
	int cost; // the total cost of the tree
	int starting_node; //the starting node for the algorithm
public:
	graph() //class constructor
	{
	}
	~graph() //class destructor 
	{
		cout<<"destructor called"<<endl;
	}
	inline void get_info() //this function gets the data that are required for the initialization of the graph
	{
		int i=0;
		ifstream input("data.txt", ios::in); //input the contents of data.txt file
		while(!input.eof()) //loop until there are no more data
		{
			input>>i; //temorarily store each component in i
			data.push_back(i); //then save it in the data vector
		}
		nodes = data.front()+1; //we don't use 0 so the nodes offset is +1
		for(int i=0; i<nodes; i++)
		{
			closed.push_back(NULL); //initialization of closed set
		}
		cout<<"Please the that the tree should start from. Note that the available nodes are from 1 to "
			<<nodes-1<<": "<<endl;
		cin>>starting_node; //get the starting node
		closed[0]=true; //since the nodes begin from 1 the value of closed[0] doesn't matter
		closed[starting_node]=true; //the starting node by definition belongs to the closed set
		cost=0; //initialization of total cost
	}
	inline void set_edges() //create the graph accordingly to data.txt (or equivalently the data vector)
	{
		for(int i=0; i<nodes; i++)
		{
			vi row; //a single dimensional matrix, only to be used for the initialization of the node_list
			for(int j=0; j<nodes; j++)
			{
				if(!i) //enumerate the nodes at the top and the leftmost of the graph
				{
					row.push_back(j);
				}
				else if(!j) //enumerate the nodes at the top and the leftmost of the graph
					{
						row.push_back(i);
					}
					else
					{
						row.push_back(INFINITE); //initialize every edge at the INFINITE value
					}
			}
			node_list.push_back(row); //create the two dimensional vector
		}
		for(int i=1; i<data.size(); i++) //exclude the first value, because it is the number of nodes in the graph
		{
			if(i%3==0) //read the components of data at a step of 3, since we want the three consequent values (i,j,edge)
			{
				node_list[data[i-1]][data[i-2]]=data[i]; //set the edges accordingly
			}
		}
	}
	inline void show_graph() //an auxilliary function used to show the graph
	{
		for(int i=0;i<node_list.size();i++)
		{
			for (int j=0;j<node_list.size();j++)
			{
				if(j!=nodes-1)
				cout<<node_list[i][j]<<"\t";  //print every line of the matrix horizontally
				if(j==nodes-1)
				cout<<node_list[i][j]<<endl<<endl; //change the line at the end of each row
			}
		}
	}
	inline int get_closest_node(int factor) //this function returns the closest neighboring node for every node in the closed set
	{
		int closest_node; //the closest node will be stored here
		vi edges; //a vector conatining the edges for every node used as factor
		int smallest_edge=INFINITE; //initialization of smallest_node
		for(int i=1; i<nodes; i++)
		{
			if(!closed[i]) //do, only if the node hasn't been added in the closed set
			{
				edges.push_back(node_list[i][factor]);
			}
		}
		sort(edges.begin(),edges.end()); //sort the edges in increasing order
		smallest_edge=edges.front(); //this means that the first edge in the sorted edges vector is the smallest as well
		for(int i=1; i<node_list.size();i++) //find the smallest edge in the graph
		{
			if(node_list[i][factor]==smallest_edge) //find the smallest edge in the graph
			{
				closest_node = i; //set the node to which the smallest edge leads to, as the closest node to factor node
			}
		}
		return closest_node; //return the closest node
	}
	inline void prim() //this function actually implements prim's algorithm
	{
		vi neighbor(nodes); //a vector conatining the closest nodes for every node in the closed set
		int closest_node; //this closest_node is totally the node with the smallest edge 
			while(accumulate(closed.begin(),closed.end(),0)!=nodes) //if every node is added to the closed set, then we have found the mst and every value of closed will be 1-true
			{
				for(int i=1; i<nodes; i++)
				{
					if(closed[i]) //check the closest nodes only for the nodes that belong so far to th mst
					{
						neighbor[i]=get_closest_node(i); //store the closest nodes for every node of closed set
					}
				}
				vi edge_to_neighbor(neighbor.size()); //this vector will be used to store the edges to each closest node
				edge_to_neighbor[0]=INFINITE; //since we don't use node 0
				for(int i=1;i<neighbor.size();i++)
				{
					if(neighbor[i]>0) //if there is node stored in neighbor[i]
					{
						edge_to_neighbor[i]=node_list[i][neighbor[i]]; //find the edge in the graph and store in this vector
					}
					else //if there isn't a node stored in neighbor[i]
					{
						edge_to_neighbor[i]=INFINITE; //there isn't an available edge as well
					}
				}
				sort(edge_to_neighbor.begin(),edge_to_neighbor.end()); //sort the edges in increasing order
				for(int i =1; i<edge_to_neighbor.size(); i++)
				{
					for(int j=1; j<edge_to_neighbor.size(); j++)
					{
						if(closed[i] && !closed[j] && edge_to_neighbor.front()==node_list[j][i]) //if node i belongs to the closed set and j doesn't, then 
						{  //the edge i-j is valid edge, and if it is the least costly then
							closest_node=j; //set j as next node of the mst, since it is the closest node
						}
					}
				}
				closed[closest_node]=true; //accordingly set the closest node a part of the closed set
				tree.push_back(closest_node); //store in the tree vector
				cost+=edge_to_neighbor.front(); //and increment the total cost of the mst
			}
		print(); //once the mst is complete print the results
		output(); //once the mst is complete output the results
	}
	inline void print()///try template classes!! print and output
	{
		cout<<"cost: "<<cost<<endl;
		cout<<"tree: "<<starting_node;
		for(vi::iterator p=tree.begin(); p!=tree.end(); p++)
			cout<<" -> "<<*p;
		cout<<endl;
	}
	inline void output()
	{
		ofstream output("assignment results.txt"); //create a file named assignment results.txt to store the results in
		output<<"cost: "<<cost<<endl; //store the total cost of the mst
		output<<"tree: "<<starting_node; //store the mst
		for(vi::iterator p=tree.begin(); p!=tree.end(); p++) //store the mst
			output<<" -> "<<*p; //store the mst
		output<<endl;
	}
};

int main()
{
	graph g;
	g.get_info();
	g.set_edges();
	g.show_graph(); //comment if you don't want the graph to be shown 
	g.prim();
	return 0;
}