#include "Action.h"
#include <iostream>
#include <string>
#include "Simulation.h"
using std::string;
using namespace std;

// BaseAction Implementation
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

// SimulateStep Implementation
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

// AddPlan Implementation
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

// AddSettlement Implementation
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

// AddFacility Implementation
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

// PrintPlanStatus Implementation
PrintPlanStatus::PrintPlanStatus(int planId) : planId(planId) {}

void PrintPlanStatus::act(Simulation &simulation) {
    if (simulation.isPlanExists(planId)) {
        Plan& plan = simulation.getPlan(planId);
        
        // Print plan basic information
        std::cout << "PlanID: " << planId << std::endl;
        std::cout << "SettlementName: " << plan.getSettlementName() << std::endl;
        std::cout << "PlanStatus: " << (plan.isAvailable() ? "AVAILABLE" : "BUSY") << std::endl;
        std::cout << "SelectionPolicy: " << plan.getSelectionPolicyString() << std::endl;
        
        // Print scores
        std::cout << "LifeQualityScore: " << plan.getlifeQualityScore() << std::endl;
        std::cout << "EconomyScore: " << plan.getEconomyScore() << std::endl;
        std::cout << "EnvrionmentScore: " << plan.getEnvironmentScore() << std::endl;

        // Print all facilities
        const std::vector<Facility*>& facilities = plan.getFacilities();
        for (const Facility* facility : facilities) {
            std::cout << "FacilityName: " << facility->getName() << std::endl;
            std::cout << "FacilityStatus: " << 
                (facility->getStatus() == FacilityStatus :: OPERATIONAL ? "OPERATIONAL" : "UNDER_CONSTRUCTIONS") << std::endl; 
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

// ChangePlanPolicy Implementation
ChangePlanPolicy::ChangePlanPolicy(const int planId, const string &newPolicy)
    : planId(planId), newPolicy(newPolicy) {}

void ChangePlanPolicy::act(Simulation &simulation) {
    if (!simulation.isPlanExists(planId)) {
        error("Cannot change selection policy");
        return;
    }

    Plan& plan = simulation.getPlan(planId);
    SelectionPolicy* policy = nullptr;
    
    if(newPolicy == "nve") {
        policy = new NaiveSelection();
    } else if(newPolicy == "bal") {
        policy = new BalancedSelection(0,0,0);
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

// PrintActionsLog Implementation
PrintActionsLog::PrintActionsLog() {}

void PrintActionsLog::act(Simulation &simulation) {
    // Using standard output as specified in the assignment
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

// Close Implementation
Close::Close() {}

void Close::act(Simulation &simulation) {
    // Get all plans from simulation
    const std::vector<Plan>& plans = simulation.getPlans();
    
    // Print details for each plan
    for (const Plan& plan : plans) {
        std::cout << "PlanID: " << plan.getPlanId() << std::endl;
        std::cout << "SettlementName: " << plan.getSettlementName() << std::endl;
        std::cout << "LifeQuality_Score: " << plan.getlifeQualityScore() << std::endl;
        std::cout << "Economy_Score: " << plan.getEconomyScore() << std::endl;
        std::cout << "Environment_Score: " << plan.getEnvironmentScore() << std::endl;
        std::cout << std::endl;  // Empty line between plans for better readability
    }
    
    // Close the simulation
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

// BackupSimulation Implementation
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

// RestoreSimulation Implementation
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