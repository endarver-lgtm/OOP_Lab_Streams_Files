#pragma once
#include <vector>
#include "Order.h"
#include "Product.h"

class OrderFile {
public:
    static const char* fileName();

    static int  count();
    static bool append(const ItemRecord& rec);
    static bool readAt(int index, ItemRecord& rec);
    static bool writeAt(int index, const ItemRecord& rec);
    static bool removeAt(int index);
    static bool loadAll(std::vector<ItemRecord>& out);
    static bool load(Order& order);
};
