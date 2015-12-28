#pragma once
#include <memory>
#include "Throw.h"
class Hand;


/// This is a base class for an object being juggled
class Prop
{
public:
         
    Prop(int id);
    virtual ~Prop(void);

    //Properties
    int getId()         {return id_;}

    enum class State:int {DWELL = 0, FLIGHT, CATCH, DROPPED};

    Prop::State getState();

    /** 
     * Get the string of the current state name
     */
    const TCHAR* getStateName();
    
    /**
     * The current count away from the next hand
     * @return If the state is Flight then the number of counts away from its 
     *         destination Hand, else the value is 0;
     * @remark The value is set by the Throw and decremented each time Count is called 
     */
    int getCurrentSwap();

    /**
     * 
     */
    bool isDropped();
    bool isInFlight();

    virtual void Toss(Throw* toss);

    virtual void Catch(Hand* hand);

    virtual void Collision();

    virtual void Pickup(Hand* hand);

    virtual void Tick();

    
    void ConnectToToss(PropSlot slot);
    void DisconnectFromToss(PropSlot slot);

    void ConnectToDrop(PropSlot slot);
    void DisconnectFromDrop(PropSlot slot);

    void ConnectToCatch(PropSlot slot);
    void DisconnectFromCatch(PropSlot slot);

    void ConnectToAll(PropSlot tossSlot, PropSlot dropSlot, PropSlot propSlot);
    void DisconnectFromAll(PropSlot tossSlot, PropSlot dropSlot, PropSlot propSlot);

protected:
    //void DecrementSiteswap(int id);

    bool isIdValid(int id);


    
private:
    //IdPublisher tossed_;
    //PropPublisher ready_to_be_caught_;
    //IdPublisher dropped_;
    Throw toss_;


    int id_;
    struct PropStateMachine;
    std::shared_ptr<PropStateMachine> stateMachine_;

};

typedef std::shared_ptr<Prop> PropPtr;
