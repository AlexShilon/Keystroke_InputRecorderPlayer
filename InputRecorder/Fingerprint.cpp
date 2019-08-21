//
//  Fingerprint.cpp
//  objcksdn
//
//  Created by MS on 6/18/16.
//  Copyright © 2016 Sarbasov inc. All rights reserved.
// https://github.com/maratsarbasov/objc_ksdn 

#include "Fingerprint.hpp"
#include <cmath>
#include <cassert>
#include <algorithm>

#define M_SQRT1_2 0.70710678118654752440 

double calculate_mean(std::vector<time_t> v)
{
    time_t s = 0;
    for (time_t i : v)
        s += i;
    return double(s)/v.size();
}

double calculate_std_dev(std::vector<time_t> v)
{
    double mean=0.0, sum_deviation=0.0;
    int i;
    for(i = 0; i < v.size(); i++) {
        mean += v[i];
    }
    mean = mean / v.size();
    for(i = 0; i < v.size(); i++)
        sum_deviation += (v[i] - mean) * (v[i] - mean);
    return sqrt(sum_deviation/v.size());
}

double distance(std::vector<double> v1, std::vector<double> v2)
{
    assert(v1.size() == v2.size());
    double d = 0.0;
    for (int i = 0; i < v1.size(); i++) {
		if (isnan(v1[i]) || isnan(v2[i]))
			continue;
        d += pow(v1[i] - v2[i], 2);
    }
    return sqrt(d);
}

double length(std::vector<double> v)
{
    return distance(v, std::vector<double>(v.size(), 0.0));
}

Fingerprint::Fingerprint(std::vector<Adjecent> adjc)
{
    const double stddev_coef = 3.0;
    for (auto& adj : adjc) {
        mean.push_back(calculate_mean(adj.get_h1()));
//        mean.push_back(calculate_mean(adj.get_h2()));
        mean.push_back(calculate_mean(adj.get_dd()));
        mean.push_back(calculate_mean(adj.get_uu()));
        mean.push_back(calculate_mean(adj.get_ud()));
        
        threshold.push_back(stddev_coef * calculate_std_dev(adj.get_h1()));
//        threshold.push_back(stddev_coef * calculate_std_dev(adj.get_h2()));
        threshold.push_back(stddev_coef * calculate_std_dev(adj.get_dd()));
        threshold.push_back(stddev_coef * calculate_std_dev(adj.get_uu()));
        threshold.push_back(stddev_coef * calculate_std_dev(adj.get_ud()));
    }
}
Fingerprint::Fingerprint()
{}

std::vector<double> Fingerprint::get_mean()
{
    return mean;
}

double normalCFD(double value)
{
    return 0.5 * erfc(-value * M_SQRT1_2);
}

double Fingerprint::compare_with(Fingerprint &f)
{

    double d = distance(f.get_mean(), mean);
    return d;
    //return - atan(1.0/(length(threshold)/3)) * d + 1.0;
    
}

double Fingerprint::euclid_distance(Fingerprint &f)
{
    double d = distance(f.get_mean(), mean);
    return d;
}

double Fingerprint::canberra_distance(Fingerprint &f)
{
    auto a = f.get_mean();
    auto b = mean;
    
    
    double res = 0.0;
    for (int i = 0; i < a.size(); i++) {
		double d = std::abs(a[i] - b[i]) / (std::abs(a[i]) + std::abs(b[i]));
		if ( !isnan(d))
			res += d;
    }
    return res;
}

double Fingerprint::manhattan_distance(Fingerprint &f)
{
    auto a = f.get_mean();
    auto b = mean;
    
    
    double res = 0.0;
    for (int i = 0; i < a.size(); i++) {
		double d = std::abs(a[i] - b[i]);
		if (!isnan(d))
			res += d;
    }
    return res;
}
