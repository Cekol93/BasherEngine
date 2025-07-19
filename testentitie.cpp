#include "testentitie.h"
#include "engine.h"
#include "simplesprite.h"
#include "render.h"


testentitie::testentitie()
{
    get_engine()->register_module( this , "TestEntitie" );
}

testentitie::~testentitie()
{

}
void testentitie::synchro() {
        if (!visible) return;
        if (get_render()) get_render()->add_module(this , 0 );
}
void testentitie::work() {

}
void testentitie::draw() {


}
