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
    const TCHAR* getStateName();


    void Pickup(Prop* prop);

    virtual void Toss(Throw* toss);
    virtual void Release();

    bool isVacant();

    void Catch(Prop* prop);
    void Caught(Prop* prop);

    void Collision(Prop* prop);

#if defined(_DEBUG)
    std::wstring toString();
#endif // defined(_DEBUG)

protected:

private:

    std::deque<Prop*> props_;
    Throw* toss_;
    int id_;
    Prop* propCatching_;

    struct HandStateMachine;
    std::shared_ptr<HandStateMachine> stateMachine_;

};

