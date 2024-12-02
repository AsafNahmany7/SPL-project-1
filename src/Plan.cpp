#include "Plan.h"
#include <iostream>
#include <string>

Plan::Plan(const int planId, const Settlement &settlement, SelectionPolicy *selectionPolicy, const vector<FacilityType> &facilityOptions): plan_id(planId),
settlement(settlement),
construction_cap((static_cast<int>(settlement.getType())) + 1),
selectionPolicy(selectionPolicy),
status(PlanStatus::AVALIABLE),
facilities(),
underConstruction(),
facilityOptions(facilityOptions),
life_quality_score(0),
economy_score(0),
environment_score(0){}

Plan::Plan(const Plan& other): plan_id(other.plan_id),
      settlement(other.settlement),
      construction_cap(other.construction_cap),
      status(other.status),
      facilityOptions(other.facilityOptions),
      life_quality_score(other.life_quality_score),
      economy_score(other.economy_score),
      environment_score(other.environment_score),
      selectionPolicy(other.selectionPolicy->clone())
      
{
    for (Facility* facility : other.facilities) {
        Facility* facility_copy = new Facility(*facility);
        this->facilities.push_back(facility_copy);
    }

    for (Facility* facility : other.underConstruction) {
         Facility* facility_copy = new Facility(*facility);
         this->underConstruction.push_back(facility_copy);   
    }




}


Plan::~Plan() {

    for (Facility* facility : facilities) {
        delete facility;
    }

    for (Facility* facility : underConstruction) {
        delete facility;
    }

    delete selectionPolicy;
}



const int Plan::getlifeQualityScore()const{
    return life_quality_score;
}

const int Plan::getEconomyScore()const{
    return economy_score;
}

const int Plan::getEnvironmentScore()const{
    return environment_score;
}

void Plan::setSelectionPolicy(SelectionPolicy *selectionPolicy){
     delete this->selectionPolicy;
    this-> selectionPolicy = selectionPolicy; 
}


void Plan::step(){

    if (status == PlanStatus::AVALIABLE){

        while(facilities.size() <= construction_cap){
           const FacilityType& selectedFacility = selectionPolicy->selectFacility(facilityOptions);
           Facility* newFacility = new Facility(selectedFacility, settlement.getName());
           underConstruction.push_back(newFacility);
        }

        if(underConstruction.size() == construction_cap){
            this->status = PlanStatus::BUSY;
        }

       for (Facility* facility : underConstruction) {
          facility->step(); 
          if (facility->getStatus() == FacilityStatus::OPERATIONAL){
            this->addFacility(facility);
            this->status = PlanStatus::AVALIABLE;
          }
       }

    }

    else{

        for (Facility* facility : underConstruction) {
          facility->step(); 
          if (facility->getStatus() == FacilityStatus::OPERATIONAL){
            this->addFacility(facility);
            this->status = PlanStatus::AVALIABLE;
          }
          
       }

    }

}

void Plan::printStatus(){
    switch (status) {
        case PlanStatus::AVALIABLE:
            std::cout << "The current plan status is: AVALIABLE " << std::endl;
            break;
        case PlanStatus::BUSY:
            std::cout << "The current plan status is: BUSY " << std::endl;
            break;
    }    
}

const vector<Facility*>& Plan::getFacilities() const{
    return facilities;
}

void Plan::addFacility(Facility* facility){
    auto iter = underConstruction.begin();
    
    for (; iter != underConstruction.end(); ++iter) {
        if (*iter == facility) {
            break;
        }

    }

    if (iter != underConstruction.end()) {
        underConstruction.erase(iter);
    }



    facilities.push_back(facility);
    this->environment_score = facility->getEnvironmentScore();
    this->economy_score = facility->getEconomyScore();
    this->life_quality_score = facility->getLifeQualityScore(); 




}

const string Plan::toString() const{
    string result = "Plan ID: " + std::to_string(plan_id) + "\n";
    result += "Facilities:\n";

    if (facilities.empty()) {
        result += "  None\n";
    }

     else {

        for (size_t i = 0; i < facilities.size(); ++i) {
            result += "  " + std::to_string(i + 1) + ". " + facilities[i]->getName() + "\n";
        }
    }
    
    result += "Under Construction:\n";
    if (underConstruction.empty()) {
        result += "  None\n";
    } 
    
    else {
        for (size_t i = 0; i < underConstruction.size(); ++i) {
            result += "  " + std::to_string(i + 1) + ". " + underConstruction[i]->getName() + "\n";
        }
    }

    return result;
}

const string Plan::getSettlementName() const
{   
    return settlement.getName();
}

bool Plan::isAvailable()
{
    if(status == PlanStatus::AVALIABLE){
        return true;
    }
    return false;
}

const string Plan::getSelectionPolicyString() const
{
    if (dynamic_cast<NaiveSelection*>(selectionPolicy)) {
        return "nve";
    }
    else if (dynamic_cast<BalancedSelection*>(selectionPolicy)) {
        return "bal";
    }
    else if (dynamic_cast<EconomySelection*>(selectionPolicy)) {
        return "eco";
    }
    else if (dynamic_cast<SustainabilitySelection*>(selectionPolicy)) {
        return "env";
    }
    
    return "unknown";
}

const int Plan::getPlanId() const
{
    return plan_id;
}
