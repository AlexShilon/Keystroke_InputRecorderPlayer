//
//  DictTree.cpp
//  objcksdn
//
//  Created by MS on 6/18/16.
//  Copyright © 2016 Sarbasov inc. All rights reserved.
//

#include "DictTree.hpp"
#include <functional>
#include <typeinfo>

DictTree::DictTree(std::string name_, std::vector<std::shared_ptr<DictTreeVertex>> children)
{
    name = name_;
    for (auto& c : children) {
        m_children[c->name] = c;
    }
}

bool DictTree::is_leave(std::shared_ptr<DictTreeVertex>& a)
{
    return typeid(a.get()) == typeid(DictTree) ? false : true;
}


DictTree DictTree::intersect(std::shared_ptr<DictTree> f1, std::shared_ptr<DictTree> f2)
{
    return DictTree("a", std::vector<std::shared_ptr<DictTreeVertex>>());
}

template <typename return_type, typename values>
DictTree DictTree::map(std::function<return_type(values)>)
{
    
}