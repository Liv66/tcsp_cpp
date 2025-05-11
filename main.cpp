#include <omp.h>

#include <array>
#include <chrono>  // ⏱ 시간 측정용
#include <filesystem>
#include <fstream>
#include <iomanip>  // 여기에 fixed, setprecision 포함
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "a.h"
#include "assign_model.h"
#include "genetic.h"

// #include "matplotlibcpp.h"

using namespace std;

void append_result_to_csv(const string& filename, const ResultRow& row)
{
    bool file_exists = filesystem::exists(filename);

    ofstream file(filename, ios::app);
    if (!file.is_open())
    {
        cerr << "Failed to open file: " << filename << endl;
        return;
    }

    // 파일이 없어서 새로 생성한 경우 헤더 작성
    if (!file_exists)
    {
        file << "title,"
             << "mip_bt,mip_ct1,mip_ct2,"
             << "makespan,"
             << "ct1,ct1_lt,ct1_et,ct1_pt,ct1_wt,"
             << "ct2,ct2_lt,ct2_et,ct2_pt,ct2_wt,"
             << "num_handover,time,type,handshake\n";
    }

    // 데이터 한 줄 추가
    file << row.title << "," << row.mip_bt << "," << row.mip_ct1 << "," << row.mip_ct2 << "," << row.makespan << ","
         << row.ct1 << "," << row.ct1_lt << "," << row.ct1_et << "," << row.ct1_pt << "," << row.ct1_wt << ","
         << row.ct2 << "," << row.ct2_lt << "," << row.ct2_et << "," << row.ct2_pt << "," << row.ct2_wt << ","
         << row.num_handover << "," << fixed << setprecision(4) << row.time << "," << row.type << "," << row.handshake
         << "\n";

    file.close();
}

int run(const string& new_title, const GAConfig& config, const Mip_result& mip_result, const vector<int>& raw_org,
        const vector<int>& raw_dest, const vector<int>& h_list, int p, int B, int type, int iter, int h_location = 0)
{
    ProblemInfo info(raw_org, raw_dest, h_list, p, B);

    int acc = 0;

    for (int i = 0; i < iter; i++)
    {
        GAresult ga_result = run_genetic_algorithm(config, info);
        ResultRow row_result = {new_title,          mip_result.BT, mip_result.CT1, mip_result.CT2,
                                ga_result.makespan, ga_result.ct1, ga_result.lt1,  ga_result.et1,
                                ga_result.pt1,      ga_result.wt1, ga_result.ct2,  ga_result.lt2,
                                ga_result.et2,      ga_result.pt2, ga_result.wt2,  ga_result.num_handover,
                                ga_result.time,     type,          h_location};

        print(ga_result.makespan);

        // append_result_to_csv("results3.csv", row_result);
        acc += ga_result.makespan;
    }
    abort();
    return acc / iter;
}

int main()
{
    omp_set_num_threads(12);
    auto start = chrono::high_resolution_clock::now();
    vector<int> p_list = {1, 5, 10, 20};

    auto instances = read_job_instances("job_instances3.csv");
    GAConfig config(250, 300, 0.6, 0.05, 4, 1);
    int B = 41;
    int iter = 50;
    int p = 0;
    int type, cnt = 0;
    auto start2 = chrono::high_resolution_clock::now();
    for (const auto& job : instances)
    {
        if (cnt % 20 == 0)
        {
            start2 = chrono::high_resolution_clock::now();
            print("-------------------------");
            print(job.name);
        }
        // p = job.p;
        p = 1;

        string new_title = job.name + "_p_" + to_string(p);
        vector<int> raw_org = {0, 0,  14, 19, 31, 0, 38, 0,  15, 23, 1,  0,  25, 0,  41, 0,  0,
                               8, 0,  36, 41, 9,  0, 37, 41, 31, 0,  29, 30, 0,  0,  0,  41, 26,
                               0, 41, 41, 12, 30, 6, 19, 18, 30, 0,  20, 0,  0,  13, 0,  31};
        vector<int> raw_dest = {20, 24, 0,  0,  0, 21, 41, 11, 0,  0,  0,  21, 41, 5,  31, 31, 9,
                                0,  22, 0,  26, 0, 38, 0,  18, 41, 2,  0,  0,  34, 14, 22, 28, 0,
                                27, 6,  18, 0,  0, 0,  0,  41, 0,  17, 41, 15, 37, 0,  2,  41};

        Mip_result mip_result = run_mip(raw_org, raw_dest, p, B);
        print("DSAAD");
        vector<int> h_list = {41, 41, 41, 41, 41, 41, 30, 41, 41, 41, 37, 41, 0, 41, 16, 13, 41,
                              41, 41, 3,  0,  41, 1,  1,  0,  16, 41, 21, 2,  2, 41, 41, 16, 41,
                              1,  0,  0,  41, 21, 41, 41, 0,  1,  41, 0,  41, 3, 41, 41, 16};
        type = 0;
        run(new_title, config, mip_result, raw_org, raw_dest, h_list, p, B, type, iter);
        type = 1;
        // vector<int> h_list;
        for (size_t i = 0; i < raw_org.size(); ++i)
        {
            if (raw_org[i] == 0 || raw_dest[i] == 0)
                h_list.push_back(B);
            else
                h_list.push_back(0);
        }
        run(new_title, config, mip_result, raw_org, raw_dest, h_list, p, B, type, iter);

        int best_h = job.best_h;
        if (best_h == 0)
        {
            type = 3;
            int best_v = 99999, tmp;
            for (int i = 0; i < 20; i++)
            {
                h_list = vector(raw_dest.size(), i + 7);
                tmp = run(new_title, config, mip_result, raw_org, raw_dest, h_list, p, B, type, 20, i + 7);
                if (tmp < best_v)
                {
                    best_v = tmp;
                    best_h = i + 7;
                }
            }
        }

        type = 2;
        h_list = vector(raw_dest.size(), best_h);
        run(new_title, config, mip_result, raw_org, raw_dest, h_list, p, B, type, iter, best_h);
        if (cnt % 20 == 0)
        {
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> elapsed = end - start2;
            std::cout << setprecision(2) << elapsed.count() << " seconds\n";
        }
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    std::cout << "\n[Execution Time] " << setprecision(2) << elapsed.count() << " seconds\n";
    return 0;
}
