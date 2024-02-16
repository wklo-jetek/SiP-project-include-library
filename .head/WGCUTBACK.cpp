#ifndef WGCUTBACK_HPP
#define WGCUTBACK_HPP

#include "Prober_Read.cpp"
#include "libCustomizeItem.cpp"
#include "libSpectrumAnalysis.h"
#include "libTsmcFunction.cpp"
#include <format.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

//! ==== WG_Cut_Back ==== !//
class WGCUTBACK
{
    int wave_size;
    int subdie_size;

    struct WG_UNIT
    {
        double wavelength;
        vector<double> il_pw;
        vector<double> S_il_pw; // smooth
    };
    vector<WG_UNIT> db;    // need to clear memory
    vector<double> X;      // need to clear memory
    vector<double> slope_; // need to clear memory
    vector<double> RSQ;    // need to clear memory
    vector<string> subdie_;
    vector<string> cond_;
    vector<double> slope_mean;
    vector<double> RSQ_mean;

public:
    int log_num;

    SPECTRUM spctm_; // for readfile test
    LV_SPECTRUM LV_obj;
    Prober_Read Rd;
    FORMAT::WGCutBack::WG_Wavelength set1;
    FORMAT::WGCutBack::WG_Analysis set2;
    FORMAT::WGCutBack::WG_Subdie set3;
    FORMAT::WGCutBack::WG_Multi_Subdie set4;

    void FindPath_loss(vector<string> Path)
    {
        // msg.prefix("path") << Path;
        LV_obj.clear();
        for (int i = 0; i < (int)Path.size(); i++)
        {
            LV_obj.CreatPath(Path[i]);
        }
        LV_obj.Getsubdie();
        for (int i = 0; i < subdie_size; i++)
        {
            for (int j = 0; j < LV_obj.GetREFsize(); j++)
            {
                // if (subdie_[i] == LV_obj.subdie[j]) {
                if (LV_obj.subdie[j].find(subdie_[i]) != string::npos)
                {
                    // msg.prefix("Path[j].find(subdie_[i])") << Path[j].find(subdie_[i]);
                    //  msg.prefix("subdie") << subdie_[i];
                    msg.prefix("LV_obj.subdie:") << LV_obj.subdie[j];
                    LV_Raw_loss(j);
                    LV_Smooth_loss(j, set1.MV_point);
                }
                else
                {
                }
            }
        }
    };
    void Get_Wavelength()
    {
        wave_size = 0;
        WG_UNIT db_temp;
        if (set1.wavelength_01 != -1)
        {
            db_temp.wavelength = set1.wavelength_01;
            db.push_back(db_temp);
        }

        if (set1.wavelength_02 != -1)
        {
            db_temp.wavelength = set1.wavelength_02;
            db.push_back(db_temp);
        }

        if (set1.wavelength_03 != -1)
        {
            db_temp.wavelength = set1.wavelength_03;
            db.push_back(db_temp);
        }

        if (set1.wavelength_04 != -1)
        {
            db_temp.wavelength = set1.wavelength_04;
            db.push_back(db_temp);
        }

        if (set1.wavelength_05 != -1)
        {
            db_temp.wavelength = set1.wavelength_05;
            db.push_back(db_temp);
        }
        wave_size = db.size();
        // msg.prefix("wave");
    };
    void Get_Subdie()
    {
        subdie_size = 0;
        if (strcmp(set3.dev_name1, (char *)"NAME") != 0)
        {
            subdie_.push_back(set3.dev_name1);
        }
        if (strcmp(set3.dev_name2, (char *)"NAME") != 0)
        {
            subdie_.push_back(set3.dev_name2);
        }
        if (strcmp(set3.dev_name3, (char *)"NAME") != 0)
        {
            subdie_.push_back(set3.dev_name3);
        }
        if (strcmp(set3.dev_name4, (char *)"NAME") != 0)
        {
            subdie_.push_back(set3.dev_name4);
        }
        if (strcmp(set3.dev_name5, (char *)"NAME") != 0)
        {
            subdie_.push_back(set3.dev_name5);
        }
        subdie_size = subdie_.size();
        // msg.prefix("subdie") << subdie_;
        // msg.prefix("subdie_size") << subdie_size;
    };
    void WG_Multi_Subdie()
    {
        subdie_size = 0;
        string subdie_comb;
        if (strcmp(set4.dev_name1, (char *)"NAME") != 0 && strcmp(set4.cond1, (char *)"NAME") != 0)
        {
            subdie_comb = std::string(set4.dev_name1) + std::string("_") + std::string(set4.cond1);
            subdie_.push_back(subdie_comb);
        }
        if (strcmp(set4.dev_name2, (char *)"NAME") != 0 && strcmp(set4.cond2, (char *)"NAME") != 0)
        {
            subdie_comb = std::string(set4.dev_name2) + std::string("_") + std::string(set4.cond2);
            subdie_.push_back(subdie_comb);
        }
        if (strcmp(set4.dev_name3, (char *)"NAME") != 0 && strcmp(set4.cond3, (char *)"NAME") != 0)
        {
            subdie_comb = std::string(set4.dev_name3) + std::string("_") + std::string(set4.cond3);
            subdie_.push_back(subdie_comb);
        }
        if (strcmp(set4.dev_name4, (char *)"NAME") != 0 && strcmp(set4.cond4, (char *)"NAME") != 0)
        {
            subdie_comb = std::string(set4.dev_name4) + std::string("_") + std::string(set4.cond4);
            subdie_.push_back(subdie_comb);
        }
        if (strcmp(set4.dev_name5, (char *)"NAME") != 0 && strcmp(set4.cond5, (char *)"NAME") != 0)
        {
            subdie_comb = std::string(set4.dev_name5) + std::string("_") + std::string(set4.cond5);
            subdie_.push_back(subdie_comb);
        }
        subdie_size = subdie_.size();
        msg.prefix("subdie:") << subdie_;
        msg.prefix("subdie_size:") << subdie_size;
    }
    void Get_loss(SPECTRUM spctmtemp)
    {
        if (db.size() == 0)
            return;
        for (int i = 0; i < wave_size; i++)
        {
            db[i].il_pw.push_back(spctmtemp.idxWaveLength(db[i].wavelength * (1E-9)));
            // for (double value : db[i].il_pw) {
            //     cout << " loss" << value << endl;
            // }
        };
    };
    void LV_Raw_loss(int index)
    {
        if (db.size() == 0)
            return;

        LV_obj.modeRaw();
        for (int i = 0; i < wave_size; i++)
        {
            // db[i].il_pw.push_back(spctmtemp.idxWaveLength(db[i].wavelength * (1E-9)));
            db[i].il_pw.push_back(LV_obj.findWavelength(index, db[i].wavelength));
            for (double value : db[i].il_pw)
            {
                // cout << " loss" << value << endl;
            }
        };
    };
    void LV_Smooth_loss(int index, int MV)
    {
        if (db.size() == 0 || set1.MV_point <= 0)
            return;

        LV_obj.modeMovingAverage(MV);
        for (int i = 0; i < wave_size; i++)
        {
            db[i].S_il_pw.push_back(LV_obj.findWavelength(index, db[i].wavelength));
            // for (double value : db[i].S_il_pw) {
            //     cout << " loss" << value << endl;
            //}
        };
    };
    vector<double> cal_Predictions(const vector<double> &x_values, double slope, double intercept)
    {
        // 计算线性回归预测值
        vector<double> predictions;
        for (double value : x_values)
        {
            double predictedY = slope * value + intercept;
            predictions.push_back(predictedY);
        }
        return predictions;
    }
    double cal_Error(const vector<double> &actual, const vector<double> &predicted)
    {
        // 计算误差（残差）
        double sum_squared_error = 0;
        for (size_t i = 0; i < actual.size(); ++i)
        {
            double error = actual[i] - predicted[i];
            sum_squared_error += pow(error, 2);
        }
        return sqrt(sum_squared_error / actual.size());
    };
    void linearFit(const vector<double> &x_values, const vector<double> &y_values, double &slope, double &RSQ)
    {
        if (x_values.size() != y_values.size() || x_values.size() == 0)
        {
            std::cerr << "Error: Invalid input data!" << std::endl;
            return;
        }

        int n = x_values.size();
        double sum_x = 0, sum_y = 0, sum_xy = 0, sum_x_squared = 0;
        // print
        msg.prefix(" x = ") << x_values;
        msg.prefix(" y = ") << y_values;

        for (int i = 0; i < n; ++i)
        {
            sum_x += x_values[i];
            sum_y += y_values[i];
            sum_xy += x_values[i] * y_values[i];
            sum_x_squared += x_values[i] * x_values[i];
        }

        double x_mean = sum_x / n;
        double y_mean = sum_y / n;

        // double b = y_mean - a * x_mean;                                                // intercept

        // 计算协方差
        double covariance = 0;
        for (int i = 0; i < n; ++i)
        {
            covariance += (x_values[i] - x_mean) * (y_values[i] - y_mean);
        }
        // 计算标准差
        double stdDevX = 0;
        double stdDevY = 0;
        for (int i = 0; i < n; ++i)
        {
            stdDevX += std::pow(x_values[i] - x_mean, 2);
            stdDevY += std::pow(y_values[i] - y_mean, 2);
        }
        stdDevX = std::sqrt(stdDevX);
        stdDevY = std::sqrt(stdDevY);
        double correlation = covariance / (stdDevX * stdDevY);

        // msg.prefix(" sumXY= ") << sumXY;
        // msg.prefix(" sumXY2= ") << sumXY2;
        // msg.prefix("y_mean= ") << (sumXY / (sqrt(sumXY2)));
        // msg.prefix("x_mean= ") << x_mean;
        // msg.prefix("intercept= ") << b;
        slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x_squared - sum_x * sum_x); // slope
        RSQ = covariance / (stdDevX * stdDevY);                                     // R
    };
    void report(int num, bool smooth)
    {
        if (smooth == false || (smooth == true && set1.MV_point > 0))
        {
            auto mode = [](bool smooth) -> const char * { return (smooth) ? "S" : "R"; };
            string itemname = set2.Item_name;
            kpa::ins::log(num, str_format("%s_%s_slope", mode(smooth), itemname.c_str()).c_str(), slope_mean, "%.4f", "");
            kpa::ins::log(num + 1, str_format("%s_%s_RSQ", mode(smooth), itemname.c_str()).c_str(), RSQ_mean, "%.4f", "");
        }
    };
    void GetX()
    {
        // get X
        double *XA = &set2.x1;
        for (int i = 0; i < (int)db[0].il_pw.size(); i++)
            if (XA[i] == -999)
                return;
            else
            {
                X.push_back(XA[i]);
            }

        msg.prefix("XA= ") << X;
    }
    void slope(bool smooth)
    {
        // linear fit
        double ss = 0;
        double ee = 0;
        slope_.clear();
        slope_mean.clear();
        RSQ.clear();
        RSQ_mean.clear();

        for (int i = 0; i < wave_size; ++i)
        {
            double S;
            double R;
            msg.prefix("<wavelength>") << db[i].wavelength;
            if (smooth && set1.MV_point > 0)
            {
                linearFit(X, db[i].S_il_pw, S, R);
                slope_.push_back(S);
                RSQ.push_back(R);
            }
            else
            {
                linearFit(X, db[i].il_pw, S, R);
                slope_.push_back(S);
                RSQ.push_back(R);
            }

            ss = ss + slope_[i];
            ee = ee + RSQ[i];
        }
        ss = ss / wave_size;
        slope_mean.push_back(ss);
        ee = ee / wave_size;
        RSQ_mean.push_back(ee);
        Printinfo();
    };
    void Printinfo()
    {
        msg.prefix("======================================== ") << "";
        msg.prefix("slope= ") << slope_;
        msg.prefix("RSS= ") << RSQ;
        msg.prefix("slope_mean= ") << slope_mean;
        msg.prefix("RSS_mean= ") << RSQ_mean;
    }
    void clear()
    {
        X.clear();
        db.clear();
        slope_.clear();
        RSQ.clear();
        subdie_.clear();
        slope_mean.clear();
        RSQ_mean.clear();
        cond_.clear();
    };
};

#endif