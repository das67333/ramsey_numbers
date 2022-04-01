#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

using NumV = uint64_t;
using NumG = uint64_t;    // it is a bottleneck for increasing n
using Bitset = uint64_t;  // for vertices only

constexpr bool PRINT_STATUS = true;
constexpr size_t N_MAX = 10;

static_assert(N_MAX < sizeof(Bitset) * 8);
static_assert(N_MAX * (N_MAX - 1) / 2 < sizeof(NumG) * 8);

class GraphsNum {
public:
    static NumG Labeled(NumV n);
    static void Benchmark(NumV n);

protected:
    static NumG BenchmarkHelper(NumV n, NumG key_start, NumG key_max, NumG task_size);
};

class Graph {
public:
    Graph(NumV n) : n_(n) {
        assert(n <= N_MAX);
    }
    void SetEdges(NumG key);
    void Complement();
    void Print() const;
    NumV BronKerboschAlgorithm() const;

protected:
    void BronKerboschHelper(NumV& result, Bitset r, Bitset p, Bitset x) const;

    NumV n_;
    Bitset e_[N_MAX];
};

NumG GraphsNum::Labeled(NumV n) {
    size_t shift = n * (n - 1) / 2;
    assert(shift <= 8 * sizeof(NumG));
    return NumG(1) << shift;
}

void GraphsNum::Benchmark(NumV n) {
    size_t thr_num = std::thread::hardware_concurrency();
    NumG task_size = 1, key_max = Labeled(n);
    std::vector<std::thread> threads(thr_num);
    auto dur = std::chrono::duration<double>::zero();
    while (dur.count() < 1) {
        task_size *= 2;
        auto start_time = std::chrono::steady_clock::now();
        for (size_t i = 0; i != thr_num; ++i) {
            NumG key_start = i * key_max / thr_num;
            threads[i] = std::thread([=] { BenchmarkHelper(n, key_start, key_max, task_size); });
        }
        for (auto& thr : threads) {
            thr.join();
        }
        dur = std::chrono::steady_clock::now() - start_time;
    }
    std::cout << static_cast<double>(task_size * thr_num) / dur.count() << ' ' << n << "-vertex graphs per second\n";
}

NumG GraphsNum::BenchmarkHelper(NumV n, NumG key_start, NumG key_max, NumG task_size) {
    NumG s = 0;
    Graph g(n);
    while (task_size) {
        g.SetEdges((key_start + task_size) % key_max);
        s += g.BronKerboschAlgorithm();
        --task_size;
    }
    return s;
}

void Graph::SetEdges(NumG key) {
    std::fill_n(e_, n_, 0);
    size_t i = 0, j = 1, k = 0;
    while (i + 1 != n_) {
        if (key & (NumG(1) << k)) {
            e_[i] |= Bitset(1) << j;
            e_[j] |= Bitset(1) << i;
        }
        ++k, ++j;
        if (j == n_) {
            ++i;
            j = i + 1;
        }
    }
}

void Graph::Complement() {
    for (size_t i = 0; i != n_; ++i) {
        e_[i] = ~(e_[i] | (Bitset(1) << i));
    }
}

void Graph::Print() const {
    for (size_t i = 0; i != n_; ++i) {
        for (size_t j = 0; j != n_; ++j) {
            std::cout << bool(e_[i] & (Bitset(1) << j)) << ' ';
        }
        std::cout << '\n';
    }
}

NumV Graph::BronKerboschAlgorithm() const {
    NumV result = 0;
    BronKerboschHelper(result, 0, (Bitset(1) << n_) - 1, 0);
    return result;
}

void Graph::BronKerboschHelper(NumV& result, Bitset r, Bitset p, Bitset x) const {
    Bitset px = p | x;
    if (!px) {
        size_t cnt = __builtin_popcountll(r);
        if (cnt > result) {
            result = cnt;
        }
        return;
    }

    // Bitset pp = p;
    Bitset pp = p & ~e_[__builtin_ctzll((px & (~px + 1)))];  // a bit faster

    // iterate over bits of pp
    while (pp) {
        Bitset v = pp & (~pp + 1);
        // get neighbors of v
        Bitset nv = e_[__builtin_ctzll(v)];
        pp ^= v;
        BronKerboschHelper(result, r | v, p & nv, x & nv);
        p &= ~v;
        x |= v;
    }
}
