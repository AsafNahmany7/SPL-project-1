#include <fstream>          
#include <sstream>           
#include <string>         
#include <vector>         
#include <stdexcept>     
#include "Simulation.h"
#include <iostream>
using std::cout;
using std::endl;

Simulation::Simulation(const string &configFilePath):
isRunning(false), 
planCounter(0),
actionsLog(),
plans(),
settlements(),
facilitiesOptions()
{
     std::ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        throw std::runtime_error("Failed to open configuration file: " + configFilePath);
    }

    string line;
    while (std::getline(configFile, line)) {
        std::istringstream iss(line);
        string type;
        iss >> type;

        if (type == "settlement") {
            string name;
            int settlementType;
            iss >> name >> settlementType;
            settlements.push_back(new Settlement(name, static_cast<SettlementType>(settlementType)));
        } 
        else if (type == "facility") {
            string name;
            int category, price, lifeqImpact, ecoImpact, envImpact;
            iss >> name >> category >> price >> lifeqImpact >> ecoImpact >> envImpact;
            facilitiesOptions.emplace_back(name, static_cast<FacilityCategory>(category), price, lifeqImpact, ecoImpact, envImpact);
        } 
        else if (type == "plan") {
            string settlementName, policyType;
            iss >> settlementName >> policyType;

            SelectionPolicy* policy = nullptr;
            if (policyType == "eco") {
                policy = new EconomySelection();
            } 
            else if (policyType == "bal") {
                policy = new BalancedSelection(0, 0, 0);
                
            } 
            
            else if (policyType == "env") {
                policy = new SustainabilitySelection();
            }

            Settlement& settlement = getSettlement(settlementName);
            addPlan(settlement, policy);
        }
    }

    configFile.close();
}

Simulation::Simulation(const Simulation& other)
    : isRunning(other.isRunning),
      planCounter(other.planCounter),
      plans(other.plans) ,
      facilitiesOptions(other.facilitiesOptions)
      

{
    for (BaseAction* currentOtherAction : other.actionsLog) {
        BaseAction* currentOtherActionCopy = currentOtherAction->clone();
        this->actionsLog.push_back(currentOtherActionCopy);
    }

    for (Settlement* currentOtherSettlement : other.settlements) {
        Settlement* currentOtherSettlementCopy = new Settlement(*currentOtherSettlement);
        this->settlements.push_back(currentOtherSettlement);
    }
}

Simulation::Simulation(Simulation&& other){
    this->isRunning=other.isRunning;
    this->planCounter=other.planCounter;
    this->plans = other.plans;
    this->facilitiesOptions = other.facilitiesOptions;

    for (BaseAction* currentOtherAction : other.actionsLog) {
        BaseAction* currentOtherActionCopy = currentOtherAction;
        this->actionsLog.push_back(currentOtherActionCopy);
        currentOtherAction=nullptr;
    }

    for (Settlement* currentOtherSettlement : other.settlements) {
        Settlement* currentOtherSettlementCopy = currentOtherSettlement;
        this->settlements.push_back(currentOtherSettlementCopy);
        currentOtherSettlement = nullptr;
    }



}

Simulation& Simulation::operator=(Simulation&& other){
    if(this==&other){
        return *this;
    }

     for(BaseAction* currentAction : this->actionsLog){
        delete currentAction;
        this->actionsLog.clear();
    }

    for(Plan currentPlan : this->plans){
        this->plans.clear();
    }
    for(Settlement* currentSettlement : this->settlements){
        delete currentSettlement;
        this->settlements.clear();
    }
    for(FacilityType currentFacility : this->facilitiesOptions){
        this->facilitiesOptions.clear();
    }


    this->isRunning = other.isRunning;
    this->planCounter = other.planCounter;

     for (BaseAction* currentOtherAction : other.actionsLog) {
        BaseAction* currentOtherActionCopy = currentOtherAction;
        this->actionsLog.push_back(currentOtherActionCopy);
        currentOtherAction=nullptr;
    }

    for (Settlement* currentOtherSettlement : other.settlements) {
        Settlement* currentOtherSettlementCopy = currentOtherSettlement;
        this->settlements.push_back(currentOtherSettlementCopy);
        currentOtherSettlement = nullptr;
    }



 
}



Simulation::~Simulation() {
    for (BaseAction* action : actionsLog) {
        delete action;
    }

    for (Settlement* settlement : settlements) {
        delete settlement;
    }

}

Simulation& Simulation::operator=(const Simulation& other) {
    if(this == &other){
        return *this;
    }



    for(BaseAction* currentAction : this->actionsLog){
        delete currentAction;
        this->actionsLog.clear();
    }

    for(Plan currentPlan : this->plans){
        this->plans.clear();
    }
    for(Settlement* currentSettlement : this->settlements){
        delete currentSettlement;
        this->settlements.clear();
    }
    for(FacilityType currentFacility : this->facilitiesOptions){
        this->facilitiesOptions.clear();
    }


    this->isRunning = other.isRunning;
    this->planCounter = other.planCounter;

    for(BaseAction* currentOtherAction : other.actionsLog){
        BaseAction* currentOtherActionCopy = currentOtherAction->clone();
        this->actionsLog.push_back(currentOtherActionCopy);
    }

    for(Plan currentOtherPlan : other.plans){
        this->plans.emplace_back(currentOtherPlan);
    }
    for(Settlement* currentOtherSettlement : other.settlements){
        Settlement* currentOtherSettlementCopy = new Settlement(*currentOtherSettlement);
        this->settlements.push_back(currentOtherSettlement);
    }
    for(FacilityType currentOtherFacility : other.facilitiesOptions){
        this->facilitiesOptions.emplace_back(currentOtherFacility);
    }
}




void Simulation::start(){
    isRunning = true;
}

void Simulation::addPlan(const Settlement &settlement, SelectionPolicy *selectionPolicy){
    planCounter++;
    Plan new_plan = Plan(planCounter,settlement,selectionPolicy,facilitiesOptions); 
    plans.push_back(new_plan);
}

void Simulation::addAction(BaseAction *action){
    actionsLog.push_back(action);
}

bool Simulation::addSettlement(Settlement *settlement){
    settlements.push_back(settlement);
    return true;
} 

bool Simulation::addFacility(FacilityType facility){
    facilitiesOptions.push_back(facility);
    return true;
}
bool Simulation::isSettlementExists(const string &settlementName){

    for (Settlement* settlement : settlements) {

        if (settlement->getName() == settlementName) {
            return true;   
        }
    }
    return false; 

}

Settlement& Simulation::getSettlement(const string& settlementName){
    Settlement* output = nullptr;
 
    for (Settlement* settelment_obj : settlements){
        if(settelment_obj->getName() == settlementName){
            output = settelment_obj;
        }        
    }
    return *output;
}

Plan& Simulation::getPlan(const int planID){
   if (!isPlanExists(planID)) {
        throw std::out_of_range("Plan ID is out of range.");
    }
    return plans[planID];

}

void Simulation::step() {
    for (Plan& plan : plans) {
        plan.step();
    }
}

void Simulation::close() {
    isRunning = false;

    for (BaseAction* action : actionsLog) {
        delete action;
    }
    actionsLog.clear();

    for (Settlement* settlement : settlements) {
        delete settlement;
    }
    settlements.clear();
}

void Simulation::open() {
    isRunning = true;
}

const vector<BaseAction*>& Simulation::getActionsLog() const {
    return actionsLog;
}

bool Simulation::isPlanExists(const int planID) {
    return planID >= 0 && planID < static_cast<int>(plans.size());
}

const std::vector<Plan>& Simulation::getPlans() const {
    return plans;
}
