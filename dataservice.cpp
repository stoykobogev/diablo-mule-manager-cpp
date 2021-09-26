#include "dataservice.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "json.hpp"
#include <filesystem>

using json = nlohmann::json;

string DataService::DIR = "./data";
string DataService::MULES_FILE = DataService::DIR + "/mules.json";
string DataService::ITEMS_FILE = DataService::DIR + "/items.json";

void DataService::saveMules() {
    ofstream os(DataService::MULES_FILE);

    json j = DataService::muleVectorToJson(DataService::MULES);
    os << j;

    os.close();
}

string DataService::readFile(string& path)
{
    string json = "";
    ifstream is(path);

    if (is.good()) {
        getline(is, json);
        is.close();
    }
    else {
        std::filesystem::create_directory(DataService::DIR);

        ofstream os(path);
        os.close();
    }

    return json;
}

Mule* DataService::findMule(string name) {

    for (auto it = DataService::MULES.begin(); it != DataService::MULES.end(); it++) {
        if (it->name == name) {
            return &*it;
        }
    }

    return NULL;
}

Item* DataService::findItem(string name) {
    for (auto it = DataService::ITEMS.begin(); it != DataService::ITEMS.end(); it++) {
        if (it->name == name) {
            return &*it;
        }
    }

    return NULL;
}

vector<Mule> DataService::initializeMules() {
    string jsonString = DataService::readFile(DataService::MULES_FILE);
    vector<Mule> mules;

    if (jsonString != "") {
        json muleJson = json::parse(jsonString);
        mules = DataService::muleVectorFromJson(muleJson);
    }

    sort(mules.begin(), mules.end(), [](Mule m1, Mule m2) {
        return m1.name.compare(m2.name) == -1;
    });

    return mules;
}

json DataService::itemToJson(MuleItem& item)
{
    return json::object({ {"id", item.id}, {"description", item.description}, {"note", item.note}, {"quantity", item.quantity} });
}

MuleItem DataService::itemFromJson(json& j)
{
    MuleItem item;
    j.at("id").get_to(item.id);
    j.at("description").get_to(item.description);
    j.at("note").get_to(item.note);
    j.at("quantity").get_to(item.quantity);

    return item;
}

vector<Item> DataService::initializeItems() {
    string jsonString = DataService::readFile(DataService::ITEMS_FILE);
    vector<Item> items;

    if (jsonString != "") {
        json itemJson = json::parse(jsonString);
        for (auto it = itemJson.begin(); it != itemJson.end(); ++it) {
            Item item;
            it->at("id").get_to(item.id);
            it->at("name").get_to(item.name);
            items.push_back(item);
        }
    }

    sort(items.begin(), items.end(), [](Item i1, Item i2) {
        return i1.name.compare(i2.name) == -1;
    });

    return items;
}

json DataService::muleToJson(Mule& mule) {
    json j;
    j["name"] = mule.name;
    j["items"] = json::array({});

    for (size_t i = 0; i < mule.items.size(); i++)
    {
        j["items"][i] = DataService::itemToJson(mule.items[i]);
    }

    return j;
}

Mule DataService::muleFromJson(json& muleJson) {
    Mule mule;
    muleJson.at("name").get_to(mule.name);
    mule.items = DataService::itemVectorFromJson(muleJson["items"]);

    return mule;
}

json DataService::itemVectorToJson(vector<MuleItem>& items)
{
    json j = json::array({});

    for (size_t i = 0; i < items.size(); i++)
    {
        j[i] = DataService::itemToJson(items[i]);
    }

    return j;
}

vector<MuleItem> DataService::itemVectorFromJson(json& j)
{
    vector<MuleItem> muleItems;
    for (auto it = j.begin(); it != j.end(); ++it) {
        MuleItem muleItem = DataService::itemFromJson(it.value());
        for (auto& item : DataService::ITEMS) {
            if (muleItem.id == item.id) {
              muleItem.name = item.name;
              break;
            } 
        }

        if (muleItem.name.empty()) {
            cerr << "Mule item id: '" << muleItem.id << "' was not found" << endl;
            exit(1);
        }

        muleItems.push_back(muleItem);
    }

    return muleItems;
}

json DataService::muleVectorToJson(vector<Mule>& mules)
{
    json j = json::array();

    for (size_t i = 0; i < mules.size(); i++)
    {
        j[i] = DataService::muleToJson(mules[i]);
    }

    return j;
}
vector<Mule> DataService::muleVectorFromJson(json& j)
{
    vector<Mule> mules;
    for (auto it = j.begin(); it != j.end(); ++it) {
        Mule mule = DataService::muleFromJson(it.value());
        mules.push_back(mule);
    }

    return mules;
}

vector<Item> DataService::ITEMS = DataService::initializeItems();
vector<Mule> DataService::MULES = DataService::initializeMules();
