#include <iostream>

// The runManager
#include "G4RunManager.hh"

// The Detector
#include "G4VUserDetectorConstruction.hh"
#include "G4Box.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"

class MyDetector : public G4VUserDetectorConstruction {
public:
    virtual G4VPhysicalVolume* Construct() override;
};

G4VPhysicalVolume* MyDetector::Construct(){

    // This is just a Liquid-Argon filled box
    auto lAr = G4NistManager::Instance()->FindOrBuildMaterial("G4_lAr");
    auto worldBox = new G4Box("worldBox", 10000, 10000, 10000);
    auto logicalWorld = new G4LogicalVolume(worldBox, lAr, "Logical World");
    auto physicalWorld = new G4PVPlacement(0, {0,0,0}, logicalWorld, "Physical World", 0, false, 0);

    return physicalWorld;
}

// The Physics List
#include "G4VUserPhysicsList.hh"
#include "G4PhysListFactory.hh"
// We are going to import a pre-made physics list called "Shielding"
// You can find other options at https://geant4.web.cern.ch/node/302

// The Action Initialization (particle source)
#include "G4VUserActionInitialization.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4Geantino.hh"

class MyPrimaryGenerator : public G4VUserPrimaryGeneratorAction{

private:
G4ParticleGun *m_particleGun;

public:

    MyPrimaryGenerator(); // construtor
    ~MyPrimaryGenerator(); // destrutor
    
    virtual void GeneratePrimaries(G4Event* anEvent) override;
};

MyPrimaryGenerator::MyPrimaryGenerator() :
     G4VUserPrimaryGeneratorAction(),
     m_particleGun(new G4ParticleGun( G4Geantino::Definition() ) ) {

};

MyPrimaryGenerator::~MyPrimaryGenerator(){
    delete m_particleGun;
};

void MyPrimaryGenerator::GeneratePrimaries(G4Event* anEvent){


    m_particleGun->GeneratePrimaryVertex(anEvent);

	std::cout << "Run ID " << anEvent->GetEventID() << std::endl;
	std::cout << "Primary " << m_particleGun->GetParticleDefinition()->GetParticleName() << std::endl;
	std::cout << "Energy (MeV) " << m_particleGun->GetParticleEnergy() << std::endl;

};

class MyActionInitialization : public G4VUserActionInitialization{
public:
    virtual void Build() const override;
};

void MyActionInitialization::Build() const {
    SetUserAction( new MyPrimaryGenerator );
};

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