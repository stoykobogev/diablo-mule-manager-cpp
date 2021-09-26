#ifndef DATASERVICE_H
#define DATASERVICE_H

#include <string>
#include "mule.h"
#include "item.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

class DataService
{
public:
    static vector<Mule> MULES;
    static vector<Item> ITEMS;

    static string MULES_FILE;
    static string ITEMS_FILE;

    static void saveMules();
    static void saveItems();

    static Mule* findMule(string name);
    static Item* findItem(string name);

private:
    static string DIR;

    static string readFile(string& path);

    static vector<Mule> initializeMules();
    static vector<Item> initializeItems();

    static MuleItem itemFromJson(json& j);
    static json itemToJson(MuleItem& item);
    static json itemVectorToJson(vector<MuleItem>& items);
    static vector<MuleItem> itemVectorFromJson(json& j);

    static Mule muleFromJson(json& j);
    static json muleToJson(Mule& mule);
    static json muleVectorToJson(vector<Mule>& mules);
    static vector<Mule> muleVectorFromJson(json& j);

    static bool itemComparator(Item &i1, Item &i2);
    static bool muleComparator(Mule &m1, Mule &m2);
};

#endif // DATASERVICE_H
