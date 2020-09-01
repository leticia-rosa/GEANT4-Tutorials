#   Define an environment variable G4INCLUDES that points to 
# your installation path, where the includes are. It is usually
# something like: /path/to/geant4-install/include/Geant4

g++ -o tutorial010 \
    main.cpp \
    source/MyDetector.cpp\
    source/MyPrimaryGenerator.cpp \
    source/MyActioninitialization.cpp \
    source/MySensorDetector.cpp \
    source/MyHit.cpp \
    source/MyRunAction.cpp \
    source/MyEventAction.cpp \
    `geant4-config --libs` -I${G4INCLUDES} 
     
