#include "MyPrimaryGenerator.h"

#include "G4Geantino.hh"

MyPrimaryGenerator::MyPrimaryGenerator() :
     G4VUserPrimaryGeneratorAction(),
     m_particleGun(new G4ParticleGun( G4Geantino::Definition() ) ) {

};

MyPrimaryGenerator::~MyPrimaryGenerator(){
    delete m_particleGun;
};

void MyPrimaryGenerator::GeneratePrimaries(G4Event* anEvent){

    int np = 5; // number of primaries
    for(int i=0; i<np; i++) m_particleGun->GeneratePrimaryVertex(anEvent);

};