from graph import *

def R(x: int, y: int):
    def found(n):
        start_time = datetime.now()
        result = True
        key_max = GraphsNum.Labeled(n) // 2
        stat1 = 0
        for key in range(key_max):
            g = Graph(n, key)
            m1 = g.BronKerboschAlgorithm2()
            g.Complement()
            m2 = g.BronKerboschAlgorithm2()
            stat1 += m1 + m2
            if (m1 < x and m2 < y) or (m1 < y and m2 < x):
                result = False
                break
        key += 1
        print(f'{n=:<5}{(datetime.now() - start_time).total_seconds():<15.3}'
              f'{str(str(key * 2) + " / " + str(key_max * 2)):<30}'
              f'{stat1:<20}')
        return result

    n = 2
    while not found(n):
        n += 1
    return n


start_time = datetime.now()

GraphsNum.Benchmark(8)
# print(f'{R(3, 3)=}')

print(f'Total time: {(datetime.now() - start_time).total_seconds()}')
