// assign_model.h
#pragma once
#include <algorithm>
#include <vector>

using namespace std;

struct Mip_result
{
    vector<int> h_list;
    double CT1;
    double CT2;
    double BT;

    Mip_result() : h_list(), CT1(0), CT2(0), BT(0) {}

    Mip_result(const vector<int>& h_list_, double CT1_, double CT2_)
        : h_list(h_list_), CT1(CT1_), CT2(CT2_), BT(max(CT1_, CT2_))
    {
    }
};

Mip_result run_mip(const vector<int>& raw_org, const vector<int>& raw_dest, const int p_, int log = 0);