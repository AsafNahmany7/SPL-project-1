#include "Facility.h"
#include <string>
#include <vector>

using std::string;
using std::to_string;

// FacilityType Implementation
FacilityType::FacilityType(const string &name, FacilityCategory category, int price, int lifeQuality_score, int economy_score, int environment_score)
    : name(name), category(category), price(price), lifeQuality_score(lifeQuality_score), economy_score(economy_score), environment_score(environment_score) {}

FacilityType::FacilityType(const FacilityType& other)
    : name(other.name), category(other.category), price(other.price), lifeQuality_score(other.lifeQuality_score), economy_score(other.economy_score), environment_score(other.environment_score) {}

const string& FacilityType::getName() const {
    return name;
}

int FacilityType::getCost() const {
    return price;
}

int FacilityType::getLifeQualityScore() const {
    return lifeQuality_score;
}

int FacilityType::getEnvironmentScore() const {
    return environment_score;
}

int FacilityType::getEconomyScore() const {
    return economy_score;
}

FacilityCategory FacilityType::getCategory() const {
    return category;
}

// Facility Implementation
Facility::Facility(const string &name, const string &settlementName, FacilityCategory category, int price, int lifeQuality_score, int economy_score, int environment_score)
    : FacilityType(name, category, price, lifeQuality_score, economy_score, environment_score),
     settlementName(settlementName), 
     status(FacilityStatus::UNDER_CONSTRUCTIONS),
      timeLeft(price) {}

Facility::Facility(const FacilityType &type, const string &settlementName)
    : FacilityType(type), 
    settlementName(settlementName),
     status(FacilityStatus::UNDER_CONSTRUCTIONS),
      timeLeft(price) {}


Facility::Facility(const Facility& other)
    : FacilityType(other), 
      settlementName(other.settlementName),
      status(other.status),
      timeLeft(other.timeLeft) {}


const string& Facility::getSettlementName() const {
    return settlementName;
}

const int Facility::getTimeLeft() const {
    return timeLeft;
}

FacilityStatus Facility::step() {
    if (timeLeft > 0) {
        timeLeft--;
        if (timeLeft == 0) {
            status = FacilityStatus::OPERATIONAL;
        }
    }
    return status;
}

void Facility::setStatus(FacilityStatus status) {
    this->status = status;
}

const FacilityStatus& Facility::getStatus() const {
    return status;
}

const string Facility::toString() const  {
    string result = "Facility: " + getName() + "\n";
    result += "Category: ";
    if (getCategory() == FacilityCategory::LIFE_QUALITY) {
        result += "Life Quality\n";
    } else if (getCategory() == FacilityCategory::ECONOMY) {
        result += "Economy\n";
    } else if (getCategory() == FacilityCategory::ENVIRONMENT) {
        result += "Environment\n";
    }
    result += "Build Time: " + to_string(getCost()) + "\n";
    result += "Life Quality Score: " + to_string(getLifeQualityScore()) + "\n";
    result += "Economy Score: " + to_string(getEconomyScore()) + "\n";
    result += "Environment Score: " + to_string(getEnvironmentScore()) + "\n";
    result += "Settlement: " + getSettlementName() + "\n";
    result += "Status: ";
    if (getStatus() == FacilityStatus::UNDER_CONSTRUCTIONS) {
        result += "Under Construction\n";
    } else if (getStatus() == FacilityStatus::OPERATIONAL) {
        result += "Operational\n";
    }
    result += "Time Left: " + to_string(getTimeLeft()) + "\n";
    return result;
}