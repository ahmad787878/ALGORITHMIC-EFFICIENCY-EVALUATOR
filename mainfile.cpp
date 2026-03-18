#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>
#include <cassert>

using namespace std;

long long invN(const vector<int>& a) {
    long long c = 0;
    int n = a.size();
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (a[i] > a[j]) c++;
        }
    }
    return c;
}

long long mg(vector<int>& a, vector<int>& t, int l, int m, int r) {
    int i = l, j = m + 1, k = l;
    long long c = 0;

    while (i <= m && j <= r) {
        if (a[i] <= a[j]) t[k++] = a[i++];
        else {
            t[k++] = a[j++];
            c += (m - i + 1);
        }
    }

    while (i <= m) t[k++] = a[i++];
    while (j <= r) t[k++] = a[j++];

    for (i = l; i <= r; ++i) a[i] = t[i];

    return c;
}

long long invR(vector<int>& a, vector<int>& t, int l, int r) {
    long long c = 0;
    if (l < r) {
        int m = l + (r - l) / 2;
        c += invR(a, t, l, m);
        c += invR(a, t, m + 1, r);
        c += mg(a, t, l, m, r);
    }
    return c;
}

long long invO(vector<int> a) {
    vector<int> t(a.size());
    return invR(a, t, 0, a.size() - 1);
}

vector<int> gen(int n, int lo = 1, int hi = 100000) {
    static mt19937 g(chrono::steady_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> d(lo, hi);
    
    vector<int> a(n);
    for (int i = 0; i < n; ++i) a[i] = d(g);
    return a;
}

void stress(int it, int mx) {
    cout << "--- INITIATING LOGIC STRESS TEST ---" << endl;
    for (int i = 1; i <= it; ++i) {
        int n = rand() % mx + 1;
        vector<int> tc = gen(n);

        long long e = invN(tc);
        long long ac = invO(tc);

        if (e != ac) {
            cout << "[FAILED] Discrepancy found on test " << i << "!" << endl;
            cout << "Expected: " << e << ", Actual: " << ac << endl;
            return;
        }
    }
    cout << "[SUCCESS] 100% Logic Match over " << it << " random adversarial tests.\n\n";
}

void bench() {
    cout << "--- EMPIRICAL TIME COMPLEXITY BENCHMARK ---" << endl;
    cout << left << setw(15) << "Array Size (N)" 
         << setw(20) << "O(n^2) Naive (ms)" 
         << setw(25) << "O(n log n) Opt (ms)" 
         << "Speedup" << endl;
    cout << "-----------------------------------------------------------------------" << endl;

    vector<int> sz = {1000, 5000, 10000, 20000, 50000, 100000};

    for (int n : sz) {
        vector<int> a = gen(n);

        auto s1 = chrono::high_resolution_clock::now();
        if (n <= 50000) invN(a);
        auto e1 = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> t1 = e1 - s1;

        auto s2 = chrono::high_resolution_clock::now();
        invO(a);
        auto e2 = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> t2 = e2 - s2;

        cout << left << setw(15) << n;
        
        if (n <= 50000)
            cout << setw(20) << fixed << setprecision(3) << t1.count();
        else
            cout << setw(20) << "Skipped (Too Slow)";
        
        cout << setw(25) << fixed << setprecision(3) << t2.count();

        if (n <= 50000) {
            double sp = t1.count() / t2.count();
            cout << fixed << setprecision(2) << sp << "x";
        } else {
            cout << "N/A";
        }
        cout << endl;
    }
    cout << endl;
}

int main() {
    stress(100, 100);
    bench();
    return 0;
}
