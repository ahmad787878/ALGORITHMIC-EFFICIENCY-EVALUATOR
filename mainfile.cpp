/**
 * ALGORITHMIC EFFICIENCY EVALUATOR
 * --------------------------------
 * Author: Ahmad Abdullah Shafiq
 * Description: An automated stress-testing and benchmarking framework.
 * Demonstrates the empirical difference between O(n^2) and O(n log n) 
 * time complexities using high-resolution timing, while validating logic 
 * via adversarial random test cases.
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>
#include <cassert>

using namespace std;

// ==========================================
// 1. ALGORITHMS
// Problem: Count the number of inversions in an array.
// An inversion is a pair (i, j) such that i < j and arr[i] > arr[j].
// ==========================================

// Baseline: O(n^2) Brute Force Approach
// Simple, but scales poorly.
long long countInversionsNaive(const vector<int>& arr) {
    long long inversions = 0;
    int n = arr.size();
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (arr[i] > arr[j]) {
                inversions++;
            }
        }
    }
    return inversions;
}

// Optimized helper: O(n log n) Divide & Conquer
long long mergeAndCount(vector<int>& arr, vector<int>& temp, int left, int mid, int right) {
    int i = left;    
    int j = mid + 1; 
    int k = left;    
    long long inversions = 0;

    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
            // If arr[i] > arr[j], then all remaining elements in the left 
            // subarray (from i to mid) is also greater than arr[j].
            inversions += (mid - i + 1);
        }
    }

    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];

    for (i = left; i <= right; ++i) {
        arr[i] = temp[i];
    }

    return inversions;
}

// Optimized main: O(n log n)
long long countInversionsOptimizedRecursive(vector<int>& arr, vector<int>& temp, int left, int right) {
    long long inversions = 0;
    if (left < right) {
        int mid = left + (right - left) / 2;
        inversions += countInversionsOptimizedRecursive(arr, temp, left, mid);
        inversions += countInversionsOptimizedRecursive(arr, temp, mid + 1, right);
        inversions += mergeAndCount(arr, temp, left, mid, right);
    }
    return inversions;
}

// Wrapper for Optimized approach. 
// NOTE ON MEMORY MANAGEMENT: We allocate 'temp' ONCE here and pass by reference.
// This prevents expensive O(n) heap allocations during every recursive step,
// showcasing hardware sympathy and low-level control.
long long countInversionsOptimized(vector<int> arr) {
    vector<int> temp(arr.size());
    return countInversionsOptimizedRecursive(arr, temp, 0, arr.size() - 1);
}


// ==========================================
// 2. ADVERSARIAL GENERATOR
// ==========================================

// Generates a random array of size N
vector<int> generateRandomArray(int size, int min_val = 1, int max_val = 100000) {
    // mt19937 is a high-quality Mersenne Twister RNG (Standard in CP)
    static mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    uniform_int_distribution<int> dist(min_val, max_val);
    
    vector<int> arr(size);
    for (int i = 0; i < size; ++i) {
        arr[i] = dist(rng);
    }
    return arr;
}


// ==========================================
// 3. TESTING & BENCHMARKING FRAMEWORKS
// ==========================================

void runStressTest(int iterations, int max_size) {
    cout << "--- INITIATING LOGIC STRESS TEST ---" << endl;
    for (int i = 1; i <= iterations; ++i) {
        // Generate small random sizes for logic validation
        int size = rand() % max_size + 1; 
        vector<int> test_case = generateRandomArray(size);

        long long expected = countInversionsNaive(test_case);
        long long actual = countInversionsOptimized(test_case);

        if (expected != actual) {
            cout << "[FAILED] Discrepancy found on test " << i << "!" << endl;
            cout << "Expected: " << expected << ", Actual: " << actual << endl;
            return; // Halt on failure
        }
    }
    cout << "[SUCCESS] 100% Logic Match over " << iterations << " random adversarial tests.\n\n";
}

void runBenchmark() {
    cout << "--- EMPIRICAL TIME COMPLEXITY BENCHMARK ---" << endl;
    cout << left << setw(15) << "Array Size (N)" 
         << setw(20) << "O(n^2) Naive (ms)" 
         << setw(25) << "O(n log n) Opt (ms)" 
         << "Speedup" << endl;
    cout << "-----------------------------------------------------------------------" << endl;

    // Test across exponentially increasing data sizes
    vector<int> sizes = {1000, 5000, 10000, 20000, 50000, 100000};

    for (int n : sizes) {
        vector<int> arr = generateRandomArray(n);

        // Benchmark O(n^2)
        auto start_naive = chrono::high_resolution_clock::now();
        // Skip naive for very large N to prevent the benchmark from taking forever
        if (n <= 50000) countInversionsNaive(arr); 
        auto end_naive = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> time_naive = end_naive - start_naive;

        // Benchmark O(n log n)
        auto start_opt = chrono::high_resolution_clock::now();
        countInversionsOptimized(arr);
        auto end_opt = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> time_opt = end_opt - start_opt;

        // Print formatted results
        cout << left << setw(15) << n;
        
        if (n <= 50000) {
            cout << setw(20) << fixed << setprecision(3) << time_naive.count();
        } else {
            cout << setw(20) << "Skipped (Too Slow)";
        }
        
        cout << setw(25) << fixed << setprecision(3) << time_opt.count();

        if (n <= 50000) {
            double speedup = time_naive.count() / time_opt.count();
            cout << fixed << setprecision(2) << speedup << "x";
        } else {
            cout << "N/A";
        }
        cout << endl;
    }
    cout << endl;
}

// ==========================================
// MAIN EXECUTION
// ==========================================
int main() {
    // 1. First, prove correctness.
    // Run 100 tests with arrays up to size 100.
    runStressTest(100, 100);

    // 2. Second, prove efficiency.
    runBenchmark();

    return 0;
}
