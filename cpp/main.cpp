#include <thread>

#include "graph.h"

bool SatisfiesRamseyTheorem(NumV n, NumV x, NumV y) {
    auto start_time = std::chrono::steady_clock::now();
    bool result = true;
    NumG key = 0, key_max = GraphsNum::Labeled(n) / 2;
    size_t stat1 = 0;
    Graph g(n);
    for (; key != key_max; ++key) {
        g.SetEdges(key);
        NumV m1 = g.BronKerboschAlgorithm();
        g.Complement();
        NumV m2 = g.BronKerboschAlgorithm();
        stat1 += m1 + m2;
        if ((m1 < x && m2 < y) || (m1 < y && m2 < x)) {
            result = false;
            ++key;
            break;
        }
    }
    auto end_time = std::chrono::steady_clock::now();
    if (PRINT_STATUS) {
        std::cout << std::left << std::setw(5) << n;
        std::cout << std::left << std::setw(15) << std::setprecision(3)
                  << std::chrono::duration<double>(end_time - start_time).count();
        // std::cout << std::left << std::setw(30) << std::to_string(key * 2) + " / " + std::to_string(key_max * 2);
        std::cout << std::left << std::setw(20) << stat1 << '\n';
    }
    return result;
}

NumV R(NumV x, NumV y) {
    NumV n = 2;
    std::vector<std::thread> threads(std::thread::hardware_concurrency());
    while (!SatisfiesRamseyTheorem(n, x, y)) {
        // threads
        ++n;
    }
    return n;
}

int main() {
    auto start_time = std::chrono::steady_clock::now();

    GraphsNum::Benchmark(8);

    auto end_time = std::chrono::steady_clock::now();
    std::cout << "Total time: " << std::chrono::duration<double>(end_time - start_time).count() << '\n';
}
