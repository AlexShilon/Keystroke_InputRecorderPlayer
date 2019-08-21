//
//  Fingerprint.hpp
//  objcksdn
//
//  Created by MS on 6/18/16.
//  Copyright © 2016 Sarbasov inc. All rights reserved.
//

#ifndef Fingerprint_hpp
#define Fingerprint_hpp

#include <vector>
#include "Adjecent.hpp"
class Fingerprint
{
    std::vector<double> mean;
    std::vector<double> threshold;
public:
    Fingerprint();
    Fingerprint(std::vector<Adjecent>);
    double compare_with(Fingerprint& f);
    std::vector<double> get_mean();
    double euclid_distance(Fingerprint &f);
    double canberra_distance(Fingerprint &);
    double manhattan_distance(Fingerprint &f);

};



#endif /* Fingerprint_hpp */
