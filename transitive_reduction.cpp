#include <iostream>
#include <vector>
#include <list>
#include <stack>
#include <algorithm>
#include <fstream>

using namespace std;

//lista de adjacencia para representar o grafo
class Graph {
public:
    int V;
    vector<list<int>> adj;  

    Graph(int v) : V(v), adj(v) {}

    void addEdge(int u, int v) {
        auto& lst = adj[u];
        if (find(lst.begin(), lst.end(), v) == lst.end())
            lst.push_back(v);
    }

    void removeEdge(int u, int v) {
        adj[u].remove(v);
    }

    bool hasEdge(int u, int v) const {
        const auto& lst = adj[u];
        return find(lst.begin(), lst.end(), v) != lst.end();
    }

    //dfs
    bool isReachable(int src, int target) const {
        vector<bool> visited(V, false);
        stack<int> stk;

        stk.push(src);
        visited[src] = true;

        while (!stk.empty()) {
            int u = stk.top(); stk.pop();

            for (int w : adj[u]) {
                //ignora aresta direta
                if (u == src && w == target)
                    continue;

                if (w == target)
                    return true;

                if (!visited[w]) {
                    visited[w] = true;
                    stk.push(w);
                }
            }
        }
        return false;
    }

    void transitiveReduction() {
        vector<pair<int,int>> toRemove;

        for (int u = 0; u < V; u++) {
            //copia dos vizinhos
            vector<int> neighbors(adj[u].begin(), adj[u].end());

            for (int v : neighbors) {
                if (isReachable(u, v)) {
                    toRemove.push_back({u, v});
                }
            }
        }

        for (auto& [u, v] : toRemove) {
            removeEdge(u, v);
        }
    }

    void print(ostream& out = cout) const {
        for (int u = 0; u < V; u++) {
            out << u << " -> ";
            bool first = true;
            for (int v : adj[u]) {
                if (!first) out << ", ";
                out << v;
                first = false;
            }
            out << "\n";
        }
    }

    int edgeCount() const {
        int count = 0;
        for (int u = 0; u < V; u++)
            count += adj[u].size();
        return count;
    }
};

/*
    exemplo input 
    4 5      <- 4 vertices (0,1,2,3), 5 arestas
    0 1      <- aresta do vertice 0 ate o vertice 1
    0 2      <- aresta do vertice 0 ate o vertice 2
    1 2      <- aresta do vertice 1 ate o vertice 2
    2 3      <- aresta do vertice 2 ate o vertice 3
    0 3      <- aresta do vertice 0 ate o vertice 3
*/
Graph readGraph(istream& in) {
    int V, E;
    in >> V >> E;
    Graph g(V);
    for (int i = 0; i < E; i++) {
        int u, v;
        in >> u >> v;
        g.addEdge(u, v);
    }
    return g;
}

int main(int argc, char* argv[]) {
    //ler do arquivo ou terminal
    istream* input = &cin;
    ifstream file;
    if (argc > 1) {
        file.open(argv[1]);
        if (!file) {
            cerr << "Erro ao abrir arquivo " << argv[1] << endl;
            return 1;
        }
        input = &file;
    }

    Graph g = readGraph(*input);

    cout << "Grafo original:" << endl;
    g.print();
    cout << "Arestas: " << g.edgeCount() << "\n\n";

    g.transitiveReduction();

    cout << "Apos reducao de transitividade" << endl;
    g.print();
    cout << "Arestas: " << g.edgeCount() << endl;

    return 0;
}
