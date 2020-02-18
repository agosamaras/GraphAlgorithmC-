#include <vector> //c++ library for vectors
#include <iostream> //standard input/output library of c++
#include <ctime> //c++ library for timing
#include <algorithm> //c++ library with functions on range of elements
#include <unordered_map> //c++ library for mapping elements
using namespace std; //standard namespace
class node; //forward declaration
class server; //forward declaration
class data1; //forward declaration
void woke(server* s,node* n); //forward declaration
typedef enum importance{HU, CU, HQ, CQ, RMA} importance; //used for cacheing
ostream& operator<<(ostream& out, importance p); //forward declaration
typedef vector<node*> vn; //instead of writing "vector< vector<node> >" all the time, i'll just type "vn"
typedef vector<int> vi; //instead of writing "vector<int>" all the time, i'll just type "vi"
typedef vector<data1*> vd; //instead of writing "vector<data1>" all the time, i'll just type "vd"
const int INFINITE=999; //practically this means that there is no direct link

class data1 //class used for data1
{
private:
	int name; //name of the node
	time_t lastUpdate;
	time_t lastQuery;
public:
	friend void search4data1(node* , data1* ); //friendly access declaration
	friend class node; //friendly access declaration
	data1(int x):name(x){}; //constructor for data1
	~data1(){} //destructor for data1
	int getName()
	{
		return name;
	}
	void setupdate(time_t u) //this function adds to update
	{
		lastUpdate = u;
	}
	int getLastUpdate() //this function returns the timestamp of last update
	{
		return lastUpdate;
	}
	void setquery(time_t q) //this function adds to update
	{
		lastQuery = q;
	}
	int getLastQuery() //this function returns the timestamp of last update
	{
		return lastQuery;
	}
};

vd generatedata1() //this function generates random data1 packets
{
	cout<<"State: Creating random data1 packets\t";
	vd serverdata1;
	for(int i=0; i<1000; i++) // data1 packets number (1000)
		serverdata1.push_back(new data1(i));
	cout<<endl;
	return serverdata1;
}

class node //this is the data1 type that'll be used as the clients
{
private:
	int id; //the client id
	vd memory; //the data1 that this node contatins
	vn neighbors;
	time_t ChangedStatusTime; //used to store the time of change in status
	bool active; //used to show whether the node is active or not
	server* origin;
public:
	vector<int> uplist;
	friend void search4data1(node* , data1* ); //friendly access declaration
	friend void neighborhood(server& );
	friend void updatedata1(server& ); //friendly access declaration
	friend void askdata1(server& ); //friendly access declaration
	friend class server; //friendly access declaration
	node(int x):id(x),active(1){}; //constructor for node & by default active
	~node(){}; //destructor for node
	void insertdata1(data1& i) //inserts data1 in a node
	{
		memory.push_back(&i);
	}
	void deletedata1(data1& i) //deletes data1 from a node
	{
		for(vd::iterator p=memory.begin(); p!=memory.end(); p++)
		{
			if((*p)->name == i.name)
				memory.erase(p);
		}
	}
	void setOrigin(server* ser)
	{
		origin=ser;
	}
	void printNodeState() //prints node id and the data1 it contains
	{
		cout<<"Node no: "<<this->id<<endl;
		for(vd::iterator p=memory.begin(); p!=memory.end(); p++)
			cout<<"data1 id: "<<(*p)->name<<"\t";
		cout<<endl;
	}
	int getID() //returns the id of the node
	{
		return id;
	}
	time_t getChangedStatusTime()
	{
		return ChangedStatusTime;
	}
	void checkdata1() //checks the validity of the data1
	{
		if(uplist.size()!=0)
		{
			cout<<"Node "<<id<<" updated the following data1:\t";
			for(int i=0; i<memory.size(); i++)
			{
				for(int j=0; j<uplist.size(); j++)
				{
					if(uplist[j]==memory[i]->getName())
						cout<<"data1 "<<uplist[j]<<"\t";
				}
			}
			cout<<endl<<endl;
		}
	}
	void showNeighborhood() //prints the node's neighbors
	{
		cout<<"Communicating nodes with node "<<this->id<<":"<<endl;
		for(vn::iterator p=neighbors.begin(); p!=neighbors.end(); p++)
			cout<<"node "<<(*p)->id<<endl;
	}
	void changeStatus()
	{
		active = !active;
		if(active)
		{
			woke(origin,this);
			checkdata1();
			uplist.clear();
		
			for(auto d=memory.begin(); d!=memory.end(); d++)
			{
				if(ChangedStatusTime<(*d)->getLastQuery())
				{
					search4data1(this, *d);
				}
			}
		}
		time(&ChangedStatusTime); //////////////search4data1 called before that!!!!!!!!!!///////////////////////////
	}
};

class server //this data1 type will be used as our server
{
private:
	vn graph;
	vector< vi > weights; //weights from node to node
	vd serverdata1;
	unordered_map<int, int> states;
public:
	int dimension;
	int infosize;
	friend void simulation(server& );
	friend void neighborhood(server& );
	friend void data1Validation(server& ); //friendly access declaration
	friend void updatedata1(server& ); //friendly access declaration
	friend void askdata1(server& ); //friendly access declaration
	friend void allocatedata1(server& exp); //friendly access declaration
	server() //constructor
	{
		getInfo(); //get the needed data1 to set the experiment
		for(int i=0; i<dimension; i++)
			graph.push_back(new node(i));
		for(auto n=graph.begin(); n!=graph.end(); n++)
			(*n)->setOrigin(this);
		for(auto n=graph.begin(); n!=graph.end(); n++)
		{
			time_t t;
			time(&t);
			states.insert(make_pair<int, int> ((*n)->getID(), static_cast<int>(time(&t))));
		}
		serverdata1 = generatedata1();
		infosize=serverdata1.size();
	}
	~server(){} //destructor
	inline void getInfo() //this function acquires the number of clients
	{
		cout<<"Please select the number of nodes you want to be connected to this server:\t";
		cin>>dimension;
		cout<<endl;
	}
	inline void setGraph()  //this function creates random weights between nodes
	{
		cout<<"State: Generating a random connenction graph";
		for(int i=0;i<=dimension;i++)
		{
			vi row; //a single dimension matrix, only to be used for the initialization of the graph
			for(int j=0;j<=dimension;j++)
			{
				row.push_back(NULL);  //initialization of each row for graph
			}
			weights.push_back(row);  //and then after it is initialized each row is added to graph
		}

		for(int i=0; i<=graph.size(); i++)
		{
			if(i!=0)
				weights[i][0]=graph[i-1]->id;
		}
		
		for(int i=1;i<=dimension;i++)
		{
			for(int j=1;j<=dimension;j++)
			{
				if((rand()%100+1)<46 && i!=j)//produce a random number from 1 to 100 and compare it to desired graph density
				{								  //(which is set to 45% randomly) the i!=j factor means there are no "self-paths" for the nodes
					unsigned weight = (rand()%10)+1;//creates a random number from 1 to 10
					weights[i][j]=weight;  //adjusts this number to an edge
				}
				else
				{
					weights[i][j]=INFINITE; //this practically means that there are no "self-paths"
				}
			}
		}
		//make sure it is an undirecetd graph
		for(int i=0;i<weights.size();i++)
		{
			for(int j=0;j<weights.size();j++)
			{
				if(weights[j][i]!=INFINITE)
				{
					weights[i][j]=weights[j][i];
				}
				else
				{
					weights[j][i]=weights[i][j];
				}
			}
		}
		cout<<endl;
	}
	void printGraph() //this function shows the graph on screen
	{
		cout<<"The connections between the nodes have the following values"<<endl<<endl;
		for(int i=0;i<weights.size();i++)
		{
			for (int j=0;j<weights.size();j++)
			{
				if(i==0 && j ==0)
					cout<<"\ "<<"\t";
				else if(j!=weights.size()-1)
				cout<<weights[i][j]<<"\t";  //print every line of the matrix horizontally
				else if(j==weights.size()-1)
				cout<<weights[i][j]<<endl<<endl; //change the line at the end of each row
			}
		}
		cout<<endl;
	}
	void woke(node* n)
	{
		for(auto d=serverdata1.begin(); d!=serverdata1.end(); d++)
		{
			if((*d)->getLastUpdate() > n->getChangedStatusTime())
				n->uplist.push_back((*d)->getName());
		}
	}
};

void neighborhood(server& exp)
{
	for(vn::iterator n=exp.graph.begin(); n!=exp.graph.end(); n++)
	{
		for(int i=1; i<exp.weights.size(); i++)
		{
			if(exp.weights[i][0] == (*n)->id)
			{
				int counter = 0; //for simplicity we presume each node has 3 neighbors
				for(int j=1; j<exp.weights.size(); j++)
				{
					if(exp.weights[i][j]<=10 && counter<3)
					{
						for(vn::iterator p=exp.graph.begin(); p!=exp.graph.end(); p++)
						{
							if((*p)->id == exp.weights[0][j] && counter<3)
							{
								(*n)->neighbors.push_back(*(&(*p)));
								counter++;
							}
						}
					}
				}
			}
		}
	}
}

void allocatedata1(server& exp) //data1 allocation in nodes
{
	cout<<"State: Randomizing nodes' cache";
	for(vd::iterator p=exp.serverdata1.begin(); p!=exp.serverdata1.end(); p++)
	{
		int selected = rand()%exp.graph.size();
		exp.graph[selected]->insertdata1(*(*p));
	}
	cout<<endl;
}

void simulation(server& exp) //simulation of status changing
{
	int selected = rand()%exp.graph.size();
	exp.graph[selected]->changeStatus();
}

void updatedata1(server& exp) //random update generator
{
	time_t t;
	int selected2 = rand()%exp.serverdata1.size();
	time(&t);
	exp.serverdata1[selected2]->setupdate(t);
}

void askdata1(server& exp)
{
	time_t t;
	int selected3 = rand()%exp.graph.size();
	int selected4 = (rand() + rand())%exp.graph[selected3]->memory.size();
	time(&t);
	exp.graph[selected3]->memory[selected4]->setquery(t);
}

void woke(server* s,node* n)
{
	s->woke(n);
}

void search4data1(node* client, data1* data1) //search neighbors for the required data1
{
	bool found=0;
	for(auto n=client->neighbors.begin(); n!=client->neighbors.end(); n++)
	{
		if(found)
			break;			
		for(auto d=(*n)->memory.begin(); d!=(*n)->memory.end(); d++)
		{ //3.3.2.2 RV, BCKRCM
			if(data1->getName() == (*d)->getName() && (*d)->getLastUpdate() > client->getChangedStatusTime()) 
			{ //checks if the data1 is valid nd inside the reconnected node's memory
				cout<<"After passive disconnection found a valid instance of data1 "<<
					data1->getName()<<" in neighbor's node "<<(*n)->getID()<<" cache!"<<endl;
				found++;
				break;
			}
			else //3.3.2.2 BCKNRCM
				cout<<"Could not sertify if current instance of requested data1 "<<data1->getName()<<" is valid"<<endl;
		}
	}
}

int main()
{
	srand((unsigned)time(NULL));
	server example;
	allocatedata1(example);
	example.setGraph();
	neighborhood(example);

	cout<<endl<<"Beginning experiment, in order to stop it press Ctrl + C"<<endl<<endl;
	while(1)
	{
		updatedata1(example);
		simulation(example);
		askdata1(example);
	}
}