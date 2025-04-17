// genetic.h
#pragma once
#include <utility>
#include <vector>


using namespace std;
struct GAConfig
{
    int pop_size;
    int generations;
    double crossover_rate;
    double mutation_rate;
    int tournament_size;
    int elite_count;

    // 생성자 정의 (헤더에 같이 작성)
    GAConfig(int p, int g, double cr, double mr, int t, int e)
        : pop_size(p), generations(g), crossover_rate(cr), mutation_rate(mr), tournament_size(t), elite_count(e)
    {
    }
};

struct ProblemInfo
{
    vector<int> raw_org;
    vector<int> raw_dest;
    vector<int> h_list;
    int p;
    int c2_pos;
    int flag;

    // 생성자
    ProblemInfo(const vector<int>& raw_org, const vector<int>& raw_dest, const vector<int>& h_list, int p = 0,
                int c2_pos = 40)
        : raw_org(raw_org), raw_dest(raw_dest), h_list(h_list), p(p), c2_pos(c2_pos)
    {
        flag = h_list.empty() ? 0 : 1;
    }
};

void run_genetic_algorithm(GAConfig config, ProblemInfo info);