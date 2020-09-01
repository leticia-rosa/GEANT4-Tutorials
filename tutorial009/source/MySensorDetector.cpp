#include "MySensorDetector.h"
#include <iostream>
#include <iomanip>  //algarismos significativos
#include "G4UnitsTable.hh"

#include "MyHit.h"

MySensorDetector::MySensorDetector(const G4String &name) : G4VSensitiveDetector(name)   {
    ;

}

MySensorDetector::~MySensorDetector(){
    ;

}

G4bool MySensorDetector::ProcessHits(G4Step* aStep, G4TouchableHistory* tHist) {
    if(aStep->GetTotalEnergyDeposit() != 0){
        
      auto aHit = new MyHit(aStep);
      aHit->Print();
      delete aHit;
    }
}
