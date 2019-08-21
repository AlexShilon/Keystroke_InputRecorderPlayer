//
//  DictTree.hpp
//  objcksdn
//
//  Created by MS on 6/18/16.
//  Copyright © 2016 Sarbasov inc. All rights reserved.
//

#ifndef DictTree_hpp
#define DictTree_hpp

#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include <functional>

struct DictTreeVertex
{
    std::string name;
};

class DictTree : public DictTreeVertex
{
    std::map<std::string, std::shared_ptr<DictTreeVertex>> m_children;
public:
    DictTree(std::string name_, std::vector<std::shared_ptr<DictTreeVertex>> children);
    static bool is_leave(std::shared_ptr<DictTreeVertex>& a);
    static DictTree intersect(std::shared_ptr<DictTree> f1, std::shared_ptr<DictTree> f2);
    
    template <typename return_type, typename values>
    DictTree map(std::function<return_type(values)>);

};

#endif /* DictTree_hpp */
