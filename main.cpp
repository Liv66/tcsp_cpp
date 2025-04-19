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
    vector<int> raw_org = {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,  39, 39, 7,
                           33, 28, 35, 3,  33, 31, 15, 3,  41, 41, 41, 41, 41, 41, 41, 41, 41,
                           41, 41, 41, 41, 1,  1,  18, 38, 10, 30, 19, 7,  24, 21, 37, 12};
    vector<int> raw_dest = {37, 16, 8,  34, 37, 25, 38, 36, 15, 3,  9,  21, 27, 0,  0,  0,  0,
                            0,  0,  0,  0,  0,  0,  0,  0,  10, 15, 10, 12, 39, 23, 12, 25, 32,
                            33, 33, 18, 27, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41};
    vector<int> h_list = {31, 41, 41, 41, 41, 41, 33, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
                          41, 41, 20, 41, 41, 41, 41, 41, 0,  0,  0,  0,  39, 0,  0,  0,  24,
                          16, 16, 0,  0,  0,  0,  0,  30, 0,  16, 0,  0,  0,  0,  20, 0};
    int p = 3;

    ProblemInfo info(raw_org, raw_dest, h_list, p, 41);
    /*
        int pop_size;
        int generations;
        double crossover_rate;
        double mutation_rate;
        int tournament_size;
        int elite_count;
    */
    GAConfig config(250, 300, 0.6, 0.01, 6, 2);
    run_genetic_algorithm(config, info);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "\n[Execution Time] " << elapsed.count() << " seconds\n";
    return 0;
}
