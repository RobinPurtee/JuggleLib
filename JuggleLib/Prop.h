#pragma once

#include <memory>
#include "DropException.h"
class Hand;
struct Pass;

/// This is a base class for an object being juggled
class Prop
{
public:
    enum State {FLIGHT, CATCH, DWELL, TOSS, DROPPED};
         
    Prop(void);
    Prop(Hand* h);
    virtual ~Prop(void);

    //Properties
    /**
     *  Get the current state of the Prop
     */
    Prop::State GetState()  {return state;}
    
    /**
     * The current count away from the next hand
     * @return If the state is Flight then the number of counts away from its 
     *         destination Hand, else the value is 0;
     * @remark The value is set by the Toss and decremented each time Count is called 
     */
    int CurrentSwap()       {return swap;}
    /**
     * The current hand associated with the 
     * @return If the state is Flight then the destination hand, else the Hand that holds it, or nullptr if dropped.
     */

    Hand* CurrentHand()     {return hand;}

    /**
     * 
     */

    virtual void Toss(Pass* pass);

    virtual void Tossed();

    virtual void Catch(Hand* h);

    virtual void Caught(Hand* h);

    virtual void Drop();


    virtual void Pickup(Hand* h);

    virtual void Count();

private:

    Prop::State state;
    int swap;
    Hand* hand; // this is staight

};

typedef std::shared_ptr<Prop> PropPtr;
