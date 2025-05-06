#include "a.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

vector<int> parse_csv_list(const string& s)
{
    vector<int> result;
    stringstream ss(s);
    string token;

    while (getline(ss, token, ','))
    {
        token.erase(remove_if(token.begin(), token.end(), ::isspace), token.end());
        if (!token.empty())
            result.push_back(stoi(token));
    }

    return result;
}

vector<JobInstance> read_job_instances(const string& filename)
{
    vector<JobInstance> instances;
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Failed to open job file\n";
        return instances;
    }

    string line;
    getline(file, line);  // skip header

    while (getline(file, line))
    {
        JobInstance instance;

        // 먼저 인덱스와 이름을 쉼표 기준으로 추출
        stringstream ss(line);
        string index_str, name;
        getline(ss, index_str, ',');  // 인덱스
        getline(ss, name, ',');       // 이름

        instance.index = stoi(index_str);
        instance.name = name;

        // 그 다음 큰따옴표로 전체 line을 분리해서 raw_org, raw_dest 추출
        size_t first_quote = line.find('"');
        size_t second_quote = line.find('"', first_quote + 1);
        size_t third_quote = line.find('"', second_quote + 1);
        size_t fourth_quote = line.find('"', third_quote + 1);

        if (first_quote == string::npos || second_quote == string::npos || third_quote == string::npos ||
            fourth_quote == string::npos)
        {
            cerr << "Format error in line: " << line << endl;
            continue;
        }

        string raw_org_str = line.substr(first_quote + 1, second_quote - first_quote - 1);
        string raw_dest_str = line.substr(third_quote + 1, fourth_quote - third_quote - 1);

        instance.raw_org = parse_csv_list(raw_org_str);
        instance.raw_dest = parse_csv_list(raw_dest_str);

        instances.push_back(instance);
    }

    file.close();
    return instances;
}
