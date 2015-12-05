#include "pch.h"
#include "Hand.h"


Hand::Hand(void)
: state(VACANT)
{
}


Hand::~Hand(void)
{
}


void Hand::AddProp(Prop* prop)
{
    if(nullptr != prop)
    {
        propQue.push_back(prop);
    }
}

void Hand::AddPass(const Pass* pass)
{
    passList.push_back(*pass);
}

void Hand::AddPasses(PassList::iterator begin, PassList::iterator end)
{
    std::for_each(begin, end,[this](Pass pass){passList.push_back(pass);});
}


void Hand::InComming()
{

}

void Hand::Toss()
{
    if(State::VACANT == state || propQue.empty())
    {
        throw new std::exception();
    }

    Prop* prop = *propQue.begin();
    propQue.pop_front();
    prop->Toss(&*currentPass);
    ++currentPass;
    if(passList.end() == currentPass)
    {
        currentPass = passList.begin();
    }
}

void Hand::Catch(Prop* prop)
{
    prop->Catch();
    propQue.push_back(prop);
    state = State::CATCH;
}

void Hand::Count()
{
    ++currentPass;
    if(nullptr != &*currentPass)
    {
        Toss();
    }
}
