#include <chrono>  // ⏱ 시간 측정용
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "a.h"
#include "assign_model.h"
#include "genetic.h"
#include "matplotlibcpp.h"

using namespace std;
namespace plt = matplotlibcpp;
int main()
{
    auto start = std::chrono::high_resolution_clock::now();
    // NUM_CITIES = 20;
    // std::random_device rd;
    // std::mt19937 gen(rd());
    // std::uniform_real_distribution<> dist(0.0, 100.0);

    // // 도시 좌표 생성
    // cities.resize(NUM_CITIES);
    // for (auto& c : cities) c = {dist(gen), dist(gen)};

    // GA 실행
    vector<int> raw_org = {0,  0,  0,  0, 0,  0,  0,  0,  11, 3,  21, 25, 28, 28, 13, 4,  3,
                           12, 27, 14, 8, 27, 19, 26, 30, 31, 31, 31, 31, 31, 31, 31, 31, 21,
                           10, 9,  26, 8, 9,  19, 1,  27, 15, 24, 15, 8,  9,  30, 3,  11};
    vector<int> raw_dest = {30, 3,  21, 28, 27, 27, 26, 6,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                            0,  0,  0,  0,  0,  0,  0,  0,  17, 28, 21, 2,  26, 20, 2,  10, 31,
                            31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31};
    vector<int> h_list = {41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 1,  41, 41, 41, 41, 41,
                          41, 1,  41, 41, 41, 41, 2,  41, 13, 27, 21, 1,  9,  19, 1,  9,  14,
                          10, 7,  25, 34, 9,  19, 1,  25, 15, 22, 13, 31, 8,  30, 0,  2};
    int p = 3;

    ProblemInfo info(raw_org, raw_dest, h_list, p, 31);
    /*
        int pop_size;
        int generations;
        double crossover_rate;
        double mutation_rate;
        int tournament_size;
        int elite_count;
    */
    GAConfig config(10, 20, 0.5, 0.2, 5, 2);
    run_genetic_algorithm(config, info);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "\n[Execution Time] " << elapsed.count() << " seconds\n";
    return 0;
}
