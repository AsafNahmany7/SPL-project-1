#include "SelectionPolicy.h"
#include <vector>
#include <iostream>
#include <cstdlib>
using std::vector;
#include <limits>
#include <algorithm>


NaiveSelection::NaiveSelection():lastSelectedIndex(-1), numberOfFacilities(0), builtFacilitiesList("Built Facilities list:"){}

const FacilityType& NaiveSelection::selectFacility(const vector<FacilityType>& facilitiesOptions){
    numberOfFacilities++;

    if((lastSelectedIndex + 1) < static_cast<int>(facilitiesOptions.size())){
       const FacilityType& output_option1 = facilitiesOptions[lastSelectedIndex + 1];
       lastSelectedIndex++;
       builtFacilitiesList += "\n" + std::to_string(numberOfFacilities) + ". "  + output_option1.getName(); 

       return output_option1;
    }
    else{
         const FacilityType& output_option2 = facilitiesOptions[0];
         lastSelectedIndex = 0;
         builtFacilitiesList += "\n" +std::to_string(numberOfFacilities) + ". " + output_option2.getName(); 

         return output_option2;
    }


}

const string  NaiveSelection::toString() const{
    return builtFacilitiesList;
}

NaiveSelection* NaiveSelection::clone() const{
    NaiveSelection* clone = new NaiveSelection();
    clone->lastSelectedIndex = this->lastSelectedIndex;
    clone->numberOfFacilities = this->numberOfFacilities;
    clone->builtFacilitiesList = this->builtFacilitiesList;

    return clone;
}

BalancedSelection::BalancedSelection(int LifeQualityScore, int EconomyScore, int EnvironmentScore):
LifeQualityScore(LifeQualityScore),
EconomyScore(EconomyScore),
EnvironmentScore(EnvironmentScore),
numberOfFacilities(0),
builtFacilitiesList("Built Facilities list:")
{}

const FacilityType& BalancedSelection:: selectFacility(const vector<FacilityType>& facilitiesOptions){
    numberOfFacilities++;
   
    const FacilityType* current = &facilitiesOptions[0];
    int firstadd_lifeq_score  = LifeQualityScore + current->getLifeQualityScore();
    int firstadd_eco_score = EconomyScore + current->getEconomyScore();
    int firstadd_envo_score = EnvironmentScore + current->getEnvironmentScore();

    int a = abs(firstadd_lifeq_score - firstadd_eco_score);
    int b = abs(firstadd_lifeq_score - firstadd_envo_score);
    int c = abs(firstadd_eco_score - firstadd_envo_score);
    
    int minimalDistance = std::max({a, b, c});
    
    if (minimalDistance == 0){
        return *current;
    }

    for (const FacilityType& facility : facilitiesOptions) {
        int secondAdd_lifeq_score  = LifeQualityScore + facility.getLifeQualityScore();
        int SecondAdd_eco_score = EconomyScore + facility.getEconomyScore();
        int SecondAdd_envo_score = EnvironmentScore + facility.getEnvironmentScore();

        int x = abs( secondAdd_lifeq_score -  SecondAdd_eco_score);
        int y = abs( secondAdd_lifeq_score -  SecondAdd_envo_score);
        int z = abs( SecondAdd_eco_score -  SecondAdd_envo_score);
       

        int currentdistance = std::max({x, y, z});


        if (currentdistance == 0){
            return facility;
        }

        if(minimalDistance > currentdistance){
            minimalDistance = currentdistance;
            current = &facility;
        } 

    }
    LifeQualityScore += current->getLifeQualityScore();
    EnvironmentScore += current->getEnvironmentScore();
    EconomyScore += current->getEconomyScore();

    return *current;
}


const string BalancedSelection:: toString() const{
    return builtFacilitiesList;

}

BalancedSelection* BalancedSelection::clone() const{
    BalancedSelection* clone = new BalancedSelection(LifeQualityScore,EconomyScore,EnvironmentScore);
    clone->builtFacilitiesList = this->builtFacilitiesList;
    clone->numberOfFacilities = this->numberOfFacilities;
    
    return clone;
}


EconomySelection::EconomySelection():lastSelectedIndex(-1),numberOfFacilities(0),builtFacilitiesList("Built Facilities list:"){}

const FacilityType& EconomySelection::selectFacility(const vector<FacilityType>& facilitiesOptions){

    numberOfFacilities++;
    int i;
    bool inside_start = false;


    if(lastSelectedIndex  ==-1 || lastSelectedIndex + 1 == static_cast<int>(facilitiesOptions.size())){
        i = 0; 
    }

    else{
        i = lastSelectedIndex + 1;
        inside_start = true;
    }

    
    const FacilityType* current = & facilitiesOptions[i];

        while(current->getCategory()!= FacilityCategory::ECONOMY){
            if(!inside_start){
                i++;
                current = &facilitiesOptions[i];
            }
            else{
                if(i == (static_cast<int>(facilitiesOptions.size()) - 1)){
                    i = 0;
                }
                else{
                    i++;
                }
                current = &facilitiesOptions[i];

            }


    }

    lastSelectedIndex = i;

    builtFacilitiesList += "\n" + std::to_string(numberOfFacilities) + ". "  + current->getName();
    return *current;

    }

    const string EconomySelection::toString() const{
        return builtFacilitiesList;
    }

    EconomySelection* EconomySelection::clone()const {
        EconomySelection* clone = new EconomySelection();
        clone->builtFacilitiesList = this->builtFacilitiesList;
        clone->lastSelectedIndex = this->lastSelectedIndex;
        clone->numberOfFacilities = this->numberOfFacilities;

        return clone;

    }

SustainabilitySelection::SustainabilitySelection():lastSelectedIndex(-1),numberOfFacilities(0),builtFacilitiesList("Built Facilities list:"){}

const FacilityType& SustainabilitySelection::selectFacility(const vector<FacilityType>& facilitiesOptions){

    numberOfFacilities++;
    int i;
    bool inside_start = false;


    if(lastSelectedIndex == -1 || lastSelectedIndex + 1 == static_cast<int>(facilitiesOptions.size())){
        i = 0; 
    }

    else{
        i = lastSelectedIndex + 1;
        inside_start = true;
    }

    
    const FacilityType* current = & facilitiesOptions[i];

        while(current->getCategory()!= FacilityCategory::ENVIRONMENT){
            if(!inside_start){
                i++;
                current = &facilitiesOptions[i];
            }
            else{
                if(i == (static_cast<int>(facilitiesOptions.size()) - 1)){
                    i = 0;
                }
                else{
                    i++;
                }
                current = &facilitiesOptions[i];

            }


    }


    builtFacilitiesList += "\n" + std::to_string(numberOfFacilities) + ". "  + current->getName();
    lastSelectedIndex = i;

    return *current;

    }

     const string SustainabilitySelection::toString() const{
        return builtFacilitiesList;
    }


     SustainabilitySelection* SustainabilitySelection::clone()const {
        SustainabilitySelection* clone = new SustainabilitySelection();
        clone->builtFacilitiesList = this->builtFacilitiesList;
        clone->lastSelectedIndex = this->lastSelectedIndex;
        clone->numberOfFacilities = this->numberOfFacilities;

        return clone;

    }
