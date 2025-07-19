#include "dummyLoad.h"
#include "engine.h"
#include <math.h>


dummyLoad::dummyLoad()
{
    M_getEngine()->registerModule(this, "DummyLoad");
}

dummyLoad::~dummyLoad() {

}


void dummyLoad::load() {

}

void dummyLoad::synchro() {

}

void dummyLoad::work() {

    long double x = 0;
    //for (unsigned int k = 0; k < 429496729; k++)
    //for (unsigned int j = 0; j < 429496729; j++)
    for (unsigned int i = 0; i < 4224; i++) {
        int a = sin(cos(tan(sin(i))));
        int b = sin(cos(tan(cos(tan(i-1)))));
        int c = tan( sin(cos(sin(cos(sin(a)))))+ sin(cos(sin(cos(sin(b))))) )+i;
        int d = b - a;
        int e = c + d + i;
        x += (float)e * 0.01f;
    }
    //cout << x << endl;
    x++;
    debugMessage+=to_string(x);
    
}

void dummyLoad::draw() {

}

