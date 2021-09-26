#ifndef MULEITEM_H
#define MULEITEM_H

#include "item.h"
#include <string>

using namespace std;

class MuleItem : public Item
{
public:
    string description;
    string note;
    int quantity;

    MuleItem();
};

#endif // MULEITEM_H
