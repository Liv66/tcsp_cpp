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
    vector<int> raw_org = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 13, 32, 26, 2, 20, 12, 31, 22, 41, 41, 41, 7, 14, 33};
    vector<int> raw_dest = {3, 10, 6, 7, 27, 5, 9, 9, 10, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 4, 27, 41, 41, 41};
    vector<int> h_list = {41, 41, 41, 41, 2, 41, 41, 41, 41, 41, 41, 41, 6, 11, 41, 41, 41, 6, 41, 24, 0, 0, 0, 0, 16};
    int p = 0;

    ProblemInfo info(raw_org, raw_dest, h_list, p);
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
