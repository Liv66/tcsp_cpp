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
#include "matplotlibcpp.h"

using namespace std;
namespace plt = matplotlibcpp;
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

int main()
{
    auto start = chrono::high_resolution_clock::now();
    vector<int> p_list = {1, 5, 10, 25};
    auto instances = read_job_instances("job_instances.csv");
    GAConfig config(250, 300, 0.6, 0.01, 6, 2);

    int type = 0;
    for (const auto& job : instances)
    {
        auto start2 = chrono::high_resolution_clock::now();
        print("-------------------------");
        print(job.name);
        for (auto p : p_list)
        {
            string new_title = job.name + "_p_" + to_string(p);
            // vector<int> raw_org = job.raw_org;
            // vector<int> raw_dest = job.raw_dest;

            vector<int> raw_org = {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                                   0,  0,  0,  0,  0,  0,  0,  0,  30, 20, 25, 12, 19, 14, 13, 25, 12,
                                   31, 23, 33, 10, 32, 31, 31, 15, 30, 19, 25, 34, 18, 19, 26, 30, 33,
                                   17, 26, 16, 12, 34, 24, 23, 21, 11, 16, 30, 28, 15, 19, 29, 16, 10,
                                   24, 10, 25, 35, 17, 10, 30, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40};
            vector<int> raw_dest = {33, 8,  25, 25, 7,  37, 25, 19, 36, 15, 19, 32, 16, 13, 15, 10, 5,
                                    30, 33, 10, 13, 24, 8,  26, 38, 0,  0,  0,  0,  0,  0,  0,  0,  0,
                                    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  40,
                                    40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
                                    40, 40, 40, 40, 40, 40, 40, 30, 34, 20, 33, 20, 16, 30, 34, 35, 13};
            p = 1;

            Mip_result mip_result = run_mip(raw_org, raw_dest, p, 1);
            abort();
            ProblemInfo info(raw_org, raw_dest, mip_result.h_list, p, 41);

            for (int i = 0; i < 1; i++)
            {
                GAresult ga_result = run_genetic_algorithm(config, info);

                ResultRow row_result = {new_title,
                                        mip_result.BT,
                                        mip_result.CT1,
                                        mip_result.CT2,
                                        ga_result.makespan,
                                        ga_result.ct1,
                                        ga_result.lt1,
                                        ga_result.et1,
                                        ga_result.pt1,
                                        ga_result.wt1,
                                        ga_result.ct2,
                                        ga_result.lt2,
                                        ga_result.et2,
                                        ga_result.pt2,
                                        ga_result.wt2,
                                        mip_result.num_handover,
                                        ga_result.time,
                                        type,
                                        0};
                append_result_to_csv("results6.csv", row_result);
            }
        }
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start2;
        cout << setprecision(2) << elapsed.count() << " seconds\n";
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "\n[Execution Time] " << setprecision(2) << elapsed.count() << " seconds\n";
    return 0;
}
