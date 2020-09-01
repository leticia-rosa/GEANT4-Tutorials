#pragma once  //para proteger o arquivo e garantir que o arquivo nao vai ser inserido multiplas vezes

// The Detector
#include "G4VUserDetectorConstruction.hh"


class MyDetector : public G4VUserDetectorConstruction {
public:
    virtual G4VPhysicalVolume* Construct() override;
};

