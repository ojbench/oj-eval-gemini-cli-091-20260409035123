#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct Query {
    int op;
    long long a, b, c;
};

vector<Query> queries;
vector<long long> vals;

struct Node {
    int l, r;
    int sum;
};

const int CHUNK_SIZE = 1000000;
vector<Node*> chunks;
int node_cnt = 0;

Node& get_node(int id) {
    return chunks[id / CHUNK_SIZE][id % CHUNK_SIZE];
}

int new_node() {
    int p = ++node_cnt;
    if (p / CHUNK_SIZE >= chunks.size()) {
        chunks.push_back(new Node[CHUNK_SIZE]);
    }
    Node& n = get_node(p);
    n.l = n.r = n.sum = 0;
    return p;
}

int clone_node(int p) {
    int q = new_node();
    get_node(q) = get_node(p);
    return q;
}

void update(int &p, int pre, int l, int r, int pos, int val) {
    p = clone_node(pre);
    get_node(p).sum += val;
    if (l == r) return;
    int mid = l + (r - l) / 2;
    if (pos <= mid) {
        update(get_node(p).l, get_node(pre).l, l, mid, pos, val);
    } else {
        update(get_node(p).r, get_node(pre).r, mid + 1, r, pos, val);
    }
}

int query_sum(int p, int l, int r, int ql, int qr) {
    if (!p || ql > r || qr < l) return 0;
    if (ql <= l && r <= qr) return get_node(p).sum;
    int mid = l + (r - l) / 2;
    return query_sum(get_node(p).l, l, mid, ql, qr) + query_sum(get_node(p).r, mid + 1, r, ql, qr);
}

int query_count(int p, int l, int r, int pos) {
    if (!p) return 0;
    if (l == r) return get_node(p).sum;
    int mid = l + (r - l) / 2;
    if (pos <= mid) return query_count(get_node(p).l, l, mid, pos);
    else return query_count(get_node(p).r, mid + 1, r, pos);
}

int find_pred(int p, int l, int r, int pos) {
    if (!p || get_node(p).sum == 0 || l >= pos) return -1;
    if (l == r) return l;
    int mid = l + (r - l) / 2;
    int res = -1;
    if (mid + 1 < pos) {
        res = find_pred(get_node(p).r, mid + 1, r, pos);
    }
    if (res != -1) return res;
    return find_pred(get_node(p).l, l, mid, pos);
}

int find_succ(int p, int l, int r, int pos) {
    if (!p || get_node(p).sum == 0 || r <= pos) return -1;
    if (l == r) return l;
    int mid = l + (r - l) / 2;
    int res = -1;
    if (mid > pos) {
        res = find_succ(get_node(p).l, l, mid, pos);
    }
    if (res != -1) return res;
    return find_succ(get_node(p).r, mid + 1, r, pos);
}

int find_min(int p, int l, int r) {
    if (!p || get_node(p).sum == 0) return -1;
    if (l == r) return l;
    int mid = l + (r - l) / 2;
    if (get_node(get_node(p).l).sum > 0) return find_min(get_node(p).l, l, mid);
    return find_min(get_node(p).r, mid + 1, r);
}

int root[100005];

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    int op;
    while (cin >> op) {
        Query q;
        q.op = op;
        if (op == 0 || op == 1 || op == 3) {
            cin >> q.a >> q.b;
            vals.push_back(q.b);
        } else if (op == 2) {
            cin >> q.a;
        } else if (op == 4) {
            cin >> q.a >> q.b >> q.c;
            vals.push_back(q.b);
            vals.push_back(q.c);
        }
        queries.push_back(q);
    }
    
    sort(vals.begin(), vals.end());
    vals.erase(unique(vals.begin(), vals.end()), vals.end());
    
    int M = vals.size();
    if (M == 0) M = 1;
    
    chunks.push_back(new Node[CHUNK_SIZE]); // For node 0
    
    int lst = 0;
    int it_a = -1;
    long long it_val = 0;
    bool valid = false;
    
    for (const auto& q : queries) {
        if (q.op == 0) {
            int pos = lower_bound(vals.begin(), vals.end(), q.b) - vals.begin() + 1;
            if (query_count(root[q.a], 1, M, pos) == 0) {
                update(root[q.a], root[q.a], 1, M, pos, 1);
                it_a = q.a;
                it_val = q.b;
                valid = true;
            }
        } else if (q.op == 1) {
            int pos = lower_bound(vals.begin(), vals.end(), q.b) - vals.begin() + 1;
            if (valid && it_a == q.a && it_val == q.b) {
                valid = false;
            }
            if (query_count(root[q.a], 1, M, pos) > 0) {
                update(root[q.a], root[q.a], 1, M, pos, -1);
            }
        } else if (q.op == 2) {
            ++lst;
            root[lst] = root[q.a];
        } else if (q.op == 3) {
            int pos = lower_bound(vals.begin(), vals.end(), q.b) - vals.begin() + 1;
            if (query_count(root[q.a], 1, M, pos) > 0) {
                cout << "true\n";
                it_a = q.a;
                it_val = q.b;
                valid = true;
            } else {
                cout << "false\n";
            }
        } else if (q.op == 4) {
            if (q.b > q.c) {
                cout << "0\n";
            } else {
                int l_pos = lower_bound(vals.begin(), vals.end(), q.b) - vals.begin() + 1;
                int r_pos = upper_bound(vals.begin(), vals.end(), q.c) - vals.begin();
                if (l_pos <= r_pos) {
                    cout << query_sum(root[q.a], 1, M, l_pos, r_pos) << "\n";
                } else {
                    cout << "0\n";
                }
            }
        } else if (q.op == 5) {
            if (valid) {
                int min_pos = find_min(root[it_a], 1, M);
                long long min_val = (min_pos != -1) ? vals[min_pos - 1] : -1;
                if (it_val == min_val) {
                    valid = false;
                } else {
                    int pos = lower_bound(vals.begin(), vals.end(), it_val) - vals.begin() + 1;
                    int pred_pos = find_pred(root[it_a], 1, M, pos);
                    if (pred_pos != -1) {
                        it_val = vals[pred_pos - 1];
                    } else {
                        valid = false;
                    }
                }
            }
            if (valid) {
                cout << it_val << "\n";
            } else {
                cout << "-1\n";
            }
        } else if (q.op == 6) {
            if (valid) {
                int pos = lower_bound(vals.begin(), vals.end(), it_val) - vals.begin() + 1;
                int succ_pos = find_succ(root[it_a], 1, M, pos);
                if (succ_pos != -1) {
                    it_val = vals[succ_pos - 1];
                } else {
                    valid = false;
                }
            }
            if (valid) {
                cout << it_val << "\n";
            } else {
                cout << "-1\n";
            }
        }
    }
    
    for (Node* chunk : chunks) {
        delete[] chunk;
    }
    
    return 0;
}
