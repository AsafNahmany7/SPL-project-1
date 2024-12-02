#include <fstream>          
#include <sstream>           
#include <string>         
#include <vector>         
#include <stdexcept>     
#include "Simulation.h"
#include <iostream>
using std::cout;
using std::endl;

Simulation::Simulation(const string &configFilePath):isRunning(false), planCounter(0), actionsLog(), plans(), settlements(), facilitiesOptions(){
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
            int type;
            iss >> name >> type;
            settlements.push_back(new Settlement(name, static_cast<SettlementType>(type)));
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
      facilitiesOptions(other.facilitiesOptions),
      plans(other.plans) 

{
    for (BaseAction* action : other.actionsLog) {
        actionsLog.push_back(action->clone()); 
    }

    for (Settlement* settlement : other.settlements) {
        settlements.push_back(new Settlement(*settlement)); 
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
    if (this != &other) {

        for (BaseAction* action : actionsLog) {
            delete action;
        }
        actionsLog.clear();

        for (Settlement* settlement : settlements) {
            delete settlement;
        }
        settlements.clear();

        isRunning = other.isRunning;
        planCounter = other.planCounter;
        facilitiesOptions = other.facilitiesOptions;
        plans = other.plans; 

        for (BaseAction* action : other.actionsLog) {
            actionsLog.push_back(action->clone());
        }

        for (Settlement* settlement : other.settlements) {
            settlements.push_back(new Settlement(*settlement));
        }
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
} 

bool Simulation::addFacility(FacilityType facility){
    facilitiesOptions.push_back(facility);
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
    return plans[planID];
}

void Simulation::step() {
    for (Plan& plan : plans) {
        plan.step();
    }
}

void Simulation::close() {
   for (size_t i = 0; i < plans.size(); ++i) {
        const Plan& plan = plans[i];
        std::cout << "PlanID: " << i << "\n"; 
        std::cout << "SettlementName: " << plan.getSettlementName() << "\n";
        std::cout << "LifeQuality_Score: " << plan.getlifeQualityScore() << "\n";
        std::cout << "Economy_Score: " << plan.getEconomyScore() << "\n";
        std::cout << "Environment_Score: " << plan.getEnvironmentScore() << "\n";
        std::cout << "----------------------------------\n";
    }

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
