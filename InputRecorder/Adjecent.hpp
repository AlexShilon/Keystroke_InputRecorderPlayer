//
//  Adjecent.hpp
//  objcksdn
//
//  Created by MS on 6/18/16.
//  Copyright © 2016 Sarbasov inc. All rights reserved.
//

#ifndef Adjecent_hpp
#define Adjecent_hpp

#include <vector>

//TODO: make as DictTree
class Adjecent
{
    std::vector<time_t> m_h1, m_h2, m_down_down, m_up_up, m_up_down;
    
    static time_t get_mean(std::vector<time_t> vec) {
        time_t sum = 0;
        for (time_t& i : vec) {
            sum += i;
        }
        return sum/vec.size();
    }
public:
    Adjecent() {}
    void add(time_t h1, time_t h2, time_t down_down, time_t up_up, time_t up_down) {
        m_h1.push_back(h1);
        m_h2.push_back(h2);
        m_down_down.push_back(down_down);
        m_up_up.push_back(up_up);
        m_up_down.push_back(up_down);
    }
    std::vector<time_t> get_means()
    {
        std::vector<time_t> means;
        means.push_back(get_mean(m_h1));
        means.push_back(get_mean(m_h2));
        means.push_back(get_mean(m_down_down));
        means.push_back(get_mean(m_up_up));
        means.push_back(get_mean(m_up_down));
        return means;
    }
    
    std::vector<time_t> get_h1()
    {
        return m_h1;
    }
    
    std::vector<time_t> get_h2()
    {
        return m_h2;
    }
    
    std::vector<time_t> get_dd()
    {
        return m_down_down;
    }
    
    std::vector<time_t> get_uu()
    {
        return m_up_up;
    }
    
    std::vector<time_t> get_ud()
    {
        return m_up_down;
    }
};

#endif /* Adjecent_hpp */
