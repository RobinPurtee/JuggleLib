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
    const char* getStateName();
    
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

    void Toss(Throw& toss);

    void Caught();

    void Collision();

    void Pickup(Hand* hand);

    void Tick();

    
    void connectToToss(PropSlot slot);
    void disconnectFromToss(PropSlot slot);

    void connectToDrop(PropSlot slot);
    void disconnectFromDrop(PropSlot slot);

    void connectToCatch(PropSlot slot);
    void disconnectFromCatch(PropSlot slot);

    void connectToAll(PropSlot tossSlot, PropSlot dropSlot, PropSlot propSlot);
    void disconnectFromAll(PropSlot tossSlot, PropSlot dropSlot, PropSlot propSlot);

    std::string toString();

protected:
    bool decrementSiteswap();


    bool isIdValid(int id);


    
private:
    void dropped(Prop* prop);
    void connectHand(Hand* hand);
    void disconnectHand();


    PropPublisher tossed_;
    PropPublisher catch_;
    PropPublisher dropped_;
    Throw toss_;
    Hand* hand_;

    int id_;
    struct PropStateMachine;
    std::shared_ptr<PropStateMachine> stateMachine_;

};

typedef std::shared_ptr<Prop> PropPtr;
