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
    
    if (configFile.peek() == std::ifstream::traits_type::eof()) {
        throw std::runtime_error("Configuration file is empty: " + configFilePath + "  ,Please add valid configFile and start again");
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
      actionsLog(),  // Initialize actionsLog here
      plans(other.plans),  // This will now work with Plan's copy constructor
      settlements(),  // Initialize settlements here
      facilitiesOptions(other.facilitiesOptions)
{
    for (BaseAction* action : other.actionsLog) {
        actionsLog.push_back(action->clone());
    }

    for (Settlement* settlement : other.settlements) {
        settlements.push_back(new Settlement(*settlement));
    }
}




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

Simulation& Simulation::operator=(Simulation&& other) {
    if(this != &other) {
        for(BaseAction* action : actionsLog) {
            delete action;
        }
        for(Settlement* settlement : settlements) {
            delete settlement;
        }
        
        isRunning = other.isRunning;
        planCounter = other.planCounter;
        plans = std::move(other.plans);
        facilitiesOptions = std::move(other.facilitiesOptions);
        actionsLog = std::move(other.actionsLog);
        settlements = std::move(other.settlements);
        
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

    for(BaseAction* currentAction : this->actionsLog){
        delete currentAction;
    }
    actionsLog.clear();

     plans.clear();

    for(Settlement* currentSettlement : this->settlements){
        delete currentSettlement;
    }
    settlements.clear();
    
   
    facilitiesOptions.clear();

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


void Simulation::start() {
    isRunning = true; 
    cout << "Simulation started. Enter commands:\n";

    while (isRunning) {
        cout << "> "; 
        string command;
        getline(cin, command); 

        std::istringstream iss(command);
        string actionType;
        iss >> actionType;

        if (actionType == "step") {
            int numOfSteps;
            iss >> numOfSteps;

            if (!iss.fail()) {
                SimulateStep* action = new SimulateStep(numOfSteps); 
                action->act(*this);
                addAction(action);

            } else {
                cout << "Invalid input for step command. Syntax: step <number of steps>\n";
            }
        }
        else if (actionType == "plan") {
            string settlementName, policyType;
            iss >> settlementName >> policyType;

            if (!iss.fail()) {
                AddPlan* action = new AddPlan(settlementName, policyType);
                action->act(*this);
                addAction(action);
            } else {
                cout << "Invalid input for plan command. Syntax: plan <settlement_name> <selection_policy>\n";
            }
        }
        else if (actionType == "settlement") {
            string settlementName;
            int settlementType;
            iss >> settlementName >> settlementType;

            if (!iss.fail() && (settlementType == 0 || settlementType == 1 || settlementType == 2)) {
                AddSettlement* action = new AddSettlement(settlementName, static_cast<SettlementType>(settlementType));
                action->act(*this);
                addAction(action);
            } else {
                cout << "Invalid input for settlement command. Syntax: settlement <settlement_name> <settlement_type (0: village, 1: city, 2: metropolis)>\n";
            }
        }
        else if (actionType == "facility") {
            string facilityName;
            int category, price, lifeqImpact, ecoImpact, envImpact;
            iss >> facilityName >> category >> price >> lifeqImpact >> ecoImpact >> envImpact;

            if (!iss.fail()) {
                AddFacility* action = new AddFacility(facilityName, static_cast<FacilityCategory>(category), price, lifeqImpact, ecoImpact, envImpact);
                action->act(*this);
                addAction(action);
            } else {
                cout << "Invalid input for facility command. Syntax: facility <facility_name> <category> <price> <lifeq_impact> <eco_impact> <env_impact>\n";
            }
        }
        else if (actionType == "planStatus") {
            int planId;
            iss >> planId;

            if (!iss.fail()) {
                PrintPlanStatus* action = new PrintPlanStatus(planId);
                action->act(*this);
                addAction(action);
            } else {
                cout << "Invalid input for planStatus command. Syntax: planStatus <plan_id>\n";
            }
        }
        else if (actionType == "changePolicy") {
            int planId;
            string newPolicy;
            iss >> planId >> newPolicy;

            if (!iss.fail()) {
                ChangePlanPolicy* action = new ChangePlanPolicy(planId, newPolicy);
                action->act(*this);
                addAction(action);
            } else {
                cout << "Invalid input for changePolicy command. Syntax: changePolicy <plan_id> <selection_policy>\n";
            }
        }
        else if (actionType == "log") {
            PrintActionsLog* action = new PrintActionsLog();
            action->act(*this);
            addAction(action);
        }
        else if (actionType == "close") {
            Close* action = new Close();
            action->act(*this); 
            addAction(action);
        }
        else if (actionType == "backup") {
            BackupSimulation* action = new BackupSimulation();
            action->act(*this);
            addAction(action);
        }
        else if (actionType == "restore") {
            RestoreSimulation* action = new RestoreSimulation();
            action->act(*this);
            addAction(action);
        }
        else {
            cout << "Unknown command: " << actionType << "\n";
        }
    }

    cout << "Simulation stopped.\n";
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
