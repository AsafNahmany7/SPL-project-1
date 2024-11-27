#include "Settlement.h"
#include <string>
#include <vector>
#include <string>

using std::string;
using std::to_string;

 Settlement::Settlement(const string &name, SettlementType type) : name(name),type(type){};
    const string& Settlement:: getName()const {
        int i =1;
        return name;
    };
SettlementType Settlement::getType()const {
    return type;
};
const string Settlement:: toString()const{
    string output =  "Settlement Name: " + name + ", Type: " + settlementTypeToString(type);
    return output;
    };
const string Settlement:: settlementTypeToString(SettlementType type) const {
        
    switch (type) {
        case SettlementType::VILLAGE: return "VILLAGE";
        case SettlementType::CITY: return "CITY";
        case SettlementType::METROPOLIS: return "METROPOLIS";
        default: return "UNKNOWN";
                }
}

