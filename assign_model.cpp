#include "assign_model.h"

#include <cmath>
#include <iomanip>  // for std::setprecision
#include <iostream>
#include <map>
#include <numeric>
#include <tuple>

#include "gurobi_c++.h"

using namespace std;

Mip_result run_mip(const vector<int>& raw_org, const vector<int>& raw_dest, const int p_, const int B, int log)
{
    map<int, vector<int>> a, b;
    map<int, int> n, m;
    vector<int> h_list;
    vector<int> h_list_result;
    Mip_result result;

    for (int i = 0; i < raw_org.size(); ++i)
    {
        vector<int> tmp(B, 0);
        int org = raw_org[i], dest = raw_dest[i];
        if (org > dest)
        {
            m[i] = org;
            n[i] = dest;
            for (int j = dest; j < org; ++j) tmp[j] = 1;
            b[i] = tmp;
        }
        else
        {
            m[i] = dest;
            n[i] = org;
            for (int j = org; j < dest; ++j) tmp[j] = 1;
            a[i] = tmp;
        }
        h_list.push_back((org == 0 || dest == 0) ? B : 0);
    }

    try
    {
        GRBEnv env = GRBEnv(true);
        env.set(GRB_IntParam_OutputFlag, log);
        env.start();
        GRBModel model = GRBModel(env);

        // 변수 정의
        map<pair<int, int>, GRBVar> c;
        map<pair<int, int>, GRBVar> x;
        map<int, GRBVar> h;
        map<pair<int, int>, GRBVar> p;
        GRBVar z = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "z");

        int num_jobs = raw_org.size();

        for (int k : {1, 2})
        {
            for (int j = 0; j < B; ++j)
            {
                c[{k, j}] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_INTEGER, "c_" + to_string(k) + "_" + to_string(j));
            }
        }

        for (int i = 0; i < num_jobs; ++i)
        {
            h[i] = model.addVar(0.0, B, 0.0, GRB_INTEGER, "h_" + to_string(i));
            for (int j = 0; j < B; ++j)
            {
                x[{i, j}] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "x_" + to_string(i) + "_" + to_string(j));
            }
            for (int k : {1, 2})
            {
                p[{k, i}] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "p_" + to_string(k) + "_" + to_string(i));
            }
        }

        // 제약조건 추가
        for (int j = 0; j < B; ++j)
        {
            GRBLinExpr sum_pos = 0, sum_neg = 0;
            for (auto& [i, vec] : a) sum_pos += vec[j] * x[{i, j}];
            for (auto& [i, vec] : b) sum_neg += vec[j] * x[{i, j}];
            model.addConstr(sum_pos <= c[{1, j}]);
            model.addConstr(sum_neg <= c[{1, j}]);
        }

        for (int j = 0; j < B; ++j)
        {
            GRBLinExpr sum_pos = 0, sum_neg = 0;
            for (auto& [i, vec] : a) sum_pos += vec[j] * (1 - x[{i, j}]);
            for (auto& [i, vec] : b) sum_neg += vec[j] * (1 - x[{i, j}]);
            model.addConstr(sum_pos <= c[{2, j}]);
            model.addConstr(sum_neg <= c[{2, j}]);
        }

        for (int i = 0; i < num_jobs; ++i)
        {
            for (int j = 0; j < B - 1; ++j)
            {
                model.addConstr(x[{i, j}] >= x[{i, j + 1}]);
            }
            GRBLinExpr sum_x = 0;
            for (int j = 0; j < B; ++j) sum_x += x[{i, j}];
            model.addConstr(sum_x == h[i]);
            model.addConstr(h[i] + B * p[{2, i}] >= m[i]);
            model.addConstr(h[i] <= n[i] + B * p[{1, i}]);
        }

        for (auto& [i, vec] : a)
        {
            model.addConstr(x[{i, B - 1}] <= 1 - vec[B - 1]);
            model.addConstr(x[{i, 0}] >= vec[0]);
        }
        for (auto& [i, vec] : b)
        {
            model.addConstr(x[{i, B - 1}] <= 1 - vec[B - 1]);
            model.addConstr(x[{i, 0}] >= vec[0]);
        }

        for (int k : {1, 2})
        {
            GRBLinExpr c_sum = 0, p_sum = 0;
            for (int j = 0; j < B; ++j) c_sum += c[{k, j}];
            for (int i = 0; i < num_jobs; ++i) p_sum += p[{k, i}];
            model.addConstr(2 * c_sum + 2 * p_ * p_sum <= z);
        }

        for (int j = 1; j < B; j++)
        {
            model.addConstr(B * c[{1, j - 1}] >= c[{1, j}]);
            model.addConstr(c[{2, j - 1}] <= B * c[{2, j}]);
        }

        model.setObjective(GRBLinExpr(z), GRB_MINIMIZE);
        model.optimize();

        for (int i = 0; i < num_jobs; ++i)
        {
            h_list_result.push_back(static_cast<int>(round(h[i].get(GRB_DoubleAttr_X))));
        }
        vector<int> c1_pos_sum(B, 0), c1_neg_sum(B, 0);
        for (int j = 0; j < B; ++j)
        {
            for (const auto& [i, vec] : a)
                c1_pos_sum[j] += vec[j] * static_cast<int>(round(x[{i, j}].get(GRB_DoubleAttr_X)));
            for (const auto& [i, vec] : b)
                c1_neg_sum[j] += vec[j] * static_cast<int>(round(x[{i, j}].get(GRB_DoubleAttr_X)));
        }

        vector<int> c1_max(B);
        for (int j = 0; j < B; ++j) c1_max[j] = max(c1_pos_sum[j], c1_neg_sum[j]);

        double c1_p_sum = 0.0;
        for (int i = 0; i < num_jobs; ++i) c1_p_sum += 2 * p_ * p[{1, i}].get(GRB_DoubleAttr_X);

        double ct1 = 2 * accumulate(c1_max.begin(), c1_max.end(), 0) + c1_p_sum;
        vector<int> c2_pos_sum(B, 0), c2_neg_sum(B, 0);
        for (int j = 0; j < B; ++j)
        {
            for (const auto& [i, vec] : a)
                c2_pos_sum[j] += vec[j] * (1 - static_cast<int>(round(x[{i, j}].get(GRB_DoubleAttr_X))));
            for (const auto& [i, vec] : b)
                c2_neg_sum[j] += vec[j] * (1 - static_cast<int>(round(x[{i, j}].get(GRB_DoubleAttr_X))));
        }

        vector<int> c2_max(B);
        for (int j = 0; j < B; ++j) c2_max[j] = max(c2_pos_sum[j], c2_neg_sum[j]);

        double c2_p_sum = 0.0;
        for (int i = 0; i < num_jobs; ++i) c2_p_sum += 2 * p_ * p[{2, i}].get(GRB_DoubleAttr_X);

        double ct2 = 2 * accumulate(c2_max.begin(), c2_max.end(), 0) + c2_p_sum;
        if (log)
        {
            cout << "Objective Value: " << model.get(GRB_DoubleAttr_ObjVal) << endl;

            cout << ct1 << endl;
            cout << "c1-------------------" << endl;

            cout << ct2 << endl;
            cout << "c2-------------------" << endl;
            int split_count = 0;
            cout << "split num ----------------" << endl;
            for (int i = 0; i < num_jobs; ++i)
            {
                int p1 = static_cast<int>(round(p[{1, i}].get(GRB_DoubleAttr_X)));
                int p2 = static_cast<int>(round(p[{2, i}].get(GRB_DoubleAttr_X)));
                if (p1 == p2)
                {
                    double h_val = h[i].get(GRB_DoubleAttr_X);
                    cout << i << ", location : " << h_val << ", org : " << raw_org[i] << ", dest :" << raw_dest[i]
                         << endl;
                    split_count++;
                }
            }
            cout << split_count << endl;
            cout << "------------------" << endl;
        }
        int count = 0;
        for (int i = 0; i < num_jobs; ++i)
        {
            int val1 = static_cast<int>(round(p[{1, i}].get(GRB_DoubleAttr_X)));
            int val2 = static_cast<int>(round(p[{2, i}].get(GRB_DoubleAttr_X)));
            if (val1 == val2)
                count++;
        }
        Mip_result result(h_list_result, ct1, ct2, count);
        return result;
    }
    catch (GRBException e)
    {
        cerr << "Error code = " << e.getErrorCode() << endl;
        cerr << e.getMessage() << endl;
    }
    catch (...)
    {
        cerr << "Exception during optimization" << endl;
    }

    return result;
}
