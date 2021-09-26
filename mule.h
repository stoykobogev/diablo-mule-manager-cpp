#ifndef MULE_H
#define MULE_H
#include <string>
#include <vector>
#include "muleitem.h"

class Mule
{
public:
    string name;
    vector<MuleItem> items;

    Mule();
    ~Mule();
};

#endif // MULE_H
