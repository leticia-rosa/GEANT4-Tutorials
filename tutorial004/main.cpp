#include <iostream>

// The runManager
#include "G4RunManager.hh"

// The detector
#include "source/MyDetector.h"

// The Action Initialization (particle source)
#include "source/MyActioninitialization.h"


// The Physics List
#include "G4VUserPhysicsList.hh"
#include "G4PhysListFactory.hh"
// We are going to import a pre-made physics list called "Shielding"
// You can find other options at https://geant4.web.cern.ch/node/302




// This is the User Interface
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"


#define G4VIS_USE_OPENGLX
#define G4VIS_USE_OPENGLQT

// This is the vizualization engine
#include "G4VisExecutive.hh"
#include "G4VisManager.hh"



// This is the Main code.
int main(int argc, char** argv){

    auto *runManager = new G4RunManager();

    auto *factory = new G4PhysListFactory();
    auto *physicsList = factory->GetReferencePhysList("Shielding");

    runManager->SetUserInitialization( new MyDetector() );
    runManager->SetUserInitialization( physicsList );
    runManager->SetUserInitialization( new MyActionInitialization() );
    runManager->Initialize();

    auto *visManager = new G4VisExecutive();
    visManager->Initialise();

    auto *uiExecutive = new G4UIExecutive(argc,argv, "Qt");
		


	if (argc == 1){
		uiExecutive->SessionStart();
	} else {
		auto *uiManager = G4UImanager::GetUIpointer();
		uiManager->ApplyCommand("/control/execute " + G4String(argv[1]) );
        uiExecutive->SessionStart();
	} 

    delete runManager; // The runrunManager will delete all other pointers owned by it.
   // delete uiExecutive;
	delete uiExecutive;
    delete visManager;
};