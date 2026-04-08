#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

const int BUF_SIZE = 1 << 20;
char buf[BUF_SIZE], *p1 = buf, *p2 = buf;
inline char gc() {
    return p1 == p2 && (p2 = (p1 = buf) + fread(buf, 1, BUF_SIZE, stdin), p1 == p2) ? EOF : *p1++;
}
inline int read_int() {
    int x = 0, f = 1;
    char ch = gc();
    while (ch < '0' || ch > '9') {
        if (ch == '-') f = -1;
        if (ch == EOF) return -1;
        ch = gc();
    }
    while (ch >= '0' && ch <= '9') {
        x = x * 10 + ch - '0';
        ch = gc();
    }
    return x * f;
}
inline long long read_ll() {
    long long x = 0, f = 1;
    char ch = gc();
    while (ch < '0' || ch > '9') {
        if (ch == '-') f = -1;
        if (ch == EOF) return -1;
        ch = gc();
    }
    while (ch >= '0' && ch <= '9') {
        x = x * 10 + ch - '0';
        ch = gc();
    }
    return x * f;
}

char out_buf[BUF_SIZE];
int out_p = 0;
inline void flush() {
    fwrite(out_buf, 1, out_p, stdout);
    out_p = 0;
}
inline void pc(char ch) {
    if (out_p == BUF_SIZE) flush();
    out_buf[out_p++] = ch;
}
inline void print_ll(long long x) {
    if (x < 0) {
        pc('-');
        x = -x;
    }
    if (x == 0) {
        pc('0');
        return;
    }
    char stk[20];
    int top = 0;
    while (x) {
        stk[++top] = x % 10 + '0';
        x /= 10;
    }
    while (top) pc(stk[top--]);
}
inline void print_str(const char* s) {
    while (*s) pc(*s++);
}

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

const int MAX_NODES = 25000005;
Node tr[MAX_NODES];
int node_cnt = 0;

int new_node() {
    int p = ++node_cnt;
    tr[p].l = tr[p].r = tr[p].sum = 0;
    return p;
}

int clone_node(int p) {
    int q = new_node();
    tr[q] = tr[p];
    return q;
}

void update(int &p, int pre, int l, int r, int pos, int val) {
    p = clone_node(pre);
    tr[p].sum += val;
    if (l == r) return;
    int mid = l + (r - l) / 2;
    if (pos <= mid) {
        update(tr[p].l, tr[pre].l, l, mid, pos, val);
    } else {
        update(tr[p].r, tr[pre].r, mid + 1, r, pos, val);
    }
}

int query_sum(int p, int l, int r, int ql, int qr) {
    if (!p || ql > r || qr < l) return 0;
    if (ql <= l && r <= qr) return tr[p].sum;
    int mid = l + (r - l) / 2;
    return query_sum(tr[p].l, l, mid, ql, qr) + query_sum(tr[p].r, mid + 1, r, ql, qr);
}

int query_count(int p, int l, int r, int pos) {
    if (!p) return 0;
    if (l == r) return tr[p].sum;
    int mid = l + (r - l) / 2;
    if (pos <= mid) return query_count(tr[p].l, l, mid, pos);
    else return query_count(tr[p].r, mid + 1, r, pos);
}

int find_pred(int p, int l, int r, int pos) {
    if (!p || tr[p].sum == 0 || l >= pos) return -1;
    if (l == r) return l;
    int mid = l + (r - l) / 2;
    int res = -1;
    if (mid + 1 < pos) {
        res = find_pred(tr[p].r, mid + 1, r, pos);
    }
    if (res != -1) return res;
    return find_pred(tr[p].l, l, mid, pos);
}

int find_succ(int p, int l, int r, int pos) {
    if (!p || tr[p].sum == 0 || r <= pos) return -1;
    if (l == r) return l;
    int mid = l + (r - l) / 2;
    int res = -1;
    if (mid > pos) {
        res = find_succ(tr[p].l, l, mid, pos);
    }
    if (res != -1) return res;
    return find_succ(tr[p].r, mid + 1, r, pos);
}

int find_min(int p, int l, int r) {
    if (!p || tr[p].sum == 0) return -1;
    if (l == r) return l;
    int mid = l + (r - l) / 2;
    if (tr[tr[p].l].sum > 0) return find_min(tr[p].l, l, mid);
    return find_min(tr[p].r, mid + 1, r);
}

int root[100005];

int main() {
    int op;
    while ((op = read_int()) != -1) {
        Query q;
        q.op = op;
        if (op == 0 || op == 1 || op == 3) {
            q.a = read_ll();
            q.b = read_ll();
            vals.push_back(q.b);
        } else if (op == 2) {
            q.a = read_ll();
        } else if (op == 4) {
            q.a = read_ll();
            q.b = read_ll();
            q.c = read_ll();
            vals.push_back(q.b);
            vals.push_back(q.c);
        }
        queries.push_back(q);
    }
    
    sort(vals.begin(), vals.end());
    vals.erase(unique(vals.begin(), vals.end()), vals.end());
    
    int M = vals.size();
    if (M == 0) M = 1;
    
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
                print_str("true\n");
                it_a = q.a;
                it_val = q.b;
                valid = true;
            } else {
                print_str("false\n");
            }
        } else if (q.op == 4) {
            if (q.b > q.c) {
                print_str("0\n");
            } else {
                int l_pos = lower_bound(vals.begin(), vals.end(), q.b) - vals.begin() + 1;
                int r_pos = upper_bound(vals.begin(), vals.end(), q.c) - vals.begin();
                if (l_pos <= r_pos) {
                    print_ll(query_sum(root[q.a], 1, M, l_pos, r_pos));
                    pc('\n');
                } else {
                    print_str("0\n");
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
                print_ll(it_val);
                pc('\n');
            } else {
                print_str("-1\n");
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
                print_ll(it_val);
                pc('\n');
            } else {
                print_str("-1\n");
            }
        }
    }
    
    flush();
    return 0;
}
