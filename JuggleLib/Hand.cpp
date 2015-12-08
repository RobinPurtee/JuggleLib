#include "pch.h"
#include "Hand.h"
#include "Pass.h"
#include "Prop.h"


Hand::Hand(void)
: state(VACANT)
{
}


Hand::~Hand(void)
{
}


void Hand::Pickup(Prop* prop)
{
    if(nullptr != prop)
    {
        propQue.push_back(prop);
    }
}


void Hand::Toss(const Pass& pass)
{
    if(State::VACANT == state || propQue.empty())
    {
        throw new std::exception();
    }

    Prop* prop = *propQue.begin();
    propQue.pop_front();
    prop->Toss(&pass);
}

void Hand::Catch(Prop* prop)
{
    prop->Catch();
    propQue.push_back(prop);
    state = State::CATCH;
}

void Hand::Tick()
{
}
