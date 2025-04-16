// genetic.cpp
#include "genetic.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <cmath>
#include <numeric>

int NUM_CITIES;
std::vector<std::pair<double, double>> cities;
static std::mt19937 gen(std::random_device{}());

static double distance(const std::pair<double, double>& a, const std::pair<double, double>& b) {
    return std::hypot(a.first - b.first, a.second - b.second);
}

static double total_distance(const std::vector<int>& tour) {
    double dist = 0.0;
    for (int i = 0; i < NUM_CITIES; ++i)
        dist += distance(cities[tour[i]], cities[tour[(i + 1) % NUM_CITIES]]);
    return dist;
}

static std::vector<int> random_tour() {
    std::vector<int> tour(NUM_CITIES);
    std::iota(tour.begin(), tour.end(), 0);
    std::shuffle(tour.begin(), tour.end(), gen);
    return tour;
}

static std::vector<int> tournament_selection(const std::vector<std::vector<int>>& pop, int size) {
    std::uniform_int_distribution<> dist(0, pop.size() - 1);
    std::vector<int> best = pop[dist(gen)];
    for (int i = 1; i < size; ++i) {
        auto cand = pop[dist(gen)];
        if (total_distance(cand) < total_distance(best))
            best = cand;
    }
    return best;
}

static std::pair<std::vector<int>, std::vector<int>> pmx(const std::vector<int>& p1, const std::vector<int>& p2) {
    std::uniform_int_distribution<> dist(0, NUM_CITIES - 1);
    int start = dist(gen), end = dist(gen);
    if (start > end) std::swap(start, end);

    auto child1 = p1, child2 = p2;

    auto pmx_map = [](std::vector<int>& child, const std::vector<int>& parent, int s, int e) {
        for (int i = s; i <= e; ++i) child[i] = parent[i];
        for (int i = 0; i < NUM_CITIES; ++i) {
            if (i >= s && i <= e) continue;
            int gene = parent[i];
            while (std::find(child.begin() + s, child.begin() + e + 1, gene) != child.begin() + e + 1) {
                int idx = std::distance(parent.begin(), std::find(parent.begin(), parent.end(), gene));
                gene = parent[idx];
            }
            child[i] = gene;
        }
    };

    pmx_map(child1, p2, start, end);
    pmx_map(child2, p1, start, end);

    return {child1, child2};
}

static void shuffle_mutation(std::vector<int>& tour, double mutation_rate) {
    std::uniform_real_distribution<> d(0.0, 1.0);
    if (d(gen) < mutation_rate) {
        std::uniform_int_distribution<> dist(0, NUM_CITIES - 2);
        int start = dist(gen), end = dist(gen);
        if (start > end) std::swap(start, end);
        std::shuffle(tour.begin() + start, tour.begin() + end + 1, gen);
    }
}

void run_genetic_algorithm(int pop_size, int generations,
                           double crossover_rate, double mutation_rate,
                           int tournament_size, int elite_count) {
    std::uniform_real_distribution<> prob(0.0, 1.0);
    std::vector<std::vector<int>> population;

    for (int i = 0; i < pop_size; ++i)
        population.push_back(random_tour());

    for (int gen_idx = 0; gen_idx < generations; ++gen_idx) {
        std::vector<std::vector<int>> sorted = population;
        std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {
            return total_distance(a) < total_distance(b);
        });

        std::vector<std::vector<int>> new_pop;
        for (int i = 0; i < elite_count; ++i)
            new_pop.push_back(sorted[i]);

        while (new_pop.size() < pop_size) {
            auto p1 = tournament_selection(population, tournament_size);
            auto p2 = tournament_selection(population, tournament_size);

            if (prob(gen) < crossover_rate) {
                auto [c1, c2] = pmx(p1, p2);
                shuffle_mutation(c1, mutation_rate);
                shuffle_mutation(c2, mutation_rate);
                new_pop.push_back(c1);
                new_pop.push_back(c2);
            } else {
                shuffle_mutation(p1, mutation_rate);
                shuffle_mutation(p2, mutation_rate);
                new_pop.push_back(p1);
                new_pop.push_back(p2);
            }
        }

        population = new_pop;

        std::cout << "Gen " << gen_idx << " Best = " << total_distance(sorted[0]) << "\n";
    }
}
