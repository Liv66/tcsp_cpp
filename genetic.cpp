#include "genetic.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <random>
#include <tuple>
#include <unordered_map>
#include <vector>

using namespace std;

mt19937 random_gen(random_device{}());

namespace
{
int g_len;
int g_job_num;
int g_pop_size;
int g_generations;
double g_crossover_rate;
double g_mutation_rate;
int g_tournament_size;
int g_elite_count;
int g_flag;
int g_c2_pos;
int g_p;
unordered_map<int, int> g_job_assign;
unordered_map<int, int> g_org_map;
unordered_map<int, int> g_dest_map;
}  // namespace

template <typename T>
void print(const T& t)
{
    std::cout << t << '\n';
}

// 여러 개 출력
template <typename T, typename... Args>
void print(const T& t, const Args&... args)
{
    std::cout << t << ' ';
    print(args...);
}

void set_value(int len, int jn, const unordered_map<int, int>& assign, const unordered_map<int, int>& org,
               const unordered_map<int, int>& dest, int flag, int c2_pos, int p)
{
    g_len = len;
    g_job_num = jn;
    g_job_assign = assign;
    g_org_map = org;
    g_dest_map = dest;
    g_flag = flag;
    g_c2_pos = c2_pos;
    g_p = p;
}

void set_config(GAConfig config)
{
    g_pop_size = config.pop_size;
    g_generations = config.generations;
    g_crossover_rate = config.crossover_rate;
    g_mutation_rate = config.mutation_rate;
    g_tournament_size = config.tournament_size;
    g_elite_count = config.elite_count;
}

struct Chromosome
{
    vector<int> genes;
    int makespan;

    // ct, lt, et, pt, wt
    vector<int> crane1;
    vector<int> crane2;

    void save_result(int m, vector<int>&& c1, vector<int>&& c2)
    {
        this->makespan = m;
        this->crane1 = move(c1);
        this->crane2 = move(c2);
    }

    Chromosome(vector<int> g) : genes(move(g)) { this->repair(); }
    Chromosome() = default;

    void mutate(double mutation_rate)
    {
        uniform_real_distribution<> prob(0.0, 1.0);
        uniform_int_distribution<> index_dist(0, g_len - 2);  // j ≠ i 보장
        for (int i = 0; i < g_len; ++i)
        {
            if (prob(random_gen) < mutation_rate)
            {
                int j = index_dist(random_gen);
                if (j >= i)
                    ++j;
                swap(genes[i], genes[j]);
            }
        }
    }

    void bi_direction_fitness()
    {
        int idx1, idx2, o, d, pos, move, hist_len, other_len, sign = 0, c, count_p;
        // crane_idx는 gene의 idx 저장
        vector<vector<int>> crane_hist = {{}, {0}, {g_c2_pos}}, crane_jidx_hist = {{}, {}, {}},
                            crane_idx_to_jidx = {{}, {}, {}};
        vector<int> crane_jobs = {0, 0, 0}, crane_idx = {0, 0, 0}, crane_status = {0, 0, 0},
                    crane_finish = {0, 0, 0};             // 0 : empty, 1 : loading, 2 : travel, 3 : unloading
        unordered_map<int, int> job_to_time, job_to_idx;  // crane1 idx, crane2 idx 찾음

        for (int i = 0; i < this->genes.size(); i++)
        {
            if (g_job_assign[this->genes[i]] == 1)
            {
                crane_idx_to_jidx[1].push_back(i);
                job_to_idx[this->genes[i]] = crane_idx_to_jidx[1].size() - 1;
            }
            else
            {
                crane_idx_to_jidx[2].push_back(i);
                job_to_idx[this->genes[i]] = crane_idx_to_jidx[2].size() - 1;
            }

            job_to_time[this->genes[i]] = 0;
        }

        while (crane_finish[1] != 1 || crane_finish[2] != 1)
        {
            idx1 = crane_idx[1];
            idx2 = crane_idx[2];
            print("-------------", idx1, idx2);

            crane_status[1] = 0;
            crane_status[2] = 0;
            crane_jobs[1] = crane_finish[1] != 1 ? this->genes[crane_idx_to_jidx[1][idx1]]
                                                 : this->genes[crane_idx_to_jidx[1][idx1 - 1]];
            if ((crane_jobs[1] >= g_job_num && job_to_time[crane_jobs[1] - g_job_num] == 0) || crane_finish[1] == 1)
            {
                crane_status[1] = 1;
            }

            crane_jobs[2] = crane_finish[2] != 1 ? this->genes[crane_idx_to_jidx[2][idx2]]
                                                 : this->genes[crane_idx_to_jidx[2][idx2 - 1]];
            if ((crane_jobs[2] >= g_job_num && job_to_time[crane_jobs[2] - g_job_num] == 0) || crane_finish[2] == 1)
            {
                crane_status[2] = 1;
            }

            if (crane_status[1] + crane_status[2] == 2)
            {
                print("wait sum is 2");
                abort();
            }

            if (crane_hist[1].size() <= crane_hist[2].size() && crane_status[1] + crane_status[2] == 0)
                c = 1;
            else if (crane_hist[1].size() > crane_hist[2].size() && crane_status[1] + crane_status[2] == 0)
                c = 2;
            else if (crane_status[1] == 1)
                c = 2;
            else
                c = 1;

            hist_len = crane_hist[c].size() - 1;       // 지금까지 작업 시간
            other_len = crane_hist[3 - c].size() - 1;  // 상대방 작업 시간
            pos = crane_hist[c][hist_len];             // 현재 위치
            o = g_org_map[crane_jobs[c]];              //
            d = g_dest_map[crane_jobs[c]];
            crane_status[c] = (pos == o) ? 1 : 0;
            sign = 3 - 2 * c;
            count_p = 0;

            // c, idx, crane_jobs
            print(c, crane_idx[c], crane_jobs[c]);

            while (true)
            {
                // print(crane_status[c], count_p, pos, move, d);
                hist_len++;
                if (crane_status[c] == 0)
                {
                    move = pos < o ? 1 : -1;
                }
                else if (crane_status[c] == 2)
                {
                    move = pos < d ? 1 : -1;
                    // 충돌 체크, 목적지 도착 체크
                }
                else
                {
                    move = 0;
                }

                // 충돌 체크
                if (hist_len <= other_len && sign * (pos + move) >= sign * crane_hist[3 - c][hist_len])
                {
                    // 우선순위 체크
                    if (crane_jidx_hist[c][hist_len - 2] <=
                        crane_jidx_hist[3 - c][hist_len - 2])  // jidx_hist는 1칸 작음
                    {
                        // 정상 작동, 상대방 다 꺼냄
                        pos += move;
                        int tmp_jidx = crane_jidx_hist[3 - c][hist_len - 2];  // 간섭 발생한 jidx
                        int tmp_time = job_to_time[this->genes[tmp_jidx]];    // jidx의 끝난 시간
                        int tmp_c_idx = job_to_idx[this->genes[tmp_jidx]];    // jidx의 인덱스
                        int tmp_time2 = tmp_c_idx >= 1
                                            ? job_to_time[this->genes[crane_idx_to_jidx[3 - c][tmp_c_idx - 1]]]
                                            : 0;  // jidx 이전의 작업 끝난
                                                  // 시간
                        print("interference occur A", 3 - c, tmp_c_idx, this->genes[tmp_jidx],
                              crane_jidx_hist[c][hist_len - 2], crane_jidx_hist[3 - c][hist_len - 2]);
                        for (int i = tmp_time2 + 1; i <= other_len; i++)
                        {  // crane_hist 초기화
                            crane_hist[3 - c].pop_back();
                            crane_jidx_hist[3 - c].pop_back();
                        }
                        for (int i = tmp_c_idx; i <= crane_idx[3 - c]; i++)
                        {
                            job_to_time[this->genes[crane_idx_to_jidx[3 - c][i]]] = 0;  // 작업 완료시간 초기화
                        }
                        crane_idx[3 - c] = tmp_c_idx;
                        other_len = crane_hist[3 - c].size() - 1;
                    }
                    else
                    {
                        if (crane_status[c] == 0 || crane_status[c] == 2)
                        {
                            pos -= move;
                        }
                        else
                        {
                            count_p = 0;
                            pos -= sign;
                        }
                    }
                }
                else
                {
                    pos += move;
                }
                crane_jidx_hist[c].push_back(crane_idx_to_jidx[c][crane_idx[c]]);
                crane_hist[c].push_back(pos);
                if (pos == o && crane_status[c] == 0)
                {
                    crane_status[c]++;
                }
                else if (pos == d && crane_status[c] == 2)
                {
                    crane_status[c]++;
                }
                else if (crane_status[c] == 1 || crane_status[c] == 3)
                {
                    count_p++;
                    if (count_p == g_p && crane_status[c] == 3)
                    {
                        count_p = 0;
                        crane_idx[c]++;
                        job_to_time[crane_jobs[c]] = crane_hist[c].size() - 1;
                        if (crane_idx[c] == crane_idx_to_jidx[c].size())
                        {
                            crane_finish[c] = 1;
                        }
                        break;
                    }
                    else if (count_p == g_p && crane_status[c] == 1)
                    {
                        count_p = 0;
                        crane_status[c]++;
                    }
                }
            }
        }

        int min_len = min(crane_hist[1].size(), crane_hist[2].size());
        for (int i = 0; i < min_len; i++)
        {
            if (crane_hist[1][i] >= crane_hist[2][i])
            {
                print("Interference exists", i, crane_hist[1][i], crane_hist[2][i]);
                abort();
            }
        }

        vector<vector<int>> result = {{}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
        for (int i = 1; i <= 2; i++)
        {
            result[i][0] = crane_jidx_hist[i].size() - 1;
            int tmp_job = this->genes[crane_idx_to_jidx[i][0]], tmp_pre_job;
            result[i][1] += abs(g_org_map[tmp_job] - g_dest_map[tmp_job]);
            tmp_pre_job = tmp_job;
            for (int j = 1; j < crane_idx[i]; j++)
            {
                tmp_job = this->genes[crane_idx_to_jidx[i][j]];
                result[i][1] += abs(g_org_map[tmp_job] - g_dest_map[tmp_job]);
                result[i][2] += abs(g_org_map[tmp_job] - g_dest_map[tmp_pre_job]);
                tmp_pre_job = tmp_job;
            }
            result[i][3] = 2 * g_p * crane_idx[i];
            result[i][4] = result[i][0] - result[i][1] - result[i][2] - result[i][3];
        }
        result[2][2] += abs(g_c2_pos - g_org_map[this->genes[crane_idx_to_jidx[2][0]]]);
        result[2][4] -= abs(g_c2_pos - g_org_map[this->genes[crane_idx_to_jidx[2][0]]]);

        makespan = max(result[1][0], result[2][0]);

        cout << makespan << "  " << result[1][0] << "  " << result[2][0] << endl;
        print(result[1][0], result[1][1] + result[1][3], result[1][2], result[1][4]);
        print(result[2][0], result[2][1] + result[2][3], result[2][2], result[2][4]);
    }

    void repair()
    {
        unordered_map<int, int> value_to_index;
        for (int i = 0; i < this->genes.size(); ++i) value_to_index[this->genes[i]] = i;

        for (int i = 0; i < this->genes.size(); ++i)
        {
            int a = this->genes[i];
            if (a >= g_job_num)
            {
                int b = a - g_job_num;
                auto it = value_to_index.find(b);
                if (it != value_to_index.end())
                {
                    int j = it->second;
                    if (i < j)
                    {
                        swap(this->genes[i], this->genes[j]);

                        // update the index map
                        value_to_index[this->genes[i]] = i;
                        value_to_index[this->genes[j]] = j;
                    }
                }
            }
        }
    }

    bool operator<(const Chromosome& other) const { return this->makespan < other.makespan; }
};

Chromosome tournament_selection(const vector<Chromosome>& pop, int size)
{
    uniform_int_distribution<> dist(0, pop.size() - 1);
    Chromosome best = pop[dist(random_gen)];
    for (int i = 1; i < size; ++i)
    {
        const Chromosome& cand = pop[dist(random_gen)];
        if (cand < best)
            best = cand;
    }
    return best;
}

pair<Chromosome, Chromosome> pmx(const Chromosome& p1, const Chromosome& p2)
{
    uniform_int_distribution<> dist(0, g_len - 1);
    int start = dist(random_gen), end = dist(random_gen);
    if (start > end)
        swap(start, end);

    vector<int> child1(g_len, -1), child2(g_len, -1);

    // Step 1: copy the crossover segment
    for (int i = start; i <= end; ++i)
    {
        child1[i] = p2.genes[i];
        child2[i] = p1.genes[i];
    }

    // Step 2: create mapping
    unordered_map<int, int> map1to2, map2to1;
    for (int i = start; i <= end; ++i)
    {
        map1to2[p2.genes[i]] = p1.genes[i];
        map2to1[p1.genes[i]] = p2.genes[i];
    }

    auto fill_child = [&](vector<int>& child, const vector<int>& parent, unordered_map<int, int>& mapping)
    {
        for (int i = 0; i < g_len; ++i)
        {
            if (i >= start && i <= end)
                continue;

            int gene = parent[i];
            while (find(child.begin() + start, child.begin() + end + 1, gene) != child.begin() + end + 1)
            {
                gene = mapping[gene];  // recursively resolve conflict
            }
            child[i] = gene;
        }
    };

    fill_child(child1, p1.genes, map1to2);
    fill_child(child2, p2.genes, map2to1);

    return {Chromosome(child1), Chromosome(child2)};
}

vector<int> make_initial_seq(ProblemInfo info)
{
    vector<int> v;
    unordered_map<int, int> assign;
    int jn;
    if (info.flag)
    {
        jn = info.raw_dest.size();
        unordered_map<int, int> mo;
        unordered_map<int, int> md;
        for (int i = 0; i < jn; ++i)
        {
            int o = info.raw_org[i];
            int d = info.raw_dest[i];
            int h = info.h_list[i];
            int c = (o < h || (o == h && d < h)) ? 1 : 2;
            if ((o <= h && d <= h) || (o >= h && d >= h))
            {
                v.push_back(i);
                assign[i] = c;
                mo[i] = o;
                md[i] = d;
            }
            else
            {
                v.push_back(i);
                assign[i] = c;
                v.push_back(i + jn);
                assign[i + jn] = (3 - c);
                mo[i] = o;
                md[i] = h;
                mo[i + jn] = h;
                md[i + jn] = d;
            }
        }
        set_value(v.size(), jn, assign, mo, md, info.flag, info.c2_pos, info.p);
    }

    return v;
}

void print_vector(vector<int>& vec)
{
    // std::sort(vec.begin(), vec.end());  // 오름차순 정렬

    std::cout << "[";
    for (size_t i = 0; i < vec.size(); ++i)
    {
        std::cout << vec[i];
        if (i != vec.size() - 1)
        {
            std::cout << ", ";
        }
    }
    std::cout << "]\n";
}

vector<Chromosome> initialize_population(const vector<int>& base_seq, int pop_size)
{
    vector<Chromosome> population;

    for (int i = 0; i < pop_size; ++i)
    {
        vector<int> individual = base_seq;
        shuffle(individual.begin(), individual.end(), random_gen);  // 무작위 셔플
        population.emplace_back(individual);
        // print_vector(individual);
    }

    return population;
}

void run_genetic_algorithm(GAConfig config, ProblemInfo info)
{
    vector<int> sol = {0,  33, 24, 37, 28, 46, 14, 34, 23, 44, 9,  20, 25, 35, 15, 29, 4, 36,
                       32, 12, 22, 42, 3,  13, 18, 40, 6,  30, 17, 49, 47, 19, 2,  10, 5, 21,
                       26, 7,  11, 38, 73, 31, 48, 27, 1,  68, 8,  39, 16, 43, 41, 61, 45};
    Chromosome zebal(sol);

    uniform_real_distribution<> prob(0.0, 1.0);
    set_config(config);
    vector<int> base_job = make_initial_seq(info);
    zebal.bi_direction_fitness();
    // vector<Chromosome> population = initialize_population(base_job, g_pop_size);
    // print_vector(g_job_assign);
    // print_vector(population[0].genes);
    // cout << g_job_num;
    // population[0].bi_direction_fitness();
    // for (int gen_idx = 0; gen_idx < g_generations; ++gen_idx)
    // {
    //     vector<Chromosome> sorted = population;
    //     sort(sorted.begin(), sorted.end());

    //     vector<Chromosome> new_pop;
    //     for (int i = 0; i < g_elite_count; ++i) new_pop.push_back(sorted[i]);

    // while (new_pop.size() < g_pop_size)
    // {
    //     Chromosome p1 = tournament_selection(population, g_tournament_size);
    //     Chromosome p2 = tournament_selection(population, g_tournament_size);

    //     if (prob(random_gen) < g_crossover_rate)
    //     {
    //         auto [c1, c2] = pmx(p1, p2);
    //         c1.mutate(g_mutation_rate);
    //         c2.mutate(g_mutation_rate);
    //         new_pop.push_back(c1);
    //         if (new_pop.size() < g_pop_size)
    //             new_pop.push_back(c2);
    //     }
    //     else
    //     {
    //         p1.mutate(g_mutation_rate);
    //         p2.mutate(g_mutation_rate);
    //         new_pop.push_back(p1);
    //         if (new_pop.size() < g_pop_size)
    //             new_pop.push_back(p2);
    //     }
    // }

    // population = new_pop;

    // cout << "Gen " << gen_idx << " Best = " << sorted[0].makespan << "\n";
}
