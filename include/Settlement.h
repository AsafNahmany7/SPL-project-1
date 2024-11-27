#pragma once
#include <string>
#include <vector>
using std::string;
using std::vector;

class Facility;

enum class SettlementType {
    VILLAGE,
    CITY,
    METROPOLIS,
};

class Settlement {
    public:
        Settlement(const string &name, SettlementType type) : name(name),type(type){};
        const string &getName()const {
            return name;
        };
        SettlementType getType()const {
            return type;
        };
        const string toString(){
            string output =  "Settlement Name: " + name + ", Type: " + settlementTypeToString(type);
            return output;


        };

        private:
            const string name;
            SettlementType type;

            //Private method to determine the string value from the enum for toString
            const string settlementTypeToString(SettlementType type) const {
        
                switch (type) {
                    case SettlementType::VILLAGE: return "VILLAGE";
                    case SettlementType::CITY: return "CITY";
                    case SettlementType::METROPOLIS: return "METROPOLIS";
                    default: return "UNKNOWN";
                }
            }
};