#ifndef LIBProber_Read_HPP
#define LIBProber_Read_HPP

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;
//! ==== Prober_Get_file ==== !//
class Prober_Read
{
public:
    string path_lab;
    vector<string> Path; // need to clear memory
    void GetFile(string path_)
    {
        stringstream ss(path_);
        string token;
        while (getline(ss, token, ';'))
        {
            Path.push_back(token);
        }
        // msg.prefix("path") << Path;
        //  msg.prefix("path_size") << Path.size();
    }
    void clear()
    {
        Path.clear();
    };
};

#endif