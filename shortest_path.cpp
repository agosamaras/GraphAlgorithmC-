#include <iostream> //i'm using the standard input/output library of c++
#include <vector>	//i'm using the c++ library for vectors
#include <ctime>	//i'm using the c++ library that contains code about the timing
using namespace std;//i'm using the namespace std
typedef vector<int> vi;//instead of writing "vector<int>" all the time, i'll just type "vi"
typedef vector<bool> vb;//likewise instead of "vector<bool>", i'll be typing "vb"
const int NODES=51;//the number of nodes is described by the assignment and it must be 50, 
					//so I created a const int out of it. It is 51 because i want my nodes to start from 1
const int INFINITE=999;//practically this means that there is no edge from one node to another

class graph //this is a new type for the requirements of this homework, with the following specifications
{
private:  //the private components are "hidden" from the user, so he cannot use them anyhow
	vector< vi > node_list; //this is a two dimensional matrix that will store all the nodes and the available edges in the graph
	vi recent_node;  //a vector that updates the nodes of the shortest path, until it is completed
	vb closed;  //this will keep track of all the nodes that have been added to the shortest path
	vb k_mem;  //this contains all the available values for k
	vi closest_node;  //this will contain the smallest available edge for every node 
	int distance;  //an int to keep track of the total cost of the path
	int origin_node;  //the node from wich we want to begin
	int destination_node;  //and the node to which we want to "arrive"
	int density;  //the density of the graph
public:  //the private components are "open" to the user
	inline graph() //the constructor function for the class graph
	{
		for(int i=0;i<NODES;i++)
		{
			vi row; //a single dimension matrix, only to be used for the initialization of the node_list
			for(int j=0;j<NODES;j++)
			{
				row.push_back(NULL);  //initialization of each row for node_list
			}
			node_list.push_back(row);  //and then after it is initialized each row is added to node_list
			k_mem.push_back(false);  //initialization of k_mem
			closest_node.push_back(INFINITE);  //initialization of closest_node
			closed.push_back(false);  //initialization of closed
			distance=0;  //initialization of distance
		}
		for(int i=0; i<NODES; i++)
		{
			node_list[0][i]=i;  //on the first collumn of the matrix there should be the nodes
		}
		for(int i=0;i<NODES;i++)
		{
			node_list[i][0]=i;  //likewise on the first line there should be the nodes
		}
	}
	inline void get_info()  //this function gets the vital parameters of our algorithm and stores them in the according graph components
	{ //every function used in this programme is inline, in order to let the compiler avoid "extra" work
		cout<<"Please choose your desired density, but type it in decimals. For instatnce, if"
			<<" you want a graph with density of 0,1 or 10%, type 10:\t";
		cin>>density;  //user-defined density for the graph
		while(density<0 || density>100)  //it tests the validity of the inserted value
		{
			cout<<"Please choose a density between 0% and 100%:\t";
			cin>>density;
		}
		cout<<"You chose "<<density<<endl;
		cout<<"Now please choose the origin node and the destination node.Note that the available nodes for selection are from 1 to 50.\nOrigin:"<<endl;
		cin>>origin_node;  //it gets the first node
		while(origin_node<0 || origin_node>NODES-1)  //it tests the validity of the inserted value
		{
			cout<<"The available nodes for span from node 1 to node "<<NODES-1<<". Please enter a valid value:"<<endl;
			cin>>origin_node;
		}
		cout<<"Destination:"<<endl;
		cin>>destination_node;  //it gets the last node
		while(destination_node<0 || destination_node>NODES-1)  //it tests the validity of the inserted value
		{
			cout<<"The available nodes for span from node 1 to node "<<NODES-1<<". Please enter a valid value:"<<endl;
			cin>>destination_node;
		}
		cout<<"The path you chose is "<<origin_node<<" -> "<<destination_node<<endl;
		recent_node.push_back(origin_node);//set the origin node as the first node of the shortest path
		closed[origin_node]=true;//since we begin from this node it is logical that we should check it right from the start
	}
	inline void set_edges()  //this function actually implements the graph, by creating its edges
	{
		for(int i=1;i<NODES;i++)
		{
			for(int j=1;j<NODES;j++)
			{
				if((rand()%100+1)<density && i!=j)//produce a random number from 1 to 100 and compare it to desired graph density
				{								  //the i!=j factor means there are no "self-paths" for the nodes
					unsigned weight = (rand()%10)+1;//creates a random number from 1 to 10
					node_list[i][j]=weight;  //adjusts this number to an edge
				}
				else
				{
					node_list[i][j]=INFINITE; //this practically means that there are no "self-paths"
				}
			}
		}
		for(int i=1;i<NODES;i++)//in order to make sure that the graph is conncetd set a random edge to each node to its next
		{
			if(i<NODES-1)
			{
				node_list[i+1][i]=10;
			}
			else if(i==NODES-1)
			{
				node_list[1][i]=10;
			}
		}
		/////////////////////////////////there is always at least one random edge in every node////////////////////////
		//int sum;//there is always at least one edge in every node
		//vi m;
		//int k;
		//int l;
		//for(int i=1;i<NODES;i++)
		//{
		//	m.clear();//reset m for every node!
		//	l=0;//reset l for every node!
		//	//cout<<"sum: "<<sum<<endl;
		//	sum=INFINITE*(NODES-1);
		//	for(int j=1;j<NODES;j++)
		//	{
		//		if(node_list[j][i]<INFINITE)
		//		{
		//			l++;
		//		}
		//		sum-=node_list[j][i];
		//		if(!k_mem[j] && node_list[j][i]==INFINITE && i!=j)
		//		{
		//			m.push_back(j);
		//		}
		//	}
		//		if(sum==0 || l<2)
		//		{
		//				k=rand()%(m.size());
		//				k=m[k];
		//				if(k==i)
		//				{
		//					if(k<NODES-1)
		//					{
		//						node_list[k+1][i]=(rand()%10)+1;
		//						node_list[i][k+1]=(rand()%10)+1;//this one here!!!
		//						k_mem[k+1]=true;
		//						cout<<"k= "<<k+1<<endl;
		//					}
		//					else
		//					{
		//						node_list[k-1][i]=(rand()%10)+1;
		//						node_list[i][k-1]=(rand()%10)+1;//this one here!!!
		//						k_mem[k-1]=true;
		//						cout<<"k= "<<k-1<<endl;
		//					}
		//				}
		//				else
		//				{
		//					node_list[k][i]=(rand()%10)+1;
		//					node_list[i][k]=(rand()%10)+1;//this one here!!!
		//					k_mem[k]=true;
		//					cout<<"k= "<<k<<endl;
		//				}
		//		}
		//}
		//////////////////////////////////////there is always at least one random edge in every node////////////////////////
		for(int i=1;i<NODES;i++)
		{
			for(int j=1;j<NODES;j++)
			{
				if(node_list[j][i]!=INFINITE)
				{
					node_list[i][j]=node_list[j][i]; //this makes it an undirected graph
				}
				else
				{
					node_list[j][i]=node_list[i][j]; //this makes it an undirected graph
				}
			}
		}
	}
	inline void show_graph()  //this function prints the graph to the screen, it is auxilliary, not vital for the algorithm
	{
		for(int i=0;i<node_list.size();i++)
		{
			for (int j=0;j<node_list.size();j++)
			{
				if(j!=NODES-1)
				cout<<node_list[i][j]<<"\t";  //print every line of the matrix horizontally
				if(j==NODES-1)
				cout<<node_list[i][j]<<endl<<endl; //change the line at the end of each row
			}
		}
	}
	inline int get_closest_node(int factor)  //it produces the smallest weight for the node, defined by factor
	{
		vi temp; //a temporary matrix that will store the costs of the edges
		for(int i=0;i<NODES;i++)
		{
			temp.push_back(INFINITE);  //initialization of temp
		}
		int smallest_weight=INFINITE; //initialization of smallest weight
		for(int i=1;i<NODES;i++)
		{
			if(!closed[i])  //so that it checks only those not in closed group
			{
				temp[i]=node_list[i][factor]; //store the values of the edges in temp
			}
			else if(closed[i])
			{
				temp[i]=INFINITE; //so that we won't recompute the path for already closed nodes
			}
		}
		/*for(int i=1;i<NODES;i++)
		{
			for(int j=1;j<NODES;j++)
			{
				if(temp[i]==temp[j] && temp[i]<INFINITE && i!=j)  //if the smallest edge corresponds to many adjacent nodes
				{                                                  
					if(i!=destination_node)  //and if none of them is the destination node
					{
						int c=(rand()%(NODES-1))+1;  //create a random number from 1 to NODES-1
						while(c!=i || c!=j)
						{
							c=(rand()%(NODES-1))+1;  //until there is match recompute the number
						}
						temp[c]=INFINITE;  //and choose randomly the one the two same edges
					}
					else if(i==destination_node)
					{
						temp[j]=INFINITE;  //if one of them is the destination choose this, by setting the other as INFINITE
					}
				}
			}
		}*/
		for(int i=1;i<NODES;i++)
		{
			if(smallest_weight>temp[i]) //calculate the edge with the smallest distance
			{
				smallest_weight=temp[i]; //store it in "smallest weight"
				closest_node[i]=smallest_weight; //set the value of closest_node[i], it will be used later on to identify the node, by means of i
			}
		}
		return smallest_weight; //return the value of smallest weight
	}
	inline void dijkstra()//loops until the destination node is in the closed set, then it calls print()
	{
		cout<<"processing the dijkstra's algorithm..."<<endl;
		int next_node; //this stores the value of the next-to-be node of our graph
		int next_edge; //this stores the cost of the edge to the next-to-be node of our graph
		vi edges; //a vector to temporarily store the costs of the available edges
		
		while(!closed[destination_node])
		{
			edges.clear();///////////////////
			for(int i=0;i<NODES;i++)
			{
				edges.push_back(INFINITE);//initialization of edges//////////////////
			}
			for(int i=1;i<NODES;i++)
			{
				if(closed[i]) //check the edges for nodes of the closed set
				{
					edges[i]=get_closest_node(i); //store the shortest edge for each node
				}
			}
			next_edge=INFINITE; //initialization of the next edge
			for(int i=1;i<NODES;i++)
			{
				if(edges[i]<next_edge) 
				{
					next_edge=edges[i]; //find the overall shortest edge
				}
			}
			distance+=next_edge; //increment the total cost so far
			for(int i=1;i<closest_node.size();i++)
			{
				if(next_edge==closest_node[i] && i!=recent_node.back()) //identify the corresponding node to the
				{														//shortest edge and loop avoidance
					next_node=i;
					closed[next_node]=true;//set the next node of the path
					cout<<"next node: "<<next_node<<endl;
				}
			}
			for(int i=1;i<closest_node.size();i++)
			{
				closest_node[i]=INFINITE;//reinitialize its values every time
			}
			recent_node.push_back(next_node);//save the next node of the shortest path
		}
		print_path(); //after the loop is ended which practically means that the destination node is closed
	}				  //the function that prints the results is called
	inline void print_path() //this function prints the results
	{
		cout<<"The shortest path between the nodes you have chosen is: "<<endl;
		for(int i=0;i<recent_node.size();i++)
		{
			if(i<((recent_node.size())-1))
			{
				cout<<recent_node[i]<<"->";
			}
			else
			{
				cout<<recent_node[i]<<endl;
			}
		}
		cout<<"And the total cost of this path is equal to: "<<distance<<endl;
	}
	inline ~graph() //the destructor for graph, which clears every vector used by graph
	{
		cout<<"erasing data of the graph"<<endl;
		node_list.clear();
		recent_node.clear();
		closed.clear();
		k_mem.clear();
		closest_node.clear();
	}
};
int main()
{
	srand((unsigned)time(NULL));
	graph g;
	g.get_info();
	g.set_edges();
	g.show_graph(); //un-comment to show the graph matrix
	g.dijkstra();
	return 0;
}