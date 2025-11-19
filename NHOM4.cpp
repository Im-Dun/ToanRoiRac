#include <bits/stdc++.h>
using namespace std;

const long long INF = (1LL<<60);

struct AdjEdge {
    int v;
    long long w;
    AdjEdge(int vv = 0, long long ww = 0) : v(vv), w(ww) {}
};

struct Edge {
    int u, v;
    long long w;
    Edge(int uu=0,int vv=0,long long ww=0):u(uu),v(vv),w(ww){}
};

class Graph {
private:
    int n; 
    int m;
    vector<vector<long long>> mat;         
    vector<vector<AdjEdge>> adjList;     
    vector<Edge> edges;                  

public:
    Graph(): n(0), m(0) {}
    Graph(int n_): n(n_), m(0) {
        mat.assign(n+1, vector<long long>(n+1, INF));
        for (int i = 1; i <= n; ++i) mat[i][i] = 0;
        adjList.assign(n+1, vector<AdjEdge>());
        edges.clear();
    }

    void reset(int n_) {
        n = n_;
        m = 0;
        mat.assign(n+1, vector<long long>(n+1, INF));
        for (int i = 1; i <= n; ++i) mat[i][i] = 0;
        adjList.assign(n+1, vector<AdjEdge>());
        edges.clear();
    }

    int numVertices() const { return n; }
    int numEdges() const { return m; }


    void addEdge(int u, int v, long long w) {
        if (u < 1 || u > n || v < 1 || v > n) {
            cerr << "ERROR: addEdge: chi so dinh ngoai pham vi\n";
            return;
        }
        if (w < mat[u][v]) {
            mat[u][v] = w;
            mat[v][u] = w;
        }

        edges.emplace_back(u,v,w);
    }

    void buildAdjListFromMatrix() {
        adjList.assign(n+1, vector<AdjEdge>());
        for (int u = 1; u <= n; ++u) {
            for (int v = 1; v <= n; ++v) {
                if (u != v && mat[u][v] < INF) {
                    adjList[u].push_back(AdjEdge(v, mat[u][v]));
                }
            }
        }
    }

    void printMatrix(ostream &out = cout) const {
        out << "\nMa tran trong so (INF = -):\n    ";
        for (int j = 1; j <= n; ++j) out << j << (j < 10 ? "   " : "  ");
        out << "\n";
        for (int i = 1; i <= n; ++i) {
            out << setw(2) << i << " | ";
            for (int j = 1; j <= n; ++j) {
                if (mat[i][j] >= INF/2) out << setw(3) << "-" << "  ";
                else out << setw(3) << mat[i][j] << "  ";
            }
            out << "\n";
        }
    }

    void printAdjList(ostream &out = cout) const {
        out << "\nDanh sach ke:\n";
        for (int u = 1; u <= n; ++u) {
            out << u << ": ";
            for (auto &e : adjList[u]) {
                out << "(" << e.v << ",w=" << e.w << ") ";
            }
            out << "\n";
        }
    }

    const vector<vector<long long>>& getMatrix() const { return mat; }
    const vector<vector<AdjEdge>>& getAdjList() const { return adjList; }

    bool isConnected() {

        buildAdjListFromMatrix();
        vector<int> vis(n+1, 0);
        queue<int> q;
        q.push(1);
        vis[1] = 1;
        int cnt = 0;
        while (!q.empty()) {
            int u = q.front(); q.pop(); ++cnt;
            for (auto &e : adjList[u]) {
                if (!vis[e.v]) {
                    vis[e.v] = 1;
                    q.push(e.v);
                }
            }
        }
        return cnt == n;
    }

    vector<Edge> primMatrix(int start = 1) {
        vector<Edge> mst;
        if (n == 0) return mst;
        if (!isConnected()) return mst;

        vector<long long> key(n+1, INF);
        vector<int> parent(n+1, -1);
        vector<bool> inMST(n+1, false);

        key[start] = 0;

        for (int i = 1; i <= n; ++i) {

            int u = -1;
            long long minv = INF;
            for (int v = 1; v <= n; ++v) {
                if (!inMST[v] && key[v] < minv) {
                    minv = key[v];
                    u = v;
                }
            }
            if (u == -1) break;
            inMST[u] = true;
            if (parent[u] != -1) {
                mst.emplace_back(parent[u], u, mat[parent[u]][u]);
            }

            for (int v = 1; v <= n; ++v) {
                if (!inMST[v] && mat[u][v] < key[v]) {
                    key[v] = mat[u][v];
                    parent[v] = u;
                }
            }
        }
        return mst;
    }

    vector<Edge> primHeap(int start = 1) {
        vector<Edge> mst;
        if (n == 0) return mst;
        buildAdjListFromMatrix();
        if (!isConnected()) return mst;

        vector<long long> key(n+1, INF);
        vector<int> parent(n+1, -1);
        vector<bool> inMST(n+1, false);

        typedef pair<long long,int> pli;
        priority_queue<pli, vector<pli>, greater<pli>> pq;

        key[start] = 0;
        pq.push({0, start});

        while (!pq.empty()) {
            auto cur = pq.top(); pq.pop();
            long long cur_w = cur.first;
            int u = cur.second;
            if (inMST[u]) continue;
            inMST[u] = true;
            if (parent[u] != -1) {
                mst.emplace_back(parent[u], u, cur_w);
            }
            for (auto &e : adjList[u]) {
                int v = e.v;
                long long w = e.w;
                if (!inMST[v] && w < key[v]) {
                    key[v] = w;
                    parent[v] = u;
                    pq.push({key[v], v});
                }
            }
        }
        return mst;
    }

    void saveMSTToFile(const vector<Edge> &mst, const string &filename) {
        ofstream fout(filename);
        if (!fout.is_open()) {
            cerr << "Loi: khong mo duoc file " << filename << "\n";
            return;
        }
        long long total = 0;
        fout << "MST edges (u v w):\n";
        for (auto &e : mst) {
            fout << e.u << " " << e.v << " " << e.w << "\n";
            total += e.w;
        }
        fout << "TOTAL " << total << "\n";
        fout.close();
    }
};

void printMenu() {
    cout << "\n====== PRIM MST - MENU ======\n";
    cout << "1. Nhap do thi (n m + m dong u v w)\n";
    cout << "2. In ma tran trong so\n";
    cout << "3. In danh sach ke\n";
    cout << "4. Kiem tra lien thong\n";
    cout << "5. Chay Prim (ma tran O(n^2))\n";
    cout << "6. Chay Prim (heap O(m log n))\n";
    cout << "7. Luu MST ra file\n";
    cout << "8. Thoat\n";
    cout << "Chon: ";
}

int main() {
    Graph g;
    vector<Edge> lastMST;
    bool hasGraph = false;
    string lastFilename = "mst_output.txt";

    while (true) {
        printMenu();
        int cmd;
        if (!(cin >> cmd)) { // EOF
            cout << "Thoat (EOF)\n";
            break;
        }
        if (cmd == 1) {
            int n, m;
            cout << "Nhap n (so dinh) va m (so canh): ";
            cin >> n >> m;
            g.reset(n);
            cout << "Nhap " << m << " canh (u v w):\n";
            for (int i = 0; i < m; ++i) {
                int u, v;
                long long w;
                cin >> u >> v >> w;
                g.addEdge(u, v, w);
            }
            g.buildAdjListFromMatrix();
            hasGraph = true;
            cout << "Nhap xong.\n";
        }
        else if (cmd == 2) {
            if (!hasGraph) { cout << "Chua co do thi. Chon 1 de nhap.\n"; continue; }
            g.printMatrix();
        }
        else if (cmd == 3) {
            if (!hasGraph) { cout << "Chua co do thi. Chon 1 de nhap.\n"; continue; }
            g.buildAdjListFromMatrix();
            g.printAdjList();
        }
        else if (cmd == 4) {
            if (!hasGraph) { cout << "Chua co do thi. Chon 1 de nhap.\n"; continue; }
            bool conn = g.isConnected();
            cout << (conn ? "Do thi lien thong.\n" : "Do thi KHONG lien thong.\n");
        }
        else if (cmd == 5) {
            if (!hasGraph) { cout << "Chua co do thi. Chon 1 de nhap.\n"; continue; }
            cout << "Chay Prim (ma tran) tu dinh 1...\n";
            lastMST = g.primMatrix(1);
            if (lastMST.empty()) {
                cout << "Khong co MST (do thi khong lien thong hoac loi).\n";
            } else {
                long long total = 0;
                cout << "\nMST (matrix Prim):\n";
                for (auto &e : lastMST) {
                    cout << e.u << " - " << e.v << "  (w=" << e.w << ")\n";
                    total += e.w;
                }
                cout << "Tong chi phi = " << total << "\n";
            }
        }
        else if (cmd == 6) {
            if (!hasGraph) { cout << "Chua co do thi. Chon 1 de nhap.\n"; continue; }
            cout << "Chay Prim (heap) tu dinh 1...\n";
            lastMST = g.primHeap(1);
            if (lastMST.empty()) {
                cout << "Khong co MST (do thi khong lien thong hoac loi).\n";
            } else {
                long long total = 0;
                cout << "\nMST (heap Prim):\n";
                for (auto &e : lastMST) {
                    cout << e.u << " - " << e.v << "  (w=" << e.w << ")\n";
                    total += e.w;
                }
                cout << "Tong chi phi = " << total << "\n";
            }
        }
        else if (cmd == 7) {
            if (lastMST.empty()) {
                cout << "Chua co ket qua MST de luu. Chay 5 hoac 6 truoc.\n";
                continue;
            }
            cout << "Nhap ten file de luu (mac dinh: " << lastFilename << "): ";
            string fname;
            cin >> ws;
            getline(cin, fname);
            if (fname.empty()) fname = lastFilename;
            g.saveMSTToFile(lastMST, fname);
            cout << "Da luu MST vao file: " << fname << "\n";
        }
        else if (cmd == 8) {
            cout << "Thoat chuong trinh. Bye!\n";
            break;
        }
        else {
            cout << "Lua chon khong hop le. Thu lai.\n";
        }
    }

    return 0;
}

