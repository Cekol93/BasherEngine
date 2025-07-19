#include "engine.h"
#include "enginePrefetcher.h"
#include "engineProfiler.h"
#include "engineDebugInfo.h"

enginePrefetcher* localPrefetcher = NULL;
enginePrefetcher* M_getPrefetcher() { return localPrefetcher; }

enginePrefetcher::enginePrefetcher()
{
    localPrefetcher = this;
    M_getEngine()->registerModule(this, "Engine Prefetcher");

    modules.reserve(10240);
    modules.clear();

    controllers.reserve(1024);
    controllers.clear();

    initialization = true;
}

enginePrefetcher::~enginePrefetcher() {

}

void enginePrefetcher::addController(threadController* C) {
    controllers.push_back(C);
};
void enginePrefetcher::addModule(module* M) {
    modules.push_back(M);
};
void enginePrefetcher::compute() {

   // cout << "Prefetcher:" << modules.size() << endl;
    //std::this_thread::sleep_for(chrono::milliseconds(2000));
    initialization = false;
    sort(modules.begin(), modules.end(), moduleSortMilisecondsClass()); // lags, lower fps...

    //1.SPLITTING JOB
    int i, j;
    int conselect = 0;

    for (i = 0; i < controllers.size(); i++)
        controllers[i]->computeMilisecondsPresumed = 0.0;

    for (i = 0; i < modules.size(); i++)
    {
        long double lowestMS = 5000000.0f;
        module* M = (module*)modules[i];

        //M->lastAllignCore;
        //M->requestChangeCore;
        // M->outtakeComputeMiliseconds;





        for (j = 0; j < controllers.size(); j++) // JOB SPLITTING
            if (controllers[j]->computeMilisecondsPresumed < lowestMS)
            {
                conselect = j;   lowestMS = controllers[j]->computeMilisecondsPresumed;
            }

            
        //if (M->lastAllignCore != -1)
        //    conselect = M->lastAllignCore;
        //    
        //if (M->lastAllignCore == -1)
        //    M->lastAllignCore = conselect;


        controllers[conselect]->addModule(M);
        controllers[conselect]->computeMilisecondsPresumed += M->outtakeComputeMiliseconds;

    }
    
    i = 0;
    j = 0;
    if (M_getProfiler()) {
        for (i = 0; i < min((int)15,(int)modules.size()); i++) { 
            module* M = (module*)modules[i];
            M_getProfiler()->addModule(M, M->outtakeComputeMiliseconds);
        }
        //cout << " CONTROLLERS: " << controllers.size() << endl;
        for (j = 0; j < controllers.size(); j++) 
            M_getProfiler()->addCore(j + 1, controllers[j]->computeMiliseconds, controllers[j]->intakeModulesCount);
    }

    if (M_getDebugInfo()) // Grabbing modules debug info...
        if (M_getDebugInfo()->visible) {
            vector < module* > modulesByName;
            modulesByName.clear();
            for (i = 0; i < modules.size(); i++) {
                module* M = (module*)modules[i];
                if (M->getDebugMessage().length() > 0)
                    modulesByName.push_back(M);
            }
            sort(modulesByName.begin(), modulesByName.end(), moduleSortNameClass()); // lags, lower fps...
            for (i = 0; i < modulesByName.size() ; i++) {
                module* M = (module*)modulesByName[i];
                M_getDebugInfo()->addDebugMessage("#0" + to_string(i + 1) + ".(" + M->moduleName + ") " + M->getDebugMessage() + "#e");
                //cout << i << ". " << M->moduleName  << "/" << M->getDebugMessage() << endl;
            }
            modulesByName.clear();
        }
    controllers.clear();
    modules.clear();
}

void enginePrefetcher::load() {

}
void enginePrefetcher::synchro() {

}
void enginePrefetcher::work() {
    if (!initialization)
        compute();
}
void enginePrefetcher::draw() {

}

