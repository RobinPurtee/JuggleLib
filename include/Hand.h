#pragma once
#include <list>
#include <deque>
class Throw;
class Prop;


class Hand
{
public:

    enum class State:int { VACANT = 0, DWELL, TOSS, CATCH };

    Hand(int id);
    ~Hand(void);

    int getId();

    Hand::State getState();
     /** 
     * Get the string of the current state name
     */
    const char* getStateName();


    void Pickup(Prop* prop);

    virtual void Toss(Throw* toss);
    virtual void Release();

    bool isVacant();

    void Catch(Prop* prop);
    void Caught(Prop* prop);

    void Collision(Prop* prop);

    std::string toString();

protected:

private:


    struct HandStateMachine;
    std::shared_ptr<HandStateMachine> stateMachine_;

};

