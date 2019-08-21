//
//  FeatureExtractor.hpp
//  ksdncpp
//
//  Created by MS on 6/13/16.
//  Copyright © 2016 Sarbasov inc. All rights reserved.
//  https://github.com/maratsarbasov/objc_ksdn 

#ifndef FeatureExtractor_hpp
#define FeatureExtractor_hpp

#include "KeypressEventReceiver.hpp"
#include <map>
#include <vector>
#include "Types.h"
#include "Adjecent.hpp"

#define ADJ_COUNT 6

class FeatureExtractor : public KeypressEventReceiver
{
    int pk;
    time_t pk_hold;
    time_t pk_down;
    time_t pk_up;
    time_t pt;
    std::map<int, time_t> press_time;
    time_t timing_threshold;
    std::map<int, bool> breaking;
    bool shift_pressed;
    
    std::vector<Adjecent> adjecents;
    int distance(int key1, int key2) const;
    std::vector<std::vector<int>> keys_matrix;
    bool is_letter(int key) const;


public:
    FeatureExtractor(time_t timing_threshold = 500);
    virtual void on_key(int key, EventType event_type, time_t timestamp);
    
    std::vector<Adjecent> extract_features();
    
    
};

class EventPlayer : public KeypressEventReceiver
{

	time_t lastKey_timestamp;

public:
	EventPlayer();
	virtual void on_key(int key, EventType event_type, time_t timestamp);

};
#endif /* FeatureExtractor_hpp */
