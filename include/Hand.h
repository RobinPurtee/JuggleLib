#pragma once
#include <list>
#include <deque>
class Throw;
class Prop;


class Hand
{
public:

    //enum State { VACANT, CATCH, DWELL, TOSS };

    Hand(int id);
    ~Hand(void);


    void Pickup(Prop* prop);

    virtual void Toss(Throw* toss);
    virtual void Release();

    bool isVacant();

    virtual void Catch(Prop* prop);

private:

    struct HandStateMachine;
    std::shared_ptr<HandStateMachine> stateMachine_;

};

