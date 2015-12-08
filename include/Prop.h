#pragma once
#include <tchar.h>
#include <memory>
struct Pass;
class Hand;

class IPropResponder
{
public:
    virtual void Tossed(int id) = 0;
    virtual void Catch(int id, Hand* destination) = 0;
    virtual void Dropped(int id) = 0;
};

/// This is a base class for an object being juggled
class Prop
{
public:
    enum State {DWELL = 0, FLIGHT = 1, CATCH = 2, TOSS = 3, DROPPED};
         
    Prop(int id_, IPropResponder* responder);
    virtual ~Prop(void);

    //Properties
    int get_id()        {return id;}

    /**
     *  Get the current state of the Prop
     */
    Prop::State getState();

    /** 
     * Get the string of the current state name
     */
    const TCHAR* getStateName();
    
    /**
     * The current count away from the next hand
     * @return If the state is Flight then the number of counts away from its 
     *         destination Hand, else the value is 0;
     * @remark The value is set by the Toss and decremented each time Count is called 
     */
    int getCurrentSwap();

    /**
     * 
     */

    virtual void Toss(const Pass* pass);

    virtual void Catch();

    virtual void Collision();

    virtual void Pickup();

    virtual void Tick();

private:
    struct PropStateMachine;
    std::shared_ptr<PropStateMachine> stateMachine;
    int id;

};

typedef std::shared_ptr<Prop> PropPtr;
