// C++ Implementation of Kosaraju's algorithm to print all SCCs
#define PY_MAJOR_VERSION 3
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <iostream>
#include <list>
#include <vector>
#include <stack>
using namespace std;

class Committee
{
public:
    Committee();
    void printMember();
    void add(int v);
    int get(int idx);

    int numV; // No. of vertices in this committee
    vector<int> member; // An list of id of vertices in this committee

    bool operator == (const Committee &rhs);
};

bool Committee::operator == (const Committee &rhs){
    return this == &rhs;
}

Committee::Committee()
{
    numV = 0;
}

void Committee::printMember()
{
    list<int>::iterator i;
    for(int i = 0; i < numV; i++){
        cout << member[i] << " ";
    }
}

void Committee::add(int v)
{
    numV++;
    member.push_back(v);
}

int Committee::get(int idx)
{
    return member[idx];
}

class Graph
{
    int V;          // No. of vertices
    list<int> *adj; // An array of adjacency lists

    int nCommittee;           // No. of committee

    // Fills Stack with vertices (in increasing order of finishing
    // times). The top element of stack has the maximum finishing
    // time
    void fillOrder(int v, bool visited[], stack<int> &Stack);

    // A recursive function to print DFS starting from v
    void DFSUtil(int v, bool visited[]);
    void DFS(int v, bool vistied[], Committee& Comm);

public:
    Graph(int V);
    void addEdge(int v, int w);

    // The main function that finds and prints strongly connected
    // components
    void printSCCs();
    vector<Committee*> findCommittees(int minV);

    // Function that returns reverse (or transpose) of this graph
    Graph getTranspose();
};

Graph::Graph(int V)
{
    this->V = V;
    adj = new list<int>[V];
}

// A recursive function to print DFS starting from v
void Graph::DFSUtil(int v, bool visited[])
{
    // Mark the current node as visited and print it
    visited[v] = true;
    cout << v << " ";

    // Recur for all the vertices adjacent to this vertex
    list<int>::iterator i;
    for (i = adj[v].begin(); i != adj[v].end(); ++i)
        if (!visited[*i])
            DFSUtil(*i, visited);
}
void Graph::DFS(int v, bool visited[], Committee& Comm)
{
    visited[v] = true;
    Comm.add(v);

    list<int>::iterator i;
    for (i = adj[v].begin(); i != adj[v].end(); ++i)
        if (!visited[*i])
            DFS(*i, visited, Comm);
}

Graph Graph::getTranspose()
{
    Graph g(V);
    for (int v = 0; v < V; v++)
    {
        // Recur for all the vertices adjacent to this vertex
        list<int>::iterator i;
        for(i = adj[v].begin(); i != adj[v].end(); ++i)
        {
            g.adj[*i].push_back(v);
        }
    }
    return g;
}

void Graph::addEdge(int v, int w)
{
    adj[v].push_back(w); // Add w to v’s list.
}

void Graph::fillOrder(int v, bool visited[], stack<int> &Stack)
{
    // Mark the current node as visited and print it
    visited[v] = true;

    // Recur for all the vertices adjacent to this vertex
    list<int>::iterator i;
    for(i = adj[v].begin(); i != adj[v].end(); ++i)
        if(!visited[*i])
            fillOrder(*i, visited, Stack);

    // All vertices reachable from v are processed by now, push v
    Stack.push(v);
}

// The main function that finds and prints all strongly connected
// components
void Graph::printSCCs()
{
    stack<int> Stack;

    // Mark all the vertices as not visited (For first DFS)
    bool *visited = new bool[V];
    for(int i = 0; i < V; i++)
        visited[i] = false;

    // Fill vertices in stack according to their finishing times
    for(int i = 0; i < V; i++)
        if(visited[i] == false)
            fillOrder(i, visited, Stack);

    // Create a reversed graph
    Graph gr = getTranspose();

    // Mark all the vertices as not visited (For second DFS)
    for(int i = 0; i < V; i++)
        visited[i] = false;

    // Now process all vertices in order defined by Stack
    while (Stack.empty() == false)
    {
        // Pop a vertex from stack
        int v = Stack.top();
        Stack.pop();

        // Print Strongly connected component of the popped vertex
        if (visited[v] == false)
        {
            gr.DFSUtil(v, visited);
            cout << endl;
        }
    }
}

vector<Committee*> Graph::findCommittees(int minV)
{
    stack<int> Stack;

    bool *visited = new bool[V];
    for(int i = 0; i < V; i++)
        visited[i] = false;

    for(int i = 0; i < V; i++)
        if(visited[i] == false)
            fillOrder(i, visited, Stack);

    Graph gt = getTranspose();

    for(int i = 0; i < V; i++)
        visited[i] = false;

    vector<Committee*> CommList;
    while (Stack.empty() == false){
        int v = Stack.top();
        Stack.pop();

        if (visited[v] == false){
            Committee *Comm = new Committee();
            gt.DFS(v, visited, *Comm);
            if(Comm->numV >= minV)
                CommList.push_back(Comm);
        }
    }

    return CommList;
}


// "SFBA" should be the name of dynamic lib you compile
BOOST_PYTHON_MODULE(SFBA) {
  using namespace boost::python;

  class_<Graph>("graph", init<int>())
      .def("addEdge", &Graph::addEdge)
      .def("printSCCs", &Graph::printSCCs)
      .def("findCommittees", &Graph::findCommittees)
      .def("getTranspose", &Graph::getTranspose)
      ;

  class_<Committee, Committee*>("committee", init<>())
      .def("printMember", &Committee::printMember)
      .def("add", &Committee::add)
      .def("get", &Committee::get)
      .def_readwrite("numV", &Committee::numV)
      .def_readwrite("member", &Committee::member)
      ;

  class_<vector<Committee*>>("vector_committee")
      .def(vector_indexing_suite<vector<Committee*>>())
      ;
  
  class_<vector<int>>("vector_int")
      .def(vector_indexing_suite<vector<int>>())
      ;
}