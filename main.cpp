#include <chrono>  // ⏱ 시간 측정용
#include <iostream>
#include <random>
#include <vector>

#include "a.h"
#include "assign_model.h"
#include "genetic.h"

using namespace std;

int main()
{
    auto start = std::chrono::high_resolution_clock::now();
    NUM_CITIES = 20;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0.0, 100.0);

    // 도시 좌표 생성
    cities.resize(NUM_CITIES);
    for (auto& c : cities) c = {dist(gen), dist(gen)};

    const int p_ = 5;

    vector<int> raw_org = {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                           0,  0,  0,  30, 20, 25, 12, 19, 14, 13, 25, 12, 31, 23, 33, 10, 32, 31, 31, 15, 30, 19,
                           25, 34, 18, 19, 26, 30, 33, 17, 26, 16, 12, 34, 24, 23, 21, 11, 16, 30, 28, 15, 19, 29,
                           16, 10, 24, 10, 25, 35, 17, 10, 30, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40};
    // 위 raw_org
    vector<int> raw_dest = {33, 8,  25, 25, 7,  37, 25, 19, 36, 15, 19, 32, 16, 13, 15, 10, 5,  30, 33, 10, 13, 24,
                            8,  26, 38, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                            0,  0,  0,  0,  0,  0,  40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
                            40, 40, 40, 40, 40, 40, 40, 40, 40, 30, 34, 20, 33, 20, 16, 30, 34, 35, 13};

    Mip_result mip_result = run_mip(raw_org, raw_dest, p_, 0);
    // run_mip(raw_org, raw_dest, p_, 0);

    // GA 실행
    // run_genetic_algorithm(
    //     300,     // pop_size
    //     20,    // generations
    //     0.5,    // crossover_rate
    //     0.2,    // mutation_rate
    //     5,      // tournament_size
    //     2       // elite_count
    // );

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "\n[Execution Time] " << elapsed.count() << " seconds\n";
    foo();
    return 0;
}
