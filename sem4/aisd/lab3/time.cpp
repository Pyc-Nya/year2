// main.cpp
#include "HashTable.hpp"
#include <chrono>
#include <fstream>
#include <random>
#include <string>

using namespace std;
using namespace std::chrono;

// Генерация случайного вектора ключей
vector<int> generate_keys(size_t n, int min = 0, int max = 10000) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(min, max);

    vector<int> keys;
    keys.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        keys.push_back(dis(gen));
    }
    return keys;
}

// Замер времени выполнения функции f()
template<typename Func>
long long measure(Func&& f) {
    auto start = high_resolution_clock::now();
    f();
    auto end = high_resolution_clock::now();
    return duration_cast<nanoseconds>(end - start).count();
}

void log_result(ofstream& out, long long time_ns, const string& op, size_t size) {
    out <<  time_ns << " "  << op << " " <<  size << "\n";
}

int main() {
    ofstream logfile("result.log");
    if (!logfile.is_open()) {
        cerr << "Не удалось открыть файл result.log\n";
        return 1;
    }

    for (size_t sz = 10; sz <= 1000; sz += 1) {
        for (int repeat = 0; repeat < 10; ++repeat) {
            // Генерация двух множеств случайных ключей
            auto keys1 = generate_keys(sz);
            auto keys2 = generate_keys(sz);

            HashTable<int> A(sz * 2, sz * 2);
            HashTable<int> B(sz * 2, sz * 2);

            for (const auto& k : keys1) A.insert(k);
            for (const auto& k : keys2) B.insert(k);

            // MERGE
            auto t_merge = measure([&]() {
                [[maybe_unused]] auto res = A.MERGE(B);
            });
            log_result(logfile, t_merge, "MERGE", sz);

            // CONCAT
            auto t_concat = measure([&]() {
                [[maybe_unused]] auto res = A.CONCAT(B);
            });
            log_result(logfile, t_concat, "CONCAT", sz);

            // ERASE: удалим из середины
            auto t_erase = measure([&]() {
                [[maybe_unused]] auto res = A.ERASE(sz / 4, sz / 2);
            });
            log_result(logfile, t_erase, "ERASE", sz);

            // unionSet
            auto t_union = measure([&]() {
                [[maybe_unused]] auto res = A.unionSet(B);
            });
            log_result(logfile, t_union, "UNION", sz);

            // intersection
            auto t_inter = measure([&]() {
                [[maybe_unused]] auto res = A.intersection(B);
            });
            log_result(logfile, t_inter, "INTERSECTION", sz);

            // difference
            auto t_diff = measure([&]() {
                [[maybe_unused]] auto res = A.difference(B);
            });
            log_result(logfile, t_diff, "DIFFERENCE", sz);

            // symDifference
            auto t_symdiff = measure([&]() {
                [[maybe_unused]] auto res = A.symDifference(B);
            });
            log_result(logfile, t_symdiff, "SYMMETRIC_DIFFERENCE", sz);
        }
    }

    logfile.close();
    return 0;
}
