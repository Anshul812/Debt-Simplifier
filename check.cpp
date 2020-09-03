#include <iostream>
#include <conio.h>
#include <fstream>
#include <string>
#include <stdio.h>
#include <iomanip>
#include <map>
#include <vector>
#include <queue>
#include <set>
using namespace std;
ifstream fin;
ofstream fout;
fstream f;
map<int,int> id2index , index2id;
map<int,string> index2name;

class Edge
{
public:
	int from , to , capacity , flow , residual;
	Edge (int f , int t , int c)
	{
		from = f;
		to = t;
		capacity = c;
		flow = 0;
		residual = -1;
	}

	//TODO
	int remainingcapacity()
	{
		return capacity - flow;
	}
};

class Base
{
private:
	void execute()
	{
		if(solved)
			return;

		solved = true;
		solve();
		return;
	}

protected:
	int n , s , t , maxflow;
	vector<Edge> *graph;
	vector<Edge> edges;
	bool solved;
	map<int , int> edgestocome;
public:
	Base(int vertex)
	{
		n = vertex;
		maxflow = 0;
		graph = new vector<Edge>[n];
		//solved = false;
	}

	void addEdges(vector<Edge> new_edges)
	{
		for(int i=0 ; i<new_edges.size() ; i++)
		{
			addEdge(new_edges[i].from , new_edges[i].to , new_edges[i].capacity);
		}
	}

	void addEdge(int fr , int t , int c)
	{
		int ost = 100000;
		if(!checkedge(fr , t))
		{
			Edge e1(fr , t , c);
			Edge e2(t , fr , 0);

			e1.residual = graph[t].size();
			e2.residual = graph[fr].size();

			graph[fr].push_back(e1);
			graph[t].push_back(e2);

			edges.push_back(e1);
		}

		else
		{
			graph[t][edgestocome[t*ost+fr]].capacity -= c;
		}
	}

	bool checkedge(int fr , int t)
	{
		int ost = 100000;
		if(edgestocome.find(t*ost+fr)  == edgestocome.end())
		{
			edgestocome[fr*ost + t] = graph[t].size();
			edgestocome[t*ost + fr] = graph[fr].size();
			return false;
		}

		else
		{
			return true;
		}
	}

	vector<Edge> *getgraph()
	{
		execute();
		return graph;
	}

	vector<Edge> getedges()
	{
		return edges;
	}

	int getmaxflow()
	{
		return maxflow;
	}

	void setsink(int sink)
	{
		t = sink;
	}

	void setsource(int source)
	{
		s = source;
	}

	int getsource()
	{
		return s;
	}

	int getsink()
	{
		return t;
	}

	void recompute()
	{
		solved = false;
	}

	void printEdges()
	{
		for(int i=0 ; i<edges.size() ; i++)
		{
			cout << "\n" <<index2name[edges[i].from] << "   -------" << edges[i].capacity << "------>  " << index2name[edges[i].to] ;
		}
	}
	virtual void solve()=0;
};

class Algo : public Base
{
private:
	int *level;

	bool BFS()
	{
		for(int i=0 ; i<n ; i++)
			level[i] = -1;

		level[s] = 0;
		queue<int> q;
		q.push(s);

		while(!q.empty())
		{
			int top = q.front();
			q.pop();
			for(int i=0 ; i<graph[top].size() ; i++)
			{
				Edge e = graph[top][i];
				int cap = e.remainingcapacity();

				if(cap > 0 && level[e.to] == -1)
				{
					level[e.to] = level[top] + 1;
					q.push(e.to);
				}
			}
		}

		return level[t] != -1;
	}

	int DFS(int u , int *next , int flow)
	{
		if(u == t)
			return flow;

		for( ; next[u] < graph[u].size() ; next[u]++)
		{
			Edge edge = graph[u][next[u]];
			int cap = edge.remainingcapacity();

			if(cap > 0 && level[edge.to] == (level[u] + 1))
			{
				int bneck = DFS(edge.to , next , min(flow , cap));

				if(bneck > 0)
				{
					graph[u][next[u]].flow += bneck;
					graph[graph[u][next[u]].to][graph[u][next[u]].residual].flow -= bneck;
					return bneck;
				}
			}
		}

		return 0;
	}

public:
	Algo(int vertex):Base(vertex)
	{
		level = new int[vertex];
	}

	void solve()
	{
		int *next = new int[n];

		while(BFS())
		{
			for(int i=0 ; i<n ; i++)
				next[i] = 0;

			for(int f=DFS(s,next,INT_MAX) ; f!=0 ; f=DFS(s,next,INT_MAX))
			{
				//cout << s << endl;
				//cout << f << endl;
				maxflow += f;
			}
		}
	}
};

class DebtSimplifier
{
private:
	int offset;
	set<int> visitedEdges;
	int n_people;
	vector<Edge> vedge;

	int getHashKey(int u , int v)
	{
		return u*offset+v;
	}

	int getNonvisitedEdges(vector<Edge> edges)
	{
		int edge_pos = -1;
		int cur_pos = 0;

		for(int i=0 ; i<edges.size() ; i++)
		{
			if(visitedEdges.find(getHashKey(edges[i].from , edges[i].to)) == visitedEdges.end())
			{
				edge_pos = cur_pos;
			}

			cur_pos++;
		}

		return edge_pos;
	}

	Algo addallTransaction(Algo solver)
	{
		//solver.addEdge(1,2,40);
		//solver.addEdge(2,3,20);
		//solver.addEdge(3,4,50);
		//solver.addEdge(5,1,10);
		//solver.addEdge(5,2,30);
		//solver.addEdge(5,3,10);
		//solver.addEdge(5,4,10);
		//solver.addEdge(6,1,30);
		//solver.addEdge(6,3,10);

		solver.addEdges(vedge);
		return solver;
	}

public:
	DebtSimplifier(int n , vector<Edge> v)
	{
		offset = 10000000 ;
		n_people = n;
		vedge = v;
	}

	void simplify()
	{
		Algo solver(n_people);
		solver = addallTransaction(solver);

		int edge_pos;

		while(1)
		{
			edge_pos = getNonvisitedEdges(solver.getedges());
			if(edge_pos == -1)
				break;

			solver.recompute();
			Edge fe = solver.getedges().at(edge_pos);
			solver.setsource(fe.from);
			solver.setsink(fe.to);

			vector<Edge> *residualGraph = solver.getgraph();
			vector<Edge> newEdges;

			for(int i=0 ; i<n_people ; i++)
			{
				for(int j=0 ; j<residualGraph[i].size() ; j++)
				{
					int rf = ((residualGraph[i][j].flow < 0 ) ? residualGraph[i][j].capacity : residualGraph[i][j].remainingcapacity());

					if(rf > 0)
					{
						newEdges.push_back(Edge(residualGraph[i][j].from , residualGraph[i][j].to , rf));
					}
				}
			}

			int maxFlow = solver.getmaxflow();
			int source = solver.getsource();
			int sink = solver.getsink();

			visitedEdges.insert(getHashKey(source , sink));
			Algo newsolver(n_people);
			newsolver.addEdges(newEdges);
			newsolver.addEdge(source , sink , maxFlow);

			//cout << newEdges.size() << endl;
			solver = newsolver;
			
		}

		solver.printEdges();
	}
	
};

class User
{
	int userid;
	char username[50];
public:
	void getUserData()
	{
		cout << "\nEnter User Details.";
		cout << "\nUser ID - ";
		cin >> userid;
		cin.ignore();
		cout << "\nUser Name - ";
		cin.getline(username , 49);
	}

	void showUserData()
	{
		cout << "\n" << userid << setw(35) << username;
	}

	int getUserID()
	{
		return userid;
	}

	string getUserName()
	{
		return username;
	}

	void updateName()
	{
		cout << "\nEnter the new User Name - ";
		cin.getline(username , 49);
	}

	void setuserid()
	{

	}

	void addUser();
	void displayUsers();
	void deleteUser(int);
	void searchUser(int);
	void updateUser(int);

};

void User::addUser()
{
	system("cls");
	getUserData();
	fout.open("check.dat",ios::app|ios::out);
	fout.write((char *)this , sizeof(*this));
	fout.close();
	getch();
}

void User::displayUsers()
{
	system("cls");
	fin.open("check.dat",ios::in);
	if(!fin)
	{
		cout << "File not found";
		return;
	}

	fin.read((char *)this , sizeof(*this));
	cout << "\n\t" << setw(17) << "Users\n" ;
	cout << "\n" << "UserID" << setw(35) << "UserName";
	while(!fin.eof())
	{
		showUserData();
		fin.read((char *)this,sizeof(*this));
	}
	fin.close();

	getch();
}

void User::deleteUser(int t)
{
	fin.open("User.dat",ios::in);
	fout.open("temp.dat",ios::out|ios::app);
	if(!fin)
	{
		cout << "\nFile not found";
		return;
	}
	fin.read((char *)this , sizeof(*this));
	while(!fin.eof())
	{
		if(getUserID() != t)
		{
			fout.write((char *)this , sizeof(*this));
		}
		fin.read((char *)this , sizeof(*this));
	}
	fin.close();
	fout.close();

	remove("User.dat");
	rename("temp.dat" , "User.dat");

}

void User::searchUser(int t)
{
	fin.open("User.dat",ios::in);
	if(!fin)
	{
		cout << "\nFile not found.";
	}

	fin.read((char *)this , sizeof(*this));
	while(!fin.eof())
	{
		if(getUserID() == t)
		{
			showUserData();
		}
		fin.read((char*)this , sizeof(*this));
	}
	fin.close();
}

void User::updateUser(int t)
{
	f.open("User.dat",ios::in|ios::out|ios::ate);
	f.seekg(0);
	if(!f)
	{
		cout << "\nFile not found.";
		return;
	}
	f.read((char *)this,sizeof(*this));

	while(!f.eof())
	{
		if(getUserID() == t)
		{
			updateName();
			f.seekp(f.tellp()-sizeof(*this));
			f.write((char*)this , sizeof(*this));
		}
		f.read((char*)this , sizeof(*this));
	}
	f.close();
}

class Transaction
{
	int transactionid;
	int transactionamount;
	int giverid ; int receiverid;
public:
	void getTransactionData()
	{
		
		cout << "\nEnter the Transaction Details.";
		cout << "\nEnter the Transaction ID - ";
		cin >> transactionid;
		cout << "\nEnter the Transaction Amount - ";
		cin >> transactionamount;
		cout << "\nEnter the GiverID - ";
		cin >> giverid;
		cout << "\nEnter the ReceiverID - ";
		cin >> receiverid;
	}

	void showTransactionData()
	{
		cout << "\n"<< giverid << setw(35) << receiverid << setw(35) << transactionamount; 
	}

	int getTransactionID()
	{
		return transactionid;
	}

	int getTransactionAmount()
	{
		return transactionamount;
	}

	int getTransactionGiverID()
	{
		return giverid;
	}

	int getTransactionReceiverID()
	{
		return receiverid;
	}

	void updateTransactionDetails()
	{
		cout << "\nUpdate transaction details.";
		cout << "Enter the Transaction Amount.-";
		cin >> transactionamount;
		cout << "\nEnter the GiverID - ";
		cin >> giverid;
		cout << "\nEnter the ReceiverID - ";
		cin >> receiverid;
	}

	void addTransaction();
	void dislpayAllTransaction();
	void deleteTransaction(int);
	void searchTransaction(int);
	void updateTransaction(int);
};

void Transaction::addTransaction()
{
	system("cls");
	getTransactionData();
	fout.open("Transaction.dat" , ios::out | ios::app);
	fout.write((char*)this , sizeof(*this));
	fout.close();
	getch();
}

void Transaction::dislpayAllTransaction()
{
	system("cls");
	fin.open("Transaction.dat" , ios::in);
	if(!fin)
	{
		cout << "\nFile not found.";
		return;
	}
	fin.read((char*)this , sizeof(*this));
	cout << "\n\t" << setw(35) <<"TRANSACTIONS\n";
	cout << "\n" << "GiverID" << setw(35) << "ReceiverID" << setw(35) << "Amount";  
	while(!fin.eof())
	{
		showTransactionData();
		fin.read((char*)this , sizeof(*this));
	}
	fin.close();
	getch();
}

void Transaction::deleteTransaction(int t)
{
	fin.open("Transaction.dat" , ios::in);
	fout.open("temp1.dat" , ios::app|ios::out);
	if(!fin)
	{
		cout << "\nFile does not Exist.";
		return;
	}
	fin.read((char*)this , sizeof(*this));

	while(!fin.eof())
	{
		if(getTransactionID() != t)
		{
			fout.write((char*)this , sizeof(*this));
		}
		fin.read((char*)this , sizeof(*this));
	}

	fin.close();
	fout.close();
	remove("Transaction.dat");
	rename("temp1.dat" , "Transaction.dat");
}

void Transaction::searchTransaction(int t)
{
	fin.open("Transaction.dat",ios::in);
	if(!fin)
	{
		cout << "\nFile not found.";
		return;
	}
	fin.read((char*)this , sizeof(*this));
	while(!fin.eof())
	{
		if(getTransactionID() == t)
		{
			showTransactionData();
		}
		fin.read((char*)this , sizeof(*this));
	}
	fin.close();
}

void Transaction::updateTransaction(int t)
{
	f.open("Transaction.dat",ios::in | ios::out | ios::ate);
	f.seekg(0);
	if(!f)
	{
		cout << "\nFile not exist.";
		return;
	}
	f.read((char*)this , sizeof(*this));
	
	while(!f.eof())
	{
		if(getTransactionID() == t)
		{
			updateTransactionDetails();
			f.seekp(f.tellp() - sizeof(*this));
			f.write((char*)this , sizeof(*this));
		}
		f.read((char*)this , sizeof(*this));
	}
	f.close();
}

void getMap()
{
	User u1 ; 
	fin.open("check.dat",ios::in);
	if(!fin)
	{
		cout << "\nFile does not Exist.";
		fin.close();
		return;
	}

	
	else
	{
		int i=0;
		fin.read((char *)&u1 , sizeof(u1));
		while(!fin.eof())
		{
			if(id2index.find(u1.getUserID()) == id2index.end())
			{
				id2index[u1.getUserID()] = i;
				index2id[i] = u1.getUserID();
				index2name[i] = u1.getUserName();
			}
			i++;
			fin.read((char*)&u1 , sizeof(u1));
		}

	}

	fin.close();
	return ;
}

void getEd(vector<Edge> &input , map<int,int> &id2index)
{
	Transaction t1 ;
	fin.open("Transaction.dat" , ios::in);
	if(!fin)
	{
		cout << "\nFile not found.";
		fin.close();
		return;
	}

	else
	{
		fin.read((char*)&t1 , sizeof(t1));
		while(!fin.eof())
		{
			input.push_back(Edge(id2index[t1.getTransactionGiverID()] , id2index[t1.getTransactionReceiverID()] , t1.getTransactionAmount()));
			fin.read((char*)&t1 , sizeof(t1));
		}
	}

	fin.close();
	return;
}


void SimplifyTransaction()
{
	system("cls");
	id2index.clear();
	index2id.clear();
	index2name.clear();
	getMap();

	vector<Edge> input;
	getEd(input , id2index);

	int n_people = id2index.size();

	cout << "\n\tSimplified Debts.";
	cout << "\n"<< "GiverID  ------ Amount ----->  ReceiverID.";

	DebtSimplifier ds(n_people , input);
	ds.simplify();
	getch();
}

void menu()
{
	system("cls");
	char ch; User u; Transaction t;
	cout << "\n\n\tDebt Simplifier.";
	cout << "\n\t1. Add User.";
	cout << "\n\t2. View all User.";
	cout << "\n\t3. Add Transaction.";
	cout << "\n\t4. View all Transaction.";
	cout << "\n\t5. Simplify Debts.";
	cout << "\n\t0. Exit.";
	cout << "\nPlease select option. ";
	ch = getch();

	switch(ch)
	{
		case '1':u.addUser();menu();break;
		case '2':u.displayUsers();menu();break;
		case '3':t.addTransaction();menu();break;
		case '4':t.dislpayAllTransaction();menu();break;
		case '5':SimplifyTransaction();menu();break;
		case '0':return;break;
		default : cout << "Wrong Entry..Try Again."; getch();menu();break;
	}


}

int main()
{
	menu();
	return 0;
}