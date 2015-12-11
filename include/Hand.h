#pragma once
#include <list>
#include <deque>
struct Throw;
class Prop;


class Hand
{
public:

    enum State { VACANT, CATCH, DWELL, TOSS };

    Hand(void);
    ~Hand(void);

    void Pickup(Prop* prop);


    virtual void Toss(const Throw& pass);

    virtual void Catch(Prop* prop);

    virtual void Tick();
private:

    Hand::State state;

    // the props currently held by the hand
    std::deque<Prop*> propQue;
    // the list of passes to cycle through 

};

