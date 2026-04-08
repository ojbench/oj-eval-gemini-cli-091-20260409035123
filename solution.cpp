#include <iostream>
#include <vector>

using namespace std;

struct Node {
    int l, r;
    int size;
    long long val;
    unsigned int pri;
};

const int MAX_NODES = 50000005;
Node tr[MAX_NODES];
int root[100005];
int node_cnt = 0;

unsigned int seed = 131;
unsigned int rand_pri() {
    seed = seed * 19260817 + 1;
    return seed;
}

int new_node(long long val) {
    int p = ++node_cnt;
    tr[p].l = tr[p].r = 0;
    tr[p].size = 1;
    tr[p].val = val;
    tr[p].pri = rand_pri();
    return p;
}

int clone_node(int p) {
    if (!p) return 0;
    int q = ++node_cnt;
    tr[q] = tr[p];
    return q;
}

void push_up(int p) {
    tr[p].size = tr[tr[p].l].size + tr[tr[p].r].size + 1;
}

void split(int p, long long val, int &x, int &y) {
    if (!p) {
        x = y = 0;
        return;
    }
    int q = clone_node(p);
    if (tr[q].val <= val) {
        x = q;
        split(tr[q].r, val, tr[q].r, y);
    } else {
        y = q;
        split(tr[q].l, val, x, tr[q].l);
    }
    push_up(q);
}

int merge(int x, int y) {
    if (!x || !y) return x ? clone_node(x) : clone_node(y);
    if (tr[x].pri > tr[y].pri) {
        int q = clone_node(x);
        tr[q].r = merge(tr[q].r, y);
        push_up(q);
        return q;
    } else {
        int q = clone_node(y);
        tr[q].l = merge(x, tr[q].l);
        push_up(q);
        return q;
    }
}

bool find(int rt, long long val) {
    int p = rt;
    while (p) {
        if (tr[p].val == val) return true;
        if (tr[p].val < val) p = tr[p].r;
        else p = tr[p].l;
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
        if (tr[p].val <= val) {
            res += tr[tr[p].l].size + 1;
            p = tr[p].r;
        } else {
            p = tr[p].l;
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
        if (tr[p].val < val) {
            res = tr[p].val;
            found = true;
            p = tr[p].r;
        } else {
            p = tr[p].l;
        }
    }
    return found ? res : -1;
}

long long get_succ(int rt, long long val) {
    int p = rt;
    long long res = -1;
    bool found = false;
    while (p) {
        if (tr[p].val > val) {
            res = tr[p].val;
            found = true;
            p = tr[p].l;
        } else {
            p = tr[p].r;
        }
    }
    return found ? res : -1;
}

long long get_min(int rt) {
    int p = rt;
    if (!p) return -1;
    while (tr[p].l) p = tr[p].l;
    return tr[p].val;
}

long long get_max(int rt) {
    int p = rt;
    if (!p) return -1;
    while (tr[p].r) p = tr[p].r;
    return tr[p].val;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
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
