#include "MyActioninitialization.h"

#include "MyPrimaryGenerator.h"
// The User's Actions
#include "MyRunAction.h"
#include "MyEvetAction.h"

void MyActionInitialization::Build() const {
    SetUserAction( new MyPrimaryGenerator );
    SetUserAction(new MyRunAction()); //in MyActionInitialization
    SetUserAction(new MyEventAction());
};