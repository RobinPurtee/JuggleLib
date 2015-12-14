#pragma once
#include <memory>
#include "Throw.h"
class Hand;

class IPropResponder
{
public:
    virtual void Tossed(int id) = 0;
    virtual void Catch(int id) = 0;
    virtual void Dropped(int id) = 0;
    //virtual void DecrementSiteswap(int id_) = 0;
    //virtual bool IsInFlight() = 0;
};

/// This is a base class for an object being juggled
class Prop : public IPropResponder
{
public:
         
    Prop(int id_);
    virtual ~Prop(void);

    //Properties
    int get_id()        {return id;}


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

    virtual void Toss(Throw* pass_);

    virtual void Catch();

    virtual void Collision();

    virtual void Pickup();

    virtual void Tick();

    IdPublisher tossed;
    IdPublisher ready_to_be_caught;
    IdPublisher dropped;

    void Tossed(int id_);
    void Catch(int id_);
    void Dropped(int id_);
    //void DecrementSiteswap(int id_);
    //bool IsInFlight();

protected:
    bool isIdValid(int id_);

    int getState();

    Throw toss;
private:


    int id;
    struct PropStateMachine;
    std::shared_ptr<PropStateMachine> stateMachine;

};

typedef std::shared_ptr<Prop> PropPtr;
