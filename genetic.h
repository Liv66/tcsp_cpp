// genetic.h
#pragma once
#include <vector>
#include <utility>

extern int NUM_CITIES;
extern std::vector<std::pair<double, double>> cities;

void run_genetic_algorithm(int pop_size, int generations,
                           double crossover_rate, double mutation_rate,
                           int tournament_size, int elite_count);
