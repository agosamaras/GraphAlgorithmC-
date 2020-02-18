#include <vector> //c++ library for vectors
#include <iostream> //standard input/output library of c++
#include <ctime> //c++ library for timing
#include <algorithm> //c++ library with functions on range of elements
using namespace std; //standard namespace
class node; //forward declaration
class server; //forward declaration
class data1; //forward declaration
typedef enum importance{HU, CU, HQ, CQ, RMA} importance; //used for cacheing
ostream& operator<<(ostream& out, importance p); //forward declaration
typedef vector<node*> vn; //instead of writing "vector< vector<node> >" all the time, i'll just type "vn"
typedef vector<int> vi; //instead of writing "vector<int>" all the time, i'll just type "vi"
typedef vector<data1*> vd; //instead of writing "vector<data1>" all the time, i'll just type "vd"

class data1 //class used for data1
{
private:
	importance imp; //hot/cold update, hot/cold query, remainder
	int queryCounter; //counter for how many times it has been required
	int lastQuery; //when was the last query
	int updateCounter; //counter for how many times it has been updated
	int lastUpdate; //when was the last update
public:
	int name; //name of the node
	friend class node; //friendly access declaration
	friend struct less_than_query; //friendly access declaration
	friend struct less_than_update; //friendly access declaration
	data1(int x, importance imp):name(x),imp(imp),updateCounter(0),queryCounter(0){}; //constructor for data1
	~data1(){} //destructor for data1
	void setImportance(importance i) //this function sets this data1's category
	{
		imp = i;
	}
	importance getImportance() //this function returns this data1's category
	{
		return imp;
	}
	void setquery(int q) //this function adds to query
	{
		lastQuery = q;
		queryCounter++;
	}
	int getLastQuery() //this function returns the timestamp of last query
	{
		return lastQuery;
	}
	int getQueryCounter() //this function returns the queryCounter
	{
		return queryCounter;
	}
	void setupdate(int u) //this function adds to update
	{
		lastUpdate = u;
		updateCounter++;
	}
	int getLastUpdate() //this function returns the timestamp of last update
	{
		return lastUpdate;
	}
	int getUpdateCounter() //this function returns the updateCounter
	{
		return updateCounter;
	}
	void reset() //this function resets the counters
	{
		updateCounter = 0;
		queryCounter = 0;
	}
};

struct less_than_query //comparison by No. of queries
{
	inline bool operator() (const data1& struct1, const data1& struct2)
    {
		return (struct1.queryCounter < struct2.queryCounter);
    }
};

struct less_than_update //comparison by No. of updates
{
	inline bool operator() (const data1& struct1, const data1& struct2)
    {
		return (struct1.updateCounter < struct2.updateCounter);
    }
};

vd generatedata1() //this function generates random data1 packets
{
	cout<<"State: Creating random data1 packets\t";
	vd serverdata1;
	for(int i=0; i<10000; i++) // data1 packets number (1000) depends on machine
		serverdata1.push_back(new data1(i,static_cast<importance>(rand()%5)));
	cout<<endl;
	return serverdata1;
}

class node //this is the data1 type that'll be used as the clients
{
private:
	int id; //the client id
	vd memory; //the data1 that this node contatins
	int report[4][5]; //space for storing IR report
	int irtime; //space for storing IR report timestamp
public:
	friend void updatedata1(server& ); //friendly access declaration
	friend void askdata1(server& ); //friendly access declaration
	friend ostream& operator<<(ostream& , node& ); //friendly access declaration
	friend class server; //friendly access declaration
	node(int x):id(x){}; //constructor for node
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
	void printNodeState() //prints node id and the data1 it contains
	{
		cout<<"Node no: "<<this->id<<endl;
		for(vd::iterator p=memory.begin(); p!=memory.end(); p++)
			cout<<"data1 id: "<<(*p)->name<<"\t"
			"data1 importance: "<<(*p)->getImportance()<<endl;
	}
	int getID() //returns the id of the node
	{
		return id;
	}
	void checkdata1() //checks the validity of the data1
	{
		for (int i=0; i<4; i++)
		{
			for(int j=0; j<5; j++)
			{
				for(vd::iterator m=memory.begin(); m!=memory.end(); m++)
				{
					if((*m)->name == report[i][j] && irtime > (*m)->getLastUpdate())
						(*m)->setupdate(irtime);
				}
			}
		}
	}
	void setIrTime(int t) //set the timestamp of the report
	{
		irtime = t;
	}
};

class server //this data1 type will be used as our server
{
private:
	vn graph;
	vd serverdata1;
	vector<data1> hq; //hot query
	vector<data1> cq; //cold query             
	vector<data1> hu; //hot update
	vector<data1> cu; //cold update
	time_t interval; //the in-between-intervals time
public:
    int report[4][5]; //report sent to clients
	int dimension;
	int infosize;
	friend void data1Validation(server& ); //friendly access declaration
	friend void updatedata1(server& ); //friendly access declaration
	friend void askdata1(server& ); //friendly access declaration
	friend void allocatedata1(server& exp); //friendly access declaration
	server() //constructor
	{
		getInfo(); //get the needed data1 to set the experiment
		for(int i=0; i<dimension; i++)
			graph.push_back(new node(i));
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
	
	void data1Classification() //sets data1 as hot/cold update, hot/cold query, or remainder
	{
		int ref = 0.05 * serverdata1.size(); //5% of the data1
		vector<data1> temp;
		for(vd::iterator p = serverdata1.begin(); p != serverdata1.end(); p++)
			temp.push_back(*(*p));

		sort(temp.begin(), temp.end(), less_than_query());
		for(int i = 0; i <= ref; i++)
			hq.push_back(temp[i]);
		for(int i = temp.size()-ref; i < temp.size(); i++)
			cq.push_back(temp[i]);

		sort(temp.begin(), temp.end(), less_than_update());
		for(int i = 0; i < ref; i++)
			hu.push_back(temp[i]);
		for(int i = temp.size()-ref; i < temp.size(); i++)
			cu.push_back(temp[i]);
	}
	void printGraph() //this function shows the graph on screen
	{
		cout<<"The connected nodes to this server are the following: "<<endl<<endl;
		for (vn::iterator p = graph.begin(); p != graph.end(); p++)
		{
			cout<<"Node "<<(*p)->getID()<<endl;
		}
		cout<<endl;
	}
	void setReport() //creates the report
	{
		for(int i=0; i<hu.size(); i++)
		{
			report[0][i] = hq[i].name;
			report[1][i] = hu[i].name;
			report[2][i] = cq[i].name;
			report[3][i] = cu[i].name;
		}
	}
	double getInterval() //returns the value of interval
	{
		return static_cast<double>(interval);
	}
	void setInterval(double t) //sets the interval
	{
		interval = static_cast<time_t> (t);
	}
	void sendReport() //sends the report to all the clients
	{
		time_t timer;
		for(int i = 0; i < 4; i++)
		{
			for(int j = 0; j < 5; j++)	
			{
				for(vn::iterator p=graph.begin(); p!=graph.end(); p++)
					(*p)->report[0][j] = report[0][j];
			}
		}
		for(vn::iterator p=graph.begin(); p!=graph.end(); p++)
			(*p)->setIrTime(static_cast<int>(time(&timer)));
	}
	void calcInterval() //interval adaptation
	{
		double NUD = 0; //counter for Number of Updated data1
		for(vd::iterator d=serverdata1.begin(); d!=serverdata1.end(); d++)
		{
			if((*d)->getUpdateCounter()>0)
				NUD++;
		}
		if(NUD > (2.0/3.0 * serverdata1.size())) //NUD > Th
		{
			cout<<"ir: "<<this->getInterval()<<endl;
			cout<<"NUD: "<<NUD<<endl<<"Th: "<<(2.0/3.0 * serverdata1.size())<<
				endl<<"Tl: "<<(1.0/3.0 * serverdata1.size())<<endl;
			interval = interval/2;
			cout<<"new ir: "<<this->getInterval()<<endl;
			cout<<"Too big an interval! Dividing by 2"<<endl<<endl;
		}
		else if(NUD < 1.0/3.0 * serverdata1.size()) //NUD < Tl
		{
			cout<<"ir: "<<this->getInterval()<<endl;
			cout<<"NUD: "<<NUD<<endl<<"Th: "<<(2.0/3.0 * serverdata1.size())<<
				endl<<"Tl: "<<(1.0/3.0 * serverdata1.size())<<endl;
			interval = interval+interval;
			cout<<"new ir: "<<this->getInterval()<<endl;
			cout<<"Too small an interval! Multiplying by 2"<<endl<<endl;
		}
		else // Tl < NUD < Th
		{
			cout<<"ir: "<<this->getInterval()<<endl;
			cout<<"NUD: "<<NUD<<endl<<"Th: "<<(2.0/3.0 * serverdata1.size())<<
				endl<<"Tl: "<<(1.0/3.0 * serverdata1.size())<<endl;
			cout<<"The interval is right and remains as is!"<<endl;
			cout<<"new ir: "<<this->getInterval()<<endl<<endl;
		}
	}
	void resetCounters() //resets the counters in the nodes
	{
		for(vd::iterator d=serverdata1.begin(); d!=serverdata1.end(); d++)
		{
			(*d)->reset();
		}
	}
	void resetdata1Groups() //resets the data1 categories in the nodes
	{
		hu.clear();
	}
};

void allocatedata1(server& exp) //data1 allocation in nodes
{
	cout<<"State: Randomizing nodes' cache\t";
	for(vd::iterator p=exp.serverdata1.begin(); p!=exp.serverdata1.end(); p++)
	{
		int selected = rand()%exp.graph.size();
		exp.graph[selected]->insertdata1(*(*p));
	}
	cout<<endl;
}

void askdata1(server& exp) //random query generator
{
	time_t t;
	int selected2 = rand()%exp.serverdata1.size();
	time(&t);
	exp.serverdata1[selected2]->setquery(static_cast<int>(t));
}

void updatedata1(server& exp) //random update generator
{
	time_t t;
	int selected2 = rand()%exp.serverdata1.size();
	time(&t);
	exp.serverdata1[selected2]->setupdate(static_cast<int>(t));
}

//void data1Validation(server& exp)
//{
//	for(vn::iterator n=exp.graph.begin(); n!=exp.graph.end(); n++)
//	{
//		(*n)->checkdata1();
//	}
//};

ostream& operator<<(ostream& out, importance i) //overload the << operator to show data1 category
{ 
	if(i == HU)
	{
		out<<"Hot update";
	}
	else if(i == CU)
	{
		out<<"Cold update";
	}
	else if(i == HQ)
	{
		out<<"Hot query";
	}
	else if(i == CQ)
	{
		out<<"Cold query";
	}
	else
	{
		out<<"Remainder (RMA)";
	}
	return out;
};

int main()
{
	srand((unsigned)time(NULL));
	server example;
	allocatedata1(example);
	example.setInterval(50); //arbitrary initialization 
	cout<<endl<<"Beginning experiment, in order to stop it press Ctrl + C"<<endl<<endl;
	while(1)
	{
		time_t end = time(NULL) + example.getInterval(); //actions between intervals
		while(time(NULL) <= end)
		{
			if(rand()%32768>327675) //increasing
				askdata1(example);
			//if(rand()%10000>9998) //decreasing
			//	askdata1(example);
			if(rand()%32768>32765) //increasing
				updatedata1(example);
			//if(rand()%10000>9998) //decraesing
			//	updatedata1(example);
		}
	
		example.data1Classification();
		example.setReport();
		example.sendReport();
		
		example.calcInterval();

		example.resetCounters();
		example.resetdata1Groups();
	}
}