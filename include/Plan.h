#pragma once
#include <vector>
#include "Facility.h"
#include "Settlement.h"
#include "SelectionPolicy.h"
using std::vector;

enum class PlanStatus {
    AVALIABLE,
    BUSY,
};

class Plan {
    public:
        Plan(const int planId, const Settlement &settlement, SelectionPolicy *selectionPolicy, const vector<FacilityType> &facilityOptions);
        Plan(const Plan& other);
        // Add this to your Plan.h file in the public section
        Plan& operator=(const Plan& other) = delete;
        ~Plan();
        int getlifeQualityScore() const;
        int getEconomyScore() const;
        int getEnvironmentScore() const;
        void setSelectionPolicy(SelectionPolicy *selectionPolicy);
        void step();
        void printStatus();
        const vector<Facility*> &getFacilities() const;
        const vector<Facility*> &getUnderConstruction() const;
        void addFacility(Facility* facility);
        const string toString() const;
        const string getSettlementName() const;
        bool isAvailable ();
        const string getSelectionPolicyString() const;
        int getPlanId() const;
        SelectionPolicy* getSelectionPolicy() const;


    private:
        int plan_id;
        const Settlement &settlement;
        int construction_cap;
        SelectionPolicy *selectionPolicy; //What happens if we change this to a reference?
        PlanStatus status;
        vector<Facility*> facilities;
        vector<Facility*> underConstruction;
        const vector<FacilityType> &facilityOptions;
        int life_quality_score, economy_score, environment_score;
};