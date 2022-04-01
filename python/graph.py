import math
import functools
import random
from datetime import datetime, timedelta

# graphs are assumed to be undirected


class GraphsNum:
    def Labeled(n):
        return 2**math.comb(n, 2)

    @functools.lru_cache
    def LabeledConnected(n):
        def f1(k):
            return k * math.comb(n, k) * 2**math.comb(n - k, 2)
        return GraphsNum.Labeled(
            n) - sum(f1(k) * GraphsNum.LabeledConnected(k) for k in range(1, n)) // n

    def PrintInfo():
        print(f'{"n":5}{"Labeled":30}{"LabeledConnected":30}')
        for n in range(1, 20):
            print(f'{n:<5}{math.log2(GraphsNum.Labeled(n)):<30}'
                  f'{GraphsNum.LabeledConnected(n) / GraphsNum.Labeled(n):<30}')

    def Benchmark(n):
        def inner(n, key_start, key_max, task_size):
            s = 0
            key_max = GraphsNum.Labeled(n)
            for key in range(key_start, key_start + task_size):
                g = Graph(n, key % key_max)
                s += g.BronKerboschAlgorithm2()
            return s

        task_size = 1
        key_max = GraphsNum.Labeled(n)
        duration = timedelta(0)
        while duration.total_seconds() < 1:
            task_size *= 2
            start_time = datetime.now()
            inner(n, 0, key_max, task_size)
            duration = datetime.now() - start_time
        print(f'{key_max // (key_max // task_size) / duration.total_seconds()}'
              f' {n}-vertex graphs per second')


class Graph:
    def Random(n: int):
        random.seed(0)
        return Graph(n, random.randint(0, GraphsNum.Labeled(n) - 1))

    def Complement(self):
        self.edges = [not e for e in self.edges]
        for i in range(self.n):
            self.edges[i * self.n + i] = False
        self.edges = tuple(self.edges)

    def __init__(self, n: int, key: int):
        """ Key structure:\n
        zz k0 k1 k2\n
        zz zz k3 k4\n
        zz zz zz k5\n
        zz zz zz zz\n
        """
        self.n = n
        w = math.comb(n, 2)
        assert(0 <= key < 2**w)
        s = f'{key:0{w}b}'[::-1]
        self.edges = [False] * n**2
        i, j, k = 0, 1, 0
        while i + 1 != n:
            if s[k] == '1':
                self.edges[i * n + j] = True
                self.edges[j * n + i] = True
            k += 1
            j += 1
            if j == n:
                i, j = i + 1, i + 2
        self.edges = tuple(self.edges)

    def __str__(self):
        return '\n'.join(
            ' '.join(
                str(int(self.edges[i * self.n + j])) for j in range(self.n)
            ) for i in range(self.n)
        )

    def GetNeighbors(self, i: int):
        return {j for j in range(self.n) if self.edges[i * self.n + j]}

    def BronKerboschAlgorithm1(self):
        result = 0

        def f(r, p, x):
            nonlocal result
            if len(p) + len(x) == 0:
                if result < len(r):
                    result = len(r)
                return
            for v in set(p):
                nv = self.GetNeighbors(v)
                f(r | {v}, p & nv, x & nv)
                p -= {v}
                x |= {v}

        f(set(), set(range(self.n)), set())
        return result

    def BronKerboschAlgorithm2(self):
        result = 0

        def f(r: set, p: set, x: set):
            nonlocal result
            if len(p) + len(x) == 0:
                if result < len(r):
                    result = len(r)
                return
            pivot = (p | x).pop()
            for v in p - self.GetNeighbors(pivot):
                nv = self.GetNeighbors(v)
                f(r | {v}, p & nv, x & nv)
                p -= {v}
                x |= {v}

        f(set(), set(range(self.n)), set())
        return result
