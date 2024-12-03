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

// Changes to Simulation.cpp

Simulation::Simulation(const Simulation& other)
    : isRunning(other.isRunning),
      planCounter(other.planCounter),
      actionsLog(),  // Initialize actionsLog here
      plans(other.plans),  // This will now work with Plan's copy constructor
      settlements(),  // Initialize settlements here
      facilitiesOptions(other.facilitiesOptions)
{
    // Deep copy actions
    for (BaseAction* action : other.actionsLog) {
        actionsLog.push_back(action->clone());
    }

    // Deep copy settlements
    for (Settlement* settlement : other.settlements) {
        settlements.push_back(new Settlement(*settlement));
    }
}




// Update move constructor to be more efficient
Simulation::Simulation(Simulation&& other)
    : isRunning(other.isRunning),
      planCounter(other.planCounter),
      actionsLog(std::move(other.actionsLog)),
      plans(std::move(other.plans)),
      settlements(std::move(other.settlements)),
      facilitiesOptions(std::move(other.facilitiesOptions))
      
{
    other.actionsLog.clear();
    other.settlements.clear();
}

// Update move assignment operator
Simulation& Simulation::operator=(Simulation&& other) {
    if(this != &other) {
        // Clean up existing resources
        for(BaseAction* action : actionsLog) {
            delete action;
        }
        for(Settlement* settlement : settlements) {
            delete settlement;
        }
        
        // Move all members
        isRunning = other.isRunning;
        planCounter = other.planCounter;
        plans = std::move(other.plans);
        facilitiesOptions = std::move(other.facilitiesOptions);
        actionsLog = std::move(other.actionsLog);
        settlements = std::move(other.settlements);
        
        // Clear other's resources
        other.actionsLog.clear();
        other.settlements.clear();
    }
    return *this;
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

    // First delete/clear everything
    for(BaseAction* currentAction : this->actionsLog){
        delete currentAction;
    }
    actionsLog.clear();

    for(Settlement* currentSettlement : this->settlements){
        delete currentSettlement;
    }
    settlements.clear();
    
    plans.clear();
    facilitiesOptions.clear();

    // Then copy new data
    this->isRunning = other.isRunning;
    this->planCounter = other.planCounter;

    for(BaseAction* currentOtherAction : other.actionsLog){
        BaseAction* currentOtherActionCopy = currentOtherAction->clone();
        this->actionsLog.push_back(currentOtherActionCopy);
    }

    for(const Plan& currentOtherPlan : other.plans){
        this->plans.emplace_back(currentOtherPlan);
    }

    for(Settlement* currentOtherSettlement : other.settlements){
        Settlement* currentOtherSettlementCopy = new Settlement(*currentOtherSettlement);
        this->settlements.push_back(currentOtherSettlementCopy);  // Push the copy, not the original
    }

    for(const FacilityType& currentOtherFacility : other.facilitiesOptions){
        this->facilitiesOptions.emplace_back(currentOtherFacility);
    }

    return *this;
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
