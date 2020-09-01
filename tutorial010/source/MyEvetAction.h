#pragma once
#include "G4UserEventAction.hh"

class MyEventAction : public G4UserEventAction{
    
    virtual void BeginOfEventAction(const G4Event* anEvent) override;
    virtual void EndOfEventAction(const G4Event* anEvent) override;
};