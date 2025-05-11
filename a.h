#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct JobInstance
{
    int index;
    std::string name;
    std::vector<int> raw_org;
    std::vector<int> raw_dest;
    int p;
    int best_h;
};

vector<int> parse_csv_list(const std::string& s);
vector<JobInstance> read_job_instances(const std::string& filename);