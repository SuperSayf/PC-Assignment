#include <bits/stdc++.h>
using namespace std;
#define INF INT_MAX

int main()
{
    int n, m, s; // n is number of vertices, m is number of edges, s is source vertex
    cin >> n >> m;

    s = 0; // source vertex is 0

    // adjacency list representation of the graph
    vector<pair<int, int>> adj[n];
    for (int i = 0; i < m; i++)
    {
        int u, v, w;
        cin >> u >> v >> w;
        adj[u].push_back({v, w});
        adj[v].push_back({u, w}); // add this line for undirected graph
    }

    // Dijkstra's algorithm
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    vector<int> dist(n, INF);  // stores the minimum cost to reach each vertex from the source
    vector<int> parent(n, -1); // stores the parent of each vertex in the shortest path from the source
    pq.push({0, s});
    dist[s] = 0;
    while (!pq.empty())
    {
        int u = pq.top().second;
        pq.pop();
        for (auto p : adj[u])
        {
            int v = p.first, w = p.second;
            if (dist[u] + w < dist[v])
            {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
                parent[v] = u;
            }
        }
    }

    // output the result
    cout << "The distance from 0 to each vertex is:\n";
    for (int i = 1; i < n; i++)
    {
        cout << "Vertex " << i << ": ";
        if (dist[i] == INF)
            cout << "infinite";
        else
            cout << dist[i];
        cout << endl;
    }
    cout << "\nThe shortest path from 0 to each vertex is:\n";
    for (int i = 0; i < n; i++)
    {
        if (i == s)
            continue;
        cout << "Shortest path to vertex " << i << ": ";
        if (dist[i] == INF)
            cout << "no path";
        else
        {
            vector<int> path;
            int j = i;
            while (j != -1)
            {
                path.push_back(j);
                j = parent[j];
            }
            reverse(path.begin(), path.end());
            for (int x : path)
                cout << x << " ";
        }
        cout << endl;
    }
    return 0;
}
