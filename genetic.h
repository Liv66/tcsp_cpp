// genetic.h
#pragma once
#include <iostream>
#include <string>
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
                int c2_pos = 41)
        : raw_org(raw_org), raw_dest(raw_dest), h_list(h_list), p(p), c2_pos(c2_pos)
    {
        flag = h_list.empty() ? 0 : 1;
    }
};

struct ResultRow
{
    string title;

    int mip_bt, mip_ct1, mip_ct2;
    int makespan;
    int ct1, ct1_lt, ct1_et, ct1_pt, ct1_wt;
    int ct2, ct2_lt, ct2_et, ct2_pt, ct2_wt;
    int num_handover;
    double time;
    int type;  // 0 : 우리 방식, 1 : no handover, 2 : best handshake, 3 : handshake
    int handshake;
};

struct GAresult
{
    int makespan;
    int ct1, lt1, et1, pt1, wt1;
    int ct2, lt2, et2, pt2, wt2;
    int num_handover;
    double time;

    GAresult(int makespan, int ct1, int lt1, int et1, int pt1, int wt1, int ct2, int lt2, int et2, int pt2, int wt2,
             int num_handover, double time)
        : makespan(makespan),
          ct1(ct1),
          lt1(lt1),
          et1(et1),
          pt1(pt1),
          wt1(wt1),
          ct2(ct2),
          lt2(lt2),
          et2(et2),
          pt2(pt2),
          wt2(wt2),
          num_handover(num_handover),
          time(time)
    {
    }
};

GAresult run_genetic_algorithm(GAConfig config, ProblemInfo info);
template <typename T>
void print(const T& t)
{
    std::cout << t << '\n';
}

// 여러 개 출력 (재귀적 처리)
template <typename T, typename... Args>
void print(const T& t, const Args&... args)
{
    std::cout << t << ' ';
    print(args...);
}

void print_vector(const vector<int>& vec);
