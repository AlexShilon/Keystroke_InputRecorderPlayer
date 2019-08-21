//
//  FeatureExtractor.cpp
//  ksdncpp
//
//  Created by MS on 6/13/16.
//  Copyright © 2016 Sarbasov inc. All rights reserved.
//  https://github.com/maratsarbasov/objc_ksdn 

#include "FeatureExtractor.hpp"
#include <cmath>
#include <cassert>
#include "Keycodes.h"

FeatureExtractor::FeatureExtractor(time_t timing_threshold) : pt(-1000), pk(0), press_time(), timing_threshold(timing_threshold), breaking(), shift_pressed(false), adjecents(std::vector<Adjecent>(ADJ_COUNT, Adjecent()))
{
    std::vector<int> vec1 =
        {kVK_ANSI_Q, kVK_ANSI_W, kVK_ANSI_E, kVK_ANSI_R, kVK_ANSI_T, kVK_ANSI_Y, kVK_ANSI_U, kVK_ANSI_I, kVK_ANSI_O, kVK_ANSI_P};
    std::vector<int> vec2 =
        { kVK_ANSI_A,  kVK_ANSI_S,  kVK_ANSI_D,  kVK_ANSI_F,  kVK_ANSI_G,  kVK_ANSI_H, kVK_ANSI_J, kVK_ANSI_K, kVK_ANSI_L};
    std::vector<int> vec3 =
        {-1,  kVK_ANSI_Z, kVK_ANSI_X,  kVK_ANSI_C,  kVK_ANSI_V, kVK_ANSI_B, kVK_ANSI_N, kVK_ANSI_M};
    
    keys_matrix.push_back(vec1);
    keys_matrix.push_back(vec2);
    keys_matrix.push_back(vec3);
    
    
    
    for (int i = 0; i < 1000; i++) {
        breaking[i] = true;
    }
}

int FeatureExtractor::distance(int key1, int key2) const
{
    int r1 = -1, c1 = -1, r2 = -1, c2 = -1;
    for (int i = 0; i < keys_matrix.size(); ++i) {
        std::vector<int> r = keys_matrix[i];
        auto search_result = std::find(r.begin(), r.end(), key1);
        if (search_result != r.end()) {
            r1 = i;
            c1 = int(search_result - r.begin());
        }
        
        search_result = std::find(r.begin(), r.end(), key2);
        if (search_result != r.end()) {
            r2 = i;
            c2 = int(search_result - r.begin());
        }
        
    }
    assert(r1 != -1 && r2 != -1 && c1 != -1 && c2 != -1);
    
    double distance = sqrt(double(pow(r1-r2, 2)) + pow(c1 - c2, 2));
    return round(distance);
}



void FeatureExtractor::on_key(int key, EventType event_type, time_t timestamp)
{
    if (key == kVK_Shift) {
        if (event_type == KEY_DOWN) {
            breaking[key] = true;
            shift_pressed = true;
        } else {
            breaking[key] = false;
            shift_pressed = false;
        }
        return;
    }
    
    if (key == kVK_Space) {
        breaking[key] = true;
        return;
    }
    
    if (!is_letter(key)) {
        breaking[key] = true;
        return;
    }
    
    if (event_type == KEY_DOWN) {
        time_t flight_time = timestamp - pt;
        if (flight_time > timing_threshold) {
            breaking[key] = true;
        } else {
            breaking[key] = false;
        }
        
        press_time[key] = timestamp;
        pt = timestamp;
    } else if (event_type == KEY_UP) {
        time_t dwell_time = timestamp - press_time[key];
        time_t ck_down = press_time[key];
        press_time.erase(key);
        
        if (!breaking[key]) {
            int adj = distance(key, pk);
            if (adj > ADJ_COUNT - 1)
                adj = ADJ_COUNT - 1;
            time_t h1 = pk_hold;
            time_t h2 = dwell_time;
            time_t down_down = ck_down - pk_down;
            time_t up_up = timestamp - pk_up;
            time_t up_down = ck_down - pk_up;
            adjecents[adj].add(h1, h2, down_down, up_up, up_down);
        }
        pk_hold = dwell_time;
        pk_up = timestamp;
        pk_down = ck_down;
        pk = key;
    }
}

std::vector<Adjecent> FeatureExtractor::extract_features()
{
    return adjecents;
}


bool FeatureExtractor::is_letter(int key) const
{
    std::vector<int> letters = {
        kVK_ANSI_A,
        kVK_ANSI_S,
        kVK_ANSI_D,
        kVK_ANSI_F,
        kVK_ANSI_H,
        kVK_ANSI_G,
        kVK_ANSI_Z,
        kVK_ANSI_X,
        kVK_ANSI_C,
        kVK_ANSI_V,
        kVK_ANSI_B,
        kVK_ANSI_Q,
        kVK_ANSI_W,
        kVK_ANSI_E,
        kVK_ANSI_R,
        kVK_ANSI_Y,
        kVK_ANSI_T,
        kVK_ANSI_O,
        kVK_ANSI_U,
        kVK_ANSI_I,
        kVK_ANSI_P,
        kVK_ANSI_L,
        kVK_ANSI_J,
        kVK_ANSI_K,
        kVK_ANSI_N,
        kVK_ANSI_M,
    };
    if (std::find(letters.begin(), letters.end(), key) != letters.end()) {
        return true;
    } else {
        return false;
    }
}

#include <windows.h>

// ****************************************************************************************************
// Replay events into system
// 
EventPlayer::EventPlayer()
{
	lastKey_timestamp = 0;
}

void EventPlayer::on_key(int key, EventType event_type, time_t timestamp)
{
	int scan = MapVirtualKey(key & 0xff, 0);

	if (lastKey_timestamp)
	{
		// wait between next event
		int wait_milis = timestamp - lastKey_timestamp;
		if (wait_milis < 0)
			wait_milis = 0;
		Sleep(wait_milis); // 5 milis for processing
	}
	
	lastKey_timestamp = timestamp; 

	if (event_type == KEY_DOWN)
	{
		
		//printf("%c %d\n", key, scan);
		keybd_event(key, scan, 0, 0); // key goes down
		
	}

	if (event_type == KEY_UP )
	{
		keybd_event(key, scan | 0x80, KEYEVENTF_KEYUP, 0); // key goes up
	}

}

