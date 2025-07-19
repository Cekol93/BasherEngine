#include "engine.h"

int main(int argc, char** argv)
{
    engine* E = new engine();
    E->init(argc, argv);
    E->loop();
    E->destroy();
    delete E;
    // Return success
    return 1;
}