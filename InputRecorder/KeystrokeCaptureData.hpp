//
//  KeystrokeCaptureData.hpp
//  ksdncpp
//
//  Created by MS on 6/13/16.
//  Copyright © 2016 Sarbasov inc. All rights reserved.
//  https://github.com/maratsarbasov/objc_ksdn 

#pragma once

#include <tuple>
#include <vector>
#include "KeypressEventReceiver.hpp"
#include "Types.h"


class KeystrokeCaptureData : KeypressEventReceiver
{
    std::vector<std::tuple<int, EventType, time_t>> log;
    
public:
    KeystrokeCaptureData(std::ifstream& file);
    KeystrokeCaptureData();
    virtual void on_key(int key, EventType event_type,time_t timestamp);
    void feed(KeypressEventReceiver& event_receiver);
    void print_log(std::ostream& stream);
	int size();
};
