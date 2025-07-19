#pragma once
#ifndef ENGINE_PREFETCHER_H
#define ENGINE_PREFETCHER_H

#include "engine.h"

class moduleSortMilisecondsClass {
public:
    bool operator () (const module* m1, const  module* m2)
    {
        return (m1->outtakeComputeMiliseconds <= m2->outtakeComputeMiliseconds) ? false : true;
    }
};

class moduleSortNameClass {
public:
    bool operator () (const module* m1, const  module* m2)
    {
        if (m1->moduleName.compare(m2->moduleName) == 0) // if names are equal...
            return (m1->moduleId > m2->moduleId); // return difference in id.. (older are on bottom of list...)
        else
            return (m1->moduleName < m2->moduleName);
    }
};

class enginePrefetcher : public module
{
public:
    enginePrefetcher();
    ~enginePrefetcher();
    void load();
    void synchro();
    void work();
    void draw();

    void addController(threadController* C);
    void addModule(module* M);

    void compute();

    std::vector < module* > modules; // modules to sort and add to controllers
    std::vector < threadController* > controllers; // controllers to fill with modules

    bool initialization;


    // PROFILER VARIABLES...
    //bool active; // is profiler opened...

};
enginePrefetcher* M_getPrefetcher();

#endif 