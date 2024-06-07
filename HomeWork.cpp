/*
    1) A code written in C++ to test whether the input graph is 2-vertex strongly connected.
    2) We assume that the input graph is given as following:
        - N: the number of nodes
        - M: the number of edges
        - Edges: representing the edges of the graph in the form u-v, we will use adjacency list to store the edges.
    3) To test whether the input graph is strongly connected, we use Gabow's algorithm.
    4) To test whether the undirected graph of the input graph is 2-vertex connected, we use Gabow's algorithm.
    5) Sample input:
        4 5
        1 2
        2 3
        3 4
        4 1
        2 4
        where the first line contains N and M. Each of the next M lines contains an edge u-v.
    6)  lines 31-53 written by: علي اصف زاهر
    7)  lines 54-72 written by: سامي هاني جنيدي
    8)  lines 73-91 written by: تيم عبد المنعم منصور
    9)  lines 92-103 written by: هاشم غسان اسماعيل
    10) lines 104-123 written by: كرم خالد بيلونة
    11) lines 124-140 written by: خليل طلال ملحم
    12) lines 141-155 written by: علي محمد صالح
    13) lines 156-172 written by: عبد الرحمن احمد سيد علي
    14) lines 173-189 written by: نوارة مأمون حسن
    15) lines 190-214 written by: محمد عبد الرحيم سليمان
    16) lines 215-236 written by: جويل محمد اسكندر
    17) lines 237-258 written by: مهدي حسام اسعد
    18) lines 259-271 written by: بسمة جهاد جمل
*/
#include<bits/stdc++.h>
using namespace std;
// Implementation of Gabow's algorithm in C++
class Gabow{
private:
    int n; // number of nodes in the graph
    int m; // number of edges in the graph
    int timer; // a timer to use as the dfs_number during dfs calls
    int scc_cnt; // counter for the number of strongly connected components in the graph
    vector<int> dfs_number; // dfs_number for each node representing the time of entering the node for the first time
    vector<int> scc_id; // represents the ID for the SCC to which the node belongs
    vector<vector<int>> adj; // adjacency list representation of the graph
    vector<vector<int>> sccs; // a vector of vectors to store the nodes for each SCC
    stack<int> temp; // will store nodes that don't belong to a SCC till the current dfs call
    stack<int> leaders; // will store nodes that have possibility to represent to a SCC
public:
    // Constructor
    Gabow(int n=0, int m=0, vector<vector<int>> adj={}){
        this->n = n;
        this->m = m;
        this->adj = adj;
        run();
    }
    void dfs(int u){
        // when entering a node, assign 'timer' to its dfs_number and push it to both stacks
        dfs_number[u] = timer++;
        leaders.push(u);
        temp.push(u);
        for(int v : adj[u]){
            // case node v is not visited before
            if(dfs_number[v] == -1){
                dfs(v);
            }
            // case node v has been visited but does not belong to SCC
            else if(scc_id[v] == -1){
                // keep removing nodes from leaders stack till reaching a node with
                // minimum dfs_number that can reach node v
                while(dfs_number[leaders.top()] > dfs_number[v]){
                    leaders.pop();
                }
            }
        }
        // if node u is at the top of the leaders stack, then it will represent a SCC
        if(leaders.top() == u){
            leaders.pop(); // node u represents a SCC
            scc_id[u] = scc_cnt;
            scc_cnt++;
            vector<int> new_scc;
            // keep removing nodes from the temp stack till reaching u and assign their scc_id
            while(temp.top() != u){
                int v = temp.top();
                scc_id[v] = scc_cnt;
                new_scc.push_back(v);
                temp.pop();
            }
            temp.pop();
            new_scc.push_back(u);
            // add the new SCC to our SCCs vector
            sccs.push_back(new_scc);
        }
    }
    // a function to run the algorithm and initialize the variables
    void run(){
        timer = 1;
        scc_cnt = 0;
        dfs_number =  vector<int> (n+1, -1);
        scc_id =  vector<int> (n+1, -1);
        for(int i = 1; i <= n; i++){
            if(dfs_number[i] == -1){
                dfs(i);
            }
        }
    }
    // class methods to get helpful information about the graph connectivity
    bool is_strongly_connected(){
        return scc_cnt == 1;
    }
    int get_num_of_sccs(){
        return scc_cnt;
    }
    int get_scc_id_of_node(int u){
        return scc_id[u];
    }
    void print_nodes_int_scc(int id){
        for(int u : sccs[id]){
            cout << u << ' ';
        }
        cout << '\n';
    }
    bool is_in_the_same_scc(int u, int v){
        return scc_id[u] == scc_id[v];
    }
};
// Implementation of Jens Shmidt algorithm in C++
class Shmidt{
private:
    int n; // number of nodes in the graph
    int m; // number of edges in the graph
    int timer; // a timer to use as the dfs_number during dfs calls
    int cc_cnt; // number of connected components in the graph
    int cycles_cnt; // number of cycles in the chain decomposition
    int paths_cnt; // number of paths in the chain decomposition
    vector<int> dfs_number; // dfs_number for each node representing the time of entering the node for the first time
    vector<int> order; // the order of nodes in ascending order of their dfs_number
    vector<int> up; // to store Tree-edges in reverse, this can be 1D vector since each node is discovered by at most one parent during dfs calls
    vector<bool> vis; // to mark visited nodes in chain decomposition
    vector<vector<int>> adj; // adjacency list representation of the graph
    vector<vector<int>> back_edges; // to store back edges of each node;
    vector<vector<int>> chain_decomposition; // final chain decomposition
public:
    // Constructor
    Shmidt(int n=0, int m=0, vector<vector<int>> adj={}){
        this->n = n;
        this->m = m;
        // the input adjacency is of a directed graph, we have to make it undirected
        vector<vector<int>> temp(n+1);
        for(int u = 1; u <= n; u++){
            for(int v : adj[u]){
                temp[u].push_back(v);
                temp[v].push_back(u);
            }
        }
        this->adj = temp;
        run();
    }
    void dfs(int u, int p = -1){
        order.push_back(u);
        dfs_number[u] = timer++;
        for(int v : adj[u]){
            // u-v is a tree edge
            if(dfs_number[v] == -1){
                // add the tree-edge in reverse
                up[v] = u;
                dfs(v, u);
            }
            // u-v is a back edge
            else if(v != p && dfs_number[v] < dfs_number[u]){
                // add back-edge in reverse
                back_edges[v].push_back(u);
            }
        }
    }
    // a function to run the algorithm and initialize the variables
    void run(){
        timer = 1;
        cc_cnt = 0;
        cycles_cnt = 0;
        paths_cnt = 0;
        dfs_number =  vector<int> (n+1, -1);
        up = vector<int> (n+1);
        back_edges = vector<vector<int>> (n+1);

        for(int i = 1; i <= n; i++){
            if(dfs_number[i] == -1){
                up[i] = -1;
                dfs(i);
                cc_cnt++;
            }
        }
        vis = vector<bool> (n+1, false);
        for(int u : order){
            vis[u] = true;
            for(int v : back_edges[u]){
                int x = v;
                vector<int> new_chain;
                new_chain.push_back(x);
                // keep going up in the tree edges till finding a visited node
                while(!vis[x]){
                    vis[x] = true;
                    new_chain.push_back(x);
                    x = up[x];
                }
                // if the first visited node was node u itself, this means we found a new cycle, otherwise there is a new path
                if(x == u){
                    cycles_cnt++;
                }
                else{
                    paths_cnt++;
                }
                // add the new chain to the final chain decomposition
                chain_decomposition.push_back(new_chain);
            }
        }
    }
    // class methods to get helpful information about the graph
    bool is_2_vertex_connected(){
        return cc_cnt == 1 && cycles_cnt == 1;
    }
    bool is_connected(){
        return cc_cnt == 1;
    }
    void print_chain_decomposition(){
        for(auto v : chain_decomposition){
            for(int u : v){
                cout << u << ' ';
            }
            cout << '\n';
        }
    }
    int get_cycles_count(){
        return cycles_cnt;
    }
    int get_paths_count(){
        return paths_cnt;
    }
};
// A class representing a directed graph
class DirectedGraph{
private:
    int n; // number of nodes in the graph
    int m; // number of edges in the graph
    vector<vector<int>> adj; // adjacency list representation of the graph
    Gabow G; // To apply Gabow's algorithm on the given graph
    Shmidt S; // To apply Jens Shmidt algorithm on the given graph
public:
    // Constructor
    DirectedGraph(int n, int m, vector<vector<int>> adj){
        this->n = n;
        this->m = m;
        this->adj = adj;
        this->S = Shmidt(n, m, adj);
        this->G = Gabow(n, m, adj);
    }
    // a boolean function to test whether the given graph is 2-vertex strongly biconnected,
    bool is_2_vertex_strongly_biconnected(){
        return G.is_strongly_connected() && S.is_2_vertex_connected();
    }
};
int main(){
    int n, m;
    cin >> n >> m;
    vector<vector<int>> adj(n+1);
    for(int i = 0; i < m; i++){
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
    }
    DirectedGraph g(n, m, adj);
    cout << (g.is_2_vertex_strongly_biconnected() ? "YES" : "NO") << '\n';
    return 0;
}
