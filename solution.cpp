#include <iostream>
#include <vector>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

using namespace std;
using namespace __gnu_pbds;

typedef tree<long long, null_type, less<long long>, rb_tree_tag, tree_order_statistics_node_update> ordered_set;

vector<ordered_set> s;

void ensure_size(int idx) {
    if (idx >= s.size()) {
        s.resize(idx + 1);
    }
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
            ensure_size(a);
            auto res = s[a].insert(b);
            if (res.second) {
                it_a = a;
                it_val = b;
                valid = true;
            }
        } else if (op == 1) {
            cin >> a >> b;
            ensure_size(a);
            if (valid && it_a == a && it_val == b) {
                valid = false;
            }
            s[a].erase(b);
        } else if (op == 2) {
            cin >> a;
            ensure_size(a);
            ++lst;
            ensure_size(lst);
            s[lst] = s[a];
        } else if (op == 3) {
            cin >> a >> b;
            ensure_size(a);
            auto it = s[a].find(b);
            if (it != s[a].end()) {
                cout << "true\n";
                it_a = a;
                it_val = b;
                valid = true;
            } else {
                cout << "false\n";
            }
        } else if (op == 4) {
            cin >> a >> b >> c;
            ensure_size(a);
            if (b > c) {
                cout << "0\n";
            } else {
                int cnt = s[a].order_of_key(c + 1) - s[a].order_of_key(b);
                cout << cnt << "\n";
            }
        } else if (op == 5) {
            if (valid) {
                ensure_size(it_a);
                auto it = s[it_a].find(it_val);
                if (it == s[it_a].begin()) {
                    valid = false;
                } else {
                    --it;
                    it_val = *it;
                }
            }
            if (valid) {
                cout << it_val << "\n";
            } else {
                cout << "-1\n";
            }
        } else if (op == 6) {
            if (valid) {
                ensure_size(it_a);
                auto it = s[it_a].find(it_val);
                auto it2 = it;
                ++it2;
                if (it2 == s[it_a].end()) {
                    valid = false;
                } else {
                    it_val = *it2;
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
