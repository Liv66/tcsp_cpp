#include "genetic.h"
#include "a.h"
#include "assign_model.h"
#include <iostream>
#include <random>
#include <chrono>  // ⏱ 시간 측정용

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    NUM_CITIES = 20;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0.0, 100.0);

    // 도시 좌표 생성
    cities.resize(NUM_CITIES);
    for (auto& c : cities)
        c = {dist(gen), dist(gen)};


    run_mip();

    // GA 실행
    // run_genetic_algorithm(
    //     300,     // pop_size
    //     20,    // generations
    //     0.5,    // crossover_rate
    //     0.2,    // mutation_rate
    //     5,      // tournament_size
    //     2       // elite_count
    // );

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "\n[Execution Time] " << elapsed.count() << " seconds\n";
    foo();
    return 0;
}
