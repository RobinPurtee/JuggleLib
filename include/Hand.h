#pragma once
#include <list>
#include <deque>
class Throw;
class Prop;


class Hand
{
public:

    //enum State { VACANT, CATCH, DWELL, TOSS };

    Hand(int id_);
    ~Hand(void);

    void Pickup(Prop* prop_);

    virtual void Toss(Throw* toss_);

    virtual void Catch(Prop* prop_);

private:

    struct HandStateMachine;
    std::shared_ptr<HandStateMachine> stateMachine;

};

