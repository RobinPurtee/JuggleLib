#include "pch.h"
#include "Prop.h"
#include "Hand.h"


Prop::Prop(void)
:   state(Prop::State::DROPPED)
,   swap(0)
,   hand(nullptr) 
{
}

Prop::Prop(Hand* h)
:   state(Prop::State::DWELL)
,   swap(0)
,   hand(h) 
{
}



Prop::~Prop(void)
{
}


/**
 * Start accelorating the prop
 * @remarks 
 * A sucessful toss can only be started if the prop has be caught and in the DWELL state
 */

void Prop::Toss(Pass* pass)
{
    if(State::DWELL == state && nullptr != pass)
    {
        state = State::TOSS;
        swap = pass->siteswap;
        hand = pass->destination;
    }
    else
    {
        Drop();
        throw new DropException(this);
    }
}


void Prop::Tossed()
{
    if(State::TOSS == state)
    {
        state = State::FLIGHT;
    }
    else
    {
        Drop();
        throw new DropException(this);
    }
}



void Prop::Catch(Hand* h)
{
    if(State::FLIGHT != state)
    {
        throw new DropException(this);
    }
    state = State::CATCH;
    swap = 0;
    hand = h;
}


void Prop::Caught(Hand* hand)
{
    state = State::DWELL;

}


void Prop::Drop()
{
    state = State::DROPPED;
    swap = 0;
    hand = nullptr;
}

void Prop::Pickup(Hand* h)
{
    if(State::DROPPED != state)
    {
        throw new DropException(this);
    }

    if(nullptr != h)
    {
        state = State::DWELL;
        swap = 0;
        hand = h;
    }
}

void Prop::Count()
{
    if(0 < swap)
    {
        --swap;
        if(1 == swap && nullptr != hand)
        {
            hand->InComming();
        }
        if(0 == swap)
        {
            if(nullptr != hand)
            {
                try
                {
                    hand->Catch(this);
                }
                catch(DropException& drop)
                {
                    Drop();
                    throw drop;
                }
            }
        }
    }
}