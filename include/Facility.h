#pragma once
#include <string>
#include <vector>
using std::string;
using std::vector;

enum class FacilityStatus {
    UNDER_CONSTRUCTIONS,
    OPERATIONAL,
};

enum class FacilityCategory {
    LIFE_QUALITY,
    ECONOMY,
    ENVIRONMENT,
};


class FacilityType {
    public:
        FacilityType(const string &name, const FacilityCategory category, const int price, const int lifeQuality_score, const int economy_score, const int environment_score) : name(name),category(category),price(price),lifeQuality_score(lifeQuality_score),economy_score(economy_score),environment_score(environment_score){};
        
        //Copy constructor for Facility class's 2nd constructor
        FacilityType(const FacilityType& other):name(other.name),category(other.category),price(other.price),lifeQuality_score(other.lifeQuality_score),economy_score(other.economy_score),environment_score(other.environment_score){};


        const string &getName() const {
            return name;
        }
        int getCost() const {
            return price;
        }

        int getLifeQualityScore() const {
            return lifeQuality_score;
        }
        int getEnvironmentScore() const {
            return environment_score;
        }
        int getEconomyScore() const {
            return economy_score;
        }
        FacilityCategory getCategory() const {
            return category;
        }

    protected:
        const string name;
        const FacilityCategory category;
        const int price;
        const int lifeQuality_score;
        const int economy_score;
        const int environment_score;
};



class Facility: public FacilityType {

    public:
    //First constructor: "From Zero"
        Facility(const string &name, const string &settlementName, const FacilityCategory category, const int price, const int lifeQuality_score, const int economy_score, const int environment_score):
        FacilityType(name,category,price,lifeQuality_score,economy_score,environment_score),settlementName(settlementName),status(FacilityStatus::UNDER_CONSTRUCTIONS),timeLeft(price){};
    
    //Second constructor: "Use instance of FacilityType"
        Facility(const FacilityType &type, const string &settlementName) :
        FacilityType(type),settlementName(settlementName),status(FacilityStatus::UNDER_CONSTRUCTIONS),timeLeft(price){};
    
    //Methods:

        const string &getSettlementName() const {
            return settlementName;
        };


        const int getTimeLeft() const {
        return timeLeft;
        };

        FacilityStatus step(){

            if(timeLeft == 0){
                return status;
            }
            else{
                timeLeft--;
                if(timeLeft == 0){
                    status = FacilityStatus::OPERATIONAL;
                }

                return status;
            }
        };

        void setStatus(FacilityStatus status){
            this->status = status;
        }
        const FacilityStatus& getStatus() const {
            return status;
        }


        const string toString() {
            string result = "Facility: " + getName() + "\n";
            result += "Category: ";

            if (getCategory() == FacilityCategory::LIFE_QUALITY) {
                result += "Life Quality\n";
            } 
            else if (getCategory() == FacilityCategory::ECONOMY) {
                 result += "Economy\n";
            } 
            else if (getCategory() == FacilityCategory::ENVIRONMENT) {
                result += "Environment\n";
            }

            result += "Price: " + std::to_string(getCost()) + "\n";
            result += "Life Quality Score: " + std::to_string(getLifeQualityScore()) + "\n";
            result += "Economy Score: " + std::to_string(getEconomyScore()) + "\n";
            result += "Environment Score: " + std::to_string(getEnvironmentScore()) + "\n";
            result += "Settlement: " + getSettlementName() + "\n";

            if (getStatus() == FacilityStatus::UNDER_CONSTRUCTIONS) {
                result += "Status: Under Construction\n";
            } 

            else if (getStatus() == FacilityStatus::OPERATIONAL) {
                result += "Status: Operational\n";
            }

            result += "Time Left: " + std::to_string(getTimeLeft()) + "\n";

            return result;
        };




    private:
        const string settlementName;
        FacilityStatus status;
        int timeLeft;
};