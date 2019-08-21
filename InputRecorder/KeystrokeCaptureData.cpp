//
//  KeystrokeCaptureData.cpp
//  ksdncpp
//
//  Created by MS on 6/13/16.
//  Copyright © 2016 Sarbasov inc. All rights reserved.
//

#include "KeystrokeCaptureData.hpp"
#include <iostream>
#include <fstream>
#include <sstream>



KeystrokeCaptureData::KeystrokeCaptureData() : log()
{}

KeystrokeCaptureData::KeystrokeCaptureData(std::ifstream& file) : log()
{
    std::string str;
    while(std::getline(file, str)) {
        std::istringstream ss(str);
        int key;
        int event_int;
        EventType event_type;
        time_t timestamp;
        ss >> key;
        ss >> event_int;
        ss >> timestamp;
        event_type = EventType(event_int);
        auto tuple = std::make_tuple(key, event_type, timestamp);
        log.push_back(tuple);
    }
}

void KeystrokeCaptureData::on_key(int key, EventType event_type, time_t timestamp)
{
    auto tuple = std::make_tuple(key, event_type, timestamp);
    log.push_back(tuple);
}

void KeystrokeCaptureData::feed(KeypressEventReceiver& event_receiver)
{
    for (auto &event : log) {
        event_receiver.on_key(std::get<0>(event), std::get<1>(event), std::get<2>(event));
    }
}

void KeystrokeCaptureData::print_log(std::ostream& stream)
{
    for (auto& tuple : log) {
        stream << std::get<0>(tuple) << "  " << std::get<1>(tuple) << "  " << std::get<2>(tuple) << std::endl;
    }
}

int KeystrokeCaptureData::size()
{
	return log.size();
}
