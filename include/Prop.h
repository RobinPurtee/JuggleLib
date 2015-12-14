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
         
    Prop(int id);
    virtual ~Prop(void);

    //Properties
    int get_id()        {return id_;}


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
    bool isDropped();
    bool isInFlight();

    virtual void Toss(Throw* pass);

    virtual void Catch();

    virtual void Collision();

    virtual void Pickup();

    virtual void Tick();

    IdPublisher tossed;
    IdPublisher ready_to_be_caught;
    IdPublisher dropped;

    void Tossed(int id);
    void Catch(int id);
    void Dropped(int id);
    //void DecrementSiteswap(int id);
    //bool IsInFlight();

protected:
    bool isIdValid(int id);

    int getState();

    Throw toss_;
private:


    int id_;
    struct PropStateMachine;
    std::shared_ptr<PropStateMachine> stateMachine_;

};

typedef std::shared_ptr<Prop> PropPtr;
