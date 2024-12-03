#include "Simulation.h"
#include <iostream>

using namespace std;

Simulation* backup = nullptr;

int main(int argc, char** argv){
    cout << "hello shoham !" << endl;
    if(argc!=2){
        cout << "usage: simulation <config_path>" << endl;
        return 0;
    }
    string configurationFile = argv[1];
    Simulation simulation(configurationFile);
    simulation.start();
    PrintPlanStatus prin(0);
    prin.act(simulation);
    simulation.step();
    simulation.step();
    simulation.step();
    simulation.step();
    simulation.step();
    simulation.step();
    simulation.step();
    simulation.step();
    simulation.step();
    simulation.step();
    simulation.step();
    simulation.step();
    simulation.step();
    simulation.step();
    simulation.step();
    simulation.step();
    prin.act(simulation);
    
    if(backup!=nullptr){
    	delete backup;
    	backup = nullptr;
    }
    return 0;
}