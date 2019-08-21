//
//  KeypressEventReceiver.hpp
//  ksdncpp
//
//  Created by MS on 6/13/16.
//  Copyright © 2016 Sarbasov inc. All rights reserved.
//

#pragma once

#include <tuple>
#include "Types.h"

class KeypressEventReceiver
{
public:
    virtual void on_key(int key, EventType event_type, time_t timestamp) = 0;
};
