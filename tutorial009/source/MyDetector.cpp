#include "MyDetector.h"

#include "G4Box.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"

#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"

#include "G4VisAttributes.hh"

#include "MySensorDetector.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "G4MaterialPropertiesTable.hh"

using namespace CLHEP;


G4VPhysicalVolume* MyDetector::Construct(){

    const int N_DATA = 2;
    double photonEnergies[N_DATA] = {1.65*eV, 3.10*eV};


    auto air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    auto airPropertiesTable = new G4MaterialPropertiesTable();
    air->SetMaterialPropertiesTable( airPropertiesTable );
    
    // Adding Refraction and Reflection to the materials

    // adding index of refraction to the air
    double airRIndex[N_DATA] = {1.0, 1.0};
    airPropertiesTable->AddProperty("RINDEX", photonEnergies, airRIndex, N_DATA);

    auto water = G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER");
    auto waterPropertiesTable = new G4MaterialPropertiesTable();
    water->SetMaterialPropertiesTable( waterPropertiesTable );
    
    // adding index of refraction to the water
    double waterRIndex[N_DATA] = {1.33, 1.33};
    waterPropertiesTable->AddProperty("RINDEX", photonEnergies, waterRIndex, N_DATA);

    double waterAbsLength[N_DATA]= {10*m, 10*m};
    waterPropertiesTable->AddProperty("ABSLENGTH", photonEnergies, waterAbsLength, N_DATA);

    auto glass = G4NistManager::Instance()->FindOrBuildMaterial("G4_GLASS_PLATE");
    auto glassPropertiesTable = new G4MaterialPropertiesTable();
    glass->SetMaterialPropertiesTable( glassPropertiesTable );

    // adding index of refraction to the glass
    double glassRIndex[N_DATA] = {2.50, 2.50}; // index of refraction
    glassPropertiesTable->AddProperty("RINDEX", photonEnergies, glassRIndex, N_DATA);

    // adding length threshold for absorption/killing photons
    double glassAbsLength[N_DATA] = {50*cm, 50*cm}; // average length that a photon travels before being absorved/dies
    glassPropertiesTable->AddProperty("ABSLENGTH", photonEnergies, glassAbsLength, N_DATA);

    // World Volume
    auto worldBox = new G4Box("worldBox", 0.5*m, 0.5*m, 0.5*m);
    auto logicalWorld = new G4LogicalVolume(worldBox, air, "LogicalWorld");
    auto physicalWorld = new G4PVPlacement(0, {0,0,0}, logicalWorld, "World", 0, false, 0);
    logicalWorld->SetVisAttributes( new G4VisAttributes( false )); //makes world volume invisible in geometry

    // Water Box
    double sideWaterBox = 20.0*cm;
    auto waterBox = new  G4Box("SideWaterBox", sideWaterBox/2, sideWaterBox/2, sideWaterBox/2);
    auto logWater = new G4LogicalVolume(waterBox, water, "Logical Water Box");
    auto physWater = new G4PVPlacement(0, {0,0,0}, logWater, "World/WaterBox", logicalWorld, false, 0);
    logWater->SetVisAttributes( new G4VisAttributes( G4Colour::Blue() ));

    // Glass Box
    double sideGlassBox = 10.0*cm;
    auto glassBox = new  G4Box("glassBox", sideGlassBox/2, sideGlassBox/2, sideGlassBox/2);
    auto logGlass = new G4LogicalVolume(glassBox, glass, "Logica Glass Box");
    auto physGlass = new G4PVPlacement(0, {0,0,0}, logGlass, "World/WaterBox/GlassBox", logWater, false, 0);
    logGlass->SetVisAttributes( new G4VisAttributes( G4Colour::Cyan() ));
    // Optical properties of the surfaces

    // Air Water Optical Properties
    auto surfaceAirWater = new G4OpticalSurface("Air/Water Surface");
    surfaceAirWater->SetModel(unified);
    surfaceAirWater->SetType(dielectric_dielectric);
    surfaceAirWater->SetFinish(polished);

    auto surfaceAirWaterProp = new G4MaterialPropertiesTable();
    surfaceAirWater->SetMaterialPropertiesTable(surfaceAirWaterProp);

    // Interface
    auto logSurfaceAirWater = new G4LogicalBorderSurface("Air->Water Interface", physicalWorld, physWater, surfaceAirWater);
    auto logSurfaceWaterAir = new G4LogicalBorderSurface("Water->Air Interface", physWater, physicalWorld, surfaceAirWater);

    // Probability of being absorbed in the interface (polarization of photons -macro- also interferes)
    double reflectAirWater[N_DATA] = {0.7, 0.7}; // reflectivity de 0 a 1 (%): refletividade = 1 - absorbância
    surfaceAirWaterProp->AddProperty("REFLECTIVITY", photonEnergies, reflectAirWater, N_DATA);

    // Water Glass Optical Properties
    auto surfaceWaterGlass = new G4OpticalSurface("Water/Glass Surface");
    surfaceWaterGlass->SetModel(unified);
    surfaceWaterGlass->SetType(dielectric_dielectric); // dielectric_metal: 100% reflection - mirror or (dielectric_dielectric)
    surfaceWaterGlass->SetFinish(ground); // polished or ground (rugoso)
    surfaceWaterGlass->SetSigmaAlpha(1*degree); // for ground finish only; sigmaAlpha is the gaussian width in rad(the greater sigma the greater is the ground)

    auto surfaceWaterGlassProp = new G4MaterialPropertiesTable();
    surfaceWaterGlass->SetMaterialPropertiesTable(surfaceWaterGlassProp);

    // Interface
    auto logSurfaceWaterGlass = new G4LogicalBorderSurface("Water->Glass Interface", physWater, physGlass, surfaceWaterGlass);
    auto logSurfaceGlassWater = new G4LogicalBorderSurface("Glass->Water Interface", physGlass, physWater, surfaceWaterGlass);

    // Probability of being absorbed in the interface (polarization of photons -macro- also interferes)
    double reflectWaterGlass[N_DATA] = {1, 1}; // 1 means no absorption at all (abs = 1 - reflect), photon gets trapped inside (we can kill them with abs length - see above line 58)
    surfaceWaterGlassProp->AddProperty("REFLECTIVITY", photonEnergies, reflectWaterGlass, N_DATA);

    // Sensitive detector

    // defining material silicon for the detector
    auto silicon = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");
    auto siliconPropertiesTable = new G4MaterialPropertiesTable();
    silicon->SetMaterialPropertiesTable( siliconPropertiesTable );
    
    double siliconRIndex[N_DATA] = {1, 1}; 
    siliconPropertiesTable->AddProperty("RINDEX", photonEnergies, glassRIndex, N_DATA);
    
    // killing photons when they reach the detector (they are absorved)
    double siliconAbsLength[N_DATA] = {1*nm, 1*nm}; 
    siliconPropertiesTable->AddProperty("ABSLENGTH", photonEnergies, glassAbsLength, N_DATA);

    double sensorDim = 0.5*m;
    double sensorThickness = 1*mm;
    double sensorDistance = 5*cm;
    auto boxSensor = new  G4Box("SensorBox", sensorDim/2, sensorThickness/2, sensorDim/2);
    auto logSensor = new G4LogicalVolume(boxSensor, silicon, "Logical Sensor");
    auto phySensor = new G4PVPlacement(0, {0,-(sideWaterBox/2+sensorThickness/2)-sensorDistance,0}, logSensor, "World/Sensor", logicalWorld, false, 0);
    logSensor->SetVisAttributes( new G4VisAttributes( G4Colour::Red() ));
    
    logSensor->SetSensitiveDetector(new MySensorDetector("PhotonCounter"));

    return physicalWorld;
}