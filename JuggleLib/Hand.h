#pragma once
#include <list>
#include <deque>
#include "Pass.h"
class Prop;


typedef std::list<Pass> PassList; 

class Hand
{
public:

    enum State { VACANT, CATCH, DWELL, TOSS };

    Hand(void);
    ~Hand(void);

    void AddProp(Prop* prop);

    void AddPass(const Pass* pass);

    void AddPasses(PassList::iterator begin, PassList::iterator end);


    void InComming();

    virtual void Toss();

    virtual void Catch(Prop* prop);

    virtual void Count();
private:

    Hand::State state;

    // the props currently held by the hand
    std::deque<Prop*> propQue;
    // the list of passes to cycle through 
    PassList passList;
    PassList::iterator currentPass;

};

