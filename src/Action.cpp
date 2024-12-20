#include "Action.h"
#include <iostream>
#include <string>
#include "Simulation.h"
using std::string;
using namespace std;

BaseAction::BaseAction() : errorMsg(""), status(ActionStatus::ERROR) {}

ActionStatus BaseAction::getStatus() const {
    return status;
}

void BaseAction::complete() {
    status = ActionStatus::COMPLETED;
}

void BaseAction::error(string errorMsg) {
    status = ActionStatus::ERROR;
    this->errorMsg = errorMsg;
    std::cout << "Error: " << errorMsg << std::endl;
}

const string& BaseAction::getErrorMsg() const {
    return errorMsg;
}

SimulateStep::SimulateStep(const int numOfSteps) : numOfSteps(numOfSteps) {}

void SimulateStep::act(Simulation &simulation) {
    for(int i = 0; i < numOfSteps; i++) {
        simulation.step();
    }
    complete();
}

const string SimulateStep::toString() const {
    return "step " + std::to_string(numOfSteps) + " " + (getStatus() == ActionStatus::COMPLETED ? "COMPLETED" : "ERROR");
}

SimulateStep* SimulateStep::clone() const {
    return new SimulateStep(*this);
}

AddPlan::AddPlan(const string &settlementName, const string &selectionPolicy)
    : settlementName(settlementName), selectionPolicy(selectionPolicy) {}

void AddPlan::act(Simulation &simulation) {
    if(!simulation.isSettlementExists(settlementName)) {
        error("Cannot create this plan");
        return;
    }

    SelectionPolicy* policy = nullptr;
    if(selectionPolicy == "nve") {
        policy = new NaiveSelection();
    } else if(selectionPolicy == "bal") {
        policy = new BalancedSelection(0,0,0);
    } else if(selectionPolicy == "eco") {
        policy = new EconomySelection();
    } else if(selectionPolicy == "env") {
        policy = new SustainabilitySelection();
    } else {
        error("Cannot create this plan");
        return;
    }

    Settlement& settlement = simulation.getSettlement(settlementName);
    simulation.addPlan(settlement, policy);
    complete();
}

const string AddPlan::toString() const {
    return "plan " + settlementName + " " + selectionPolicy + " " + 
           (getStatus() == ActionStatus::COMPLETED ? "COMPLETED" : "ERROR");
}

AddPlan* AddPlan::clone() const {
    return new AddPlan(*this);
}

AddSettlement::AddSettlement(const string &settlementName, SettlementType settlementType)
    : settlementName(settlementName), settlementType(settlementType) {}

void AddSettlement::act(Simulation &simulation) {
    if(simulation.isSettlementExists(settlementName)) {
        error("Settlement already exists");
        return;
    }
    
    Settlement* newSettlement = new Settlement(settlementName, settlementType);
    if(!simulation.addSettlement(newSettlement)) {
        delete newSettlement;
        error("Settlement already exists");
    } else {
        complete();
    }
}

const string AddSettlement::toString() const {
    return "settlement " + settlementName + " " + std::to_string(static_cast<int>(settlementType)) + " " +
           (getStatus() == ActionStatus::COMPLETED ? "COMPLETED" : "ERROR");
}

AddSettlement* AddSettlement::clone() const {
    return new AddSettlement(*this);
}

AddFacility::AddFacility(const string &facilityName, const FacilityCategory facilityCategory, const int price,
                         const int lifeQualityScore, const int economyScore, const int environmentScore)
            : facilityName(facilityName), facilityCategory(facilityCategory), price(price),
            lifeQualityScore(lifeQualityScore), economyScore(economyScore), environmentScore(environmentScore) {}

void AddFacility::act(Simulation &simulation) {
    FacilityType newFacility(facilityName, facilityCategory, price, 
                            lifeQualityScore, economyScore, environmentScore);
    if(!simulation.addFacility(newFacility)) {
        error("Facility already exists");
    } else {
        complete();
    }
}

const string AddFacility::toString() const {
    return "facility " + facilityName + " " + std::to_string(static_cast<int>(facilityCategory)) + " " +
           std::to_string(price) + " " + std::to_string(lifeQualityScore) + " " +
           std::to_string(economyScore) + " " + std::to_string(environmentScore) + " " +
           (getStatus() == ActionStatus::COMPLETED ? "COMPLETED" : "ERROR");
}

AddFacility* AddFacility::clone() const {
    return new AddFacility(*this);
}

PrintPlanStatus::PrintPlanStatus(int planId) : planId(planId) {}

void PrintPlanStatus::act(Simulation &simulation) {
    if (simulation.isPlanExists(planId)) {
        Plan& plan = simulation.getPlan(planId);
        
        std::cout << "PlanID: " << planId << std::endl;
        std::cout << "SettlementName: " << plan.getSettlementName() << std::endl;
        std::cout << "PlanStatus: " << (plan.isAvailable() ? "AVAILABLE" : "BUSY") << std::endl;
        std::cout << "SelectionPolicy: " << plan.getSelectionPolicyString() << std::endl;
        
        std::cout << "LifeQualityScore: " << plan.getlifeQualityScore() << std::endl;
        std::cout << "EconomyScore: " << plan.getEconomyScore() << std::endl;
        std::cout << "EnvrionmentScore: " << plan.getEnvironmentScore() << std::endl;

        const std::vector<Facility*>& underConstruction = plan.getUnderConstruction();
        for (const Facility* facility : underConstruction) {
            std::cout << "FacilityName: " << facility->getName() << std::endl;
            std::cout << "FacilityStatus: UNDER_CONSTRUCTION" << std::endl; 
        }

        const std::vector<Facility*>& facilities = plan.getFacilities();
        for (const Facility* facility : facilities) {
            std::cout << "FacilityName: " << facility->getName() << std::endl;
            std::cout << "FacilityStatus: OPERATIONAL" << std::endl; 
        }
        
        complete();
    } else {
        error("Plan doesn't exist");
    }
}

const string PrintPlanStatus::toString() const {
    return "planStatus " + std::to_string(planId) + " " +
           (getStatus() == ActionStatus::COMPLETED ? "COMPLETED" : "ERROR");
}

PrintPlanStatus* PrintPlanStatus::clone() const {
    return new PrintPlanStatus(*this);
}

ChangePlanPolicy::ChangePlanPolicy(const int planId, const string &newPolicy)
    : planId(planId), newPolicy(newPolicy) {}

void ChangePlanPolicy::act(Simulation &simulation) {
    
    Plan& plan = simulation.getPlan(planId);

    if (!simulation.isPlanExists(planId) || plan.getSelectionPolicyString() == newPolicy) {
        error("Cannot change selection policy");
        return;
    }

    SelectionPolicy* policy = nullptr;
    
    if(newPolicy == "nve") {
        policy = new NaiveSelection();
    } else if(newPolicy == "bal") {
        policy = new BalancedSelection(plan.getlifeQualityScore(),plan.getEconomyScore(),plan.getEnvironmentScore());
    } else if(newPolicy == "eco") {
        policy = new EconomySelection();
    } else if(newPolicy == "env") {
        policy = new SustainabilitySelection();
    } else {
        error("Cannot change selection policy");
        return;
    }

    plan.setSelectionPolicy(policy);
    complete();
}

const string ChangePlanPolicy::toString() const {
    return "changePolicy " + std::to_string(planId) + " " + newPolicy + " " +
           (getStatus() == ActionStatus::COMPLETED ? "COMPLETED" : "ERROR");
}

ChangePlanPolicy* ChangePlanPolicy::clone() const {
    return new ChangePlanPolicy(*this);
}

PrintActionsLog::PrintActionsLog() {}

void PrintActionsLog::act(Simulation &simulation) {
    for(const BaseAction* action : simulation.getActionsLog()) {
        std::cout << action->toString() << std::endl;
    }
    complete();
}

const string PrintActionsLog::toString() const {
    string status = (getStatus() == ActionStatus::COMPLETED) ? "COMPLETED" : "ERROR";
    return "log " + status;
}


PrintActionsLog* PrintActionsLog::clone() const {
    return new PrintActionsLog(*this);
}

Close::Close() {}

void Close::act(Simulation &simulation) {
    const std::vector<Plan>& plans = simulation.getPlans();
    
    for (int counter=0;counter<(static_cast<int>(plans.size()));counter++) {
        Plan& plan = simulation.getPlan(counter);
        std::cout << "PlanID: " << counter << std::endl;
        std::cout << "SettlementName: " << plan.getSettlementName() << std::endl;
        std::cout << "LifeQuality_Score: " << plan.getlifeQualityScore() << std::endl;
        std::cout << "Economy_Score: " << plan.getEconomyScore() << std::endl;
        std::cout << "Environment_Score: " << plan.getEnvironmentScore() << std::endl;
        std::cout << std::endl;  
    }
    
    simulation.close();
    complete();
}

const string Close::toString() const {
    string status = (getStatus() == ActionStatus::COMPLETED) ? "COMPLETED" : "ERROR";
    return "close " + status;
}

Close* Close::clone() const {
    return new Close(*this);
}

BackupSimulation::BackupSimulation() {}

void BackupSimulation::act(Simulation &simulation) {
    extern Simulation* backup;
    if(backup != nullptr) {
        delete backup;
    }
    backup = new Simulation(simulation);
    complete();
}

const string BackupSimulation::toString() const {
    string status = (getStatus() == ActionStatus::COMPLETED) ? "COMPLETED" : "ERROR";
    return "backup " + status;
}

BackupSimulation* BackupSimulation::clone() const {
    return new BackupSimulation(*this);
}

RestoreSimulation::RestoreSimulation() {}

void RestoreSimulation::act(Simulation &simulation) {
    extern Simulation* backup;
    if(backup == nullptr) {
        error("No backup available");
        return;
    }
    
    simulation = *backup;
    complete();
}

const string RestoreSimulation::toString() const {
    string status = (getStatus() == ActionStatus::COMPLETED) ? "COMPLETED" : "ERROR";
    return "restore " + status;
}

RestoreSimulation* RestoreSimulation::clone() const {
    return new RestoreSimulation(*this);
}