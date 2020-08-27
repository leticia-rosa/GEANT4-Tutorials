#include <iostream>

#include "G4RunManager.hh"


// Creating the detector

#include "G4VUserDetectorConstruction.hh"
#include "G4Box.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

class MyDetector : public G4VUserDetectorConstruction {

public:
    virtual G4VPhysicalVolume* Construct() override;

};

G4VPhysicalVolume* MyDetector::Construct(){
	//Cube filled with liquid argon

	G4Material *lAr = G4NistManager::Instance()->FindOrBuildMaterial("G4_lAr"); //material
	auto *worldBox = new G4Box("worldBox", 10000, 10000, 10000); //geometry
	auto *logicalWorld = new G4LogicalVolume(worldBox, lAr, "Logical World"); //logical world links geometry and material
	auto *physicalWorld = new G4PVPlacement(0, {0,0,0}, logicalWorld, "Physical World", 0, false, 0);

	return physicalWorld;
}


// The Physics List

#include "G4VUserPhysicsList.hh"
#include "G4PhysListFactory.hh"

// The action initialization (particle source)

#include "G4VUserActionInitialization.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4Electron.hh"

class MyPrimaryGenerator : public G4VUserPrimaryGeneratorAction{
public:
    virtual void GeneratePrimaries(G4Event* anEvent) override;	
};

void MyPrimaryGenerator::  GeneratePrimaries(G4Event* anEvent){

	auto *particleGun = new G4ParticleGun(G4Electron::Definition());
	particleGun->GeneratePrimaryVertex(anEvent);

	std::cout << "Run ID " << anEvent->GetEventID() << std::endl;
	std::cout << "Primary " << particleGun->GetParticleDefinition()->GetParticleName() << std::endl;
	std::cout << "Energy (MeV) " << particleGun->GetParticleEnergy() << std::endl;
}




class MyActionInitialization : public G4VUserActionInitialization{
public:
	virtual void Build() const override;	
};

void MyActionInitialization ::Build() const {

	SetUserAction( new MyPrimaryGenerator);	
}

// Main

int main(){

	G4RunManager *manager = new G4RunManager();

	auto *factory = new G4PhysListFactory();
	auto *physicsList = factory->GetReferencePhysList("Shielding");


	manager->SetUserInitialization( new MyDetector() );
	manager->SetUserInitialization( physicsList );
	manager->SetUserInitialization( new MyActionInitialization() );
	manager->Initialize();

	manager->BeamOn(100);

	delete manager;
	delete factory;
}
