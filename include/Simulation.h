#pragma once
#include <string>
#include <vector>
#include "Facility.h"
#include "Plan.h"
#include "Settlement.h"
#include "SelectionPolicy.h"
#include "Action.h"
using std::string;
using std::vector;

class BaseAction;
class SelectionPolicy;

class Simulation {
    public:
        Simulation(const string &configFilePath);
        Simulation(const Simulation& other);
        Simulation& operator=(const Simulation& other);
        Simulation(Simulation&& other);
        Simulation& operator=(Simulation&& other);
        ~Simulation();
        
        void start();
        void addPlan(const Settlement &settlement, SelectionPolicy *selectionPolicy);
        void addAction(BaseAction *action);
        bool addSettlement(Settlement *settlement);
        bool addFacility(FacilityType facility);
        bool isSettlementExists(const string &settlementName);
        Settlement &getSettlement(const string &settlementName);
        Plan &getPlan(const int planID);
        void step();
        void close();
        void open();
        const vector<BaseAction*>& getActionsLog() const;
        bool isPlanExists(const int planID);
        const std::vector<Plan>& getPlans() const;


    private:
        bool isRunning;
        int planCounter; 
        vector<BaseAction*> actionsLog;
        vector<Plan> plans;
        vector<Settlement*> settlements;
        vector<FacilityType> facilitiesOptions;
};