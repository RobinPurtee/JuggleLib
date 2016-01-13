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
    static const char* getStateName(Hand::State state);


    void Pickup(Prop* prop);

    virtual void Toss(Throw* toss);
    virtual void Release();

    bool isVacant();

    void Catch(Prop* prop);
    void Caught();

    void Collision(Prop* prop);
    void Collect();

    virtual std::string toString();
    virtual std::wstring toWstring();

protected:
    Prop* propCatching_;
    // the following is a debugging function
    int getNumberOfProps()  {return props_.size();}
private:

    std::deque<Prop*> props_;
    Throw* toss_;
    int id_;

    struct HandStateMachine;
    std::shared_ptr<HandStateMachine> stateMachine_;

};

