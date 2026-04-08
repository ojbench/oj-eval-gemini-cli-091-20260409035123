#include <iostream>
#include <vector>

using namespace std;

struct Node {
    int l, r;
    int size;
    long long val;
    unsigned int pri;
};

const int CHUNK_SIZE = 1000000;
vector<Node*> chunks;
int node_cnt = 0;

Node& get_node(int id) {
    return chunks[id / CHUNK_SIZE][id % CHUNK_SIZE];
}

int root[100005];

unsigned int seed = 131;
unsigned int rand_pri() {
    seed = seed * 19260817 + 1;
    return seed;
}

int new_node(long long val) {
    int p = ++node_cnt;
    if (p / CHUNK_SIZE >= chunks.size()) {
        chunks.push_back(new Node[CHUNK_SIZE]);
    }
    Node& n = get_node(p);
    n.l = n.r = 0;
    n.size = 1;
    n.val = val;
    n.pri = rand_pri();
    return p;
}

int clone_node(int p) {
    if (!p) return 0;
    int q = ++node_cnt;
    if (q / CHUNK_SIZE >= chunks.size()) {
        chunks.push_back(new Node[CHUNK_SIZE]);
    }
    get_node(q) = get_node(p);
    return q;
}

void push_up(int p) {
    Node& n = get_node(p);
    n.size = (n.l ? get_node(n.l).size : 0) + (n.r ? get_node(n.r).size : 0) + 1;
}

void split(int p, long long val, int &x, int &y) {
    if (!p) {
        x = y = 0;
        return;
    }
    int q = clone_node(p);
    Node& nq = get_node(q);
    if (nq.val <= val) {
        x = q;
        split(nq.r, val, nq.r, y);
    } else {
        y = q;
        split(nq.l, val, x, nq.l);
    }
    push_up(q);
}

int merge(int x, int y) {
    if (!x || !y) return x ? clone_node(x) : clone_node(y);
    Node& nx = get_node(x);
    Node& ny = get_node(y);
    if (nx.pri > ny.pri) {
        int q = clone_node(x);
        Node& nq = get_node(q);
        nq.r = merge(nq.r, y);
        push_up(q);
        return q;
    } else {
        int q = clone_node(y);
        Node& nq = get_node(q);
        nq.l = merge(x, nq.l);
        push_up(q);
        return q;
    }
}

bool find(int rt, long long val) {
    int p = rt;
    while (p) {
        Node& np = get_node(p);
        if (np.val == val) return true;
        if (np.val < val) p = np.r;
        else p = np.l;
    }
    return false;
}

bool insert(int &rt, long long val) {
    if (find(rt, val)) return false;
    int x, y;
    split(rt, val, x, y);
    rt = merge(merge(x, new_node(val)), y);
    return true;
}

void erase(int &rt, long long val) {
    if (!find(rt, val)) return;
    int x, y, z;
    split(rt, val, x, z);
    split(x, val - 1, x, y);
    rt = merge(x, z);
}

int count_less_equal(int rt, long long val) {
    int p = rt;
    int res = 0;
    while (p) {
        Node& np = get_node(p);
        if (np.val <= val) {
            res += (np.l ? get_node(np.l).size : 0) + 1;
            p = np.r;
        } else {
            p = np.l;
        }
    }
    return res;
}

int range(int rt, long long l, long long r) {
    if (l > r) return 0;
    return count_less_equal(rt, r) - count_less_equal(rt, l - 1);
}

long long get_pred(int rt, long long val) {
    int p = rt;
    long long res = -1;
    bool found = false;
    while (p) {
        Node& np = get_node(p);
        if (np.val < val) {
            res = np.val;
            found = true;
            p = np.r;
        } else {
            p = np.l;
        }
    }
    return found ? res : -1;
}

long long get_succ(int rt, long long val) {
    int p = rt;
    long long res = -1;
    bool found = false;
    while (p) {
        Node& np = get_node(p);
        if (np.val > val) {
            res = np.val;
            found = true;
            p = np.l;
        } else {
            p = np.r;
        }
    }
    return found ? res : -1;
}

long long get_min(int rt) {
    int p = rt;
    if (!p) return -1;
    while (get_node(p).l) p = get_node(p).l;
    return get_node(p).val;
}

long long get_max(int rt) {
    int p = rt;
    if (!p) return -1;
    while (get_node(p).r) p = get_node(p).r;
    return get_node(p).val;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    chunks.push_back(new Node[CHUNK_SIZE]); // For node 0
    
    int op;
    int lst = 0;
    int it_a = -1;
    long long it_val = 0;
    bool valid = false;
    
    while (cin >> op) {
        long long a, b, c;
        if (op == 0) {
            cin >> a >> b;
            bool res = insert(root[a], b);
            if (res) {
                it_a = a;
                it_val = b;
                valid = true;
            }
        } else if (op == 1) {
            cin >> a >> b;
            if (valid && it_a == a && it_val == b) {
                valid = false;
            }
            erase(root[a], b);
        } else if (op == 2) {
            cin >> a;
            ++lst;
            root[lst] = root[a];
        } else if (op == 3) {
            cin >> a >> b;
            if (find(root[a], b)) {
                cout << "true\n";
                it_a = a;
                it_val = b;
                valid = true;
            } else {
                cout << "false\n";
            }
        } else if (op == 4) {
            cin >> a >> b >> c;
            cout << range(root[a], b, c) << "\n";
        } else if (op == 5) {
            if (valid) {
                long long min_val = get_min(root[it_a]);
                if (it_val == min_val) {
                    valid = false;
                } else {
                    long long pred = get_pred(root[it_a], it_val);
                    if (pred != -1) {
                        it_val = pred;
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
        } else if (op == 6) {
            if (valid) {
                long long succ = get_succ(root[it_a], it_val);
                if (succ != -1) {
                    it_val = succ;
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
    return 0;
}
