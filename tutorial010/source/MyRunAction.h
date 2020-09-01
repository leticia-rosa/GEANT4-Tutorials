#pragma once
#include "G4UserRunAction.hh"

class MyRunAction : public G4UserRunAction{
    
    virtual void BeginOfRunAction(const G4Run* aRun) override;
    virtual void EndOfRunAction(const G4Run* aRun) override;
};