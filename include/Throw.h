#pragma once
#include <memory>
#include "Prop.h"

class Hand;
/**
 * This structure represents a passing action to be taken by a hand
 * The intent is for the hand to contain a list of these that it circles through
 * to provide the actions of the pattern
 *
 */
class Throw
{
public:
    Throw() : siteswap(0), destination(nullptr) {}

    Throw(int swap, Hand* dest) 
        :  siteswap(swap)
        ,  destination(dest)
    {}

    void clear()
    {
        siteswap = 0;
        destination = nullptr;
    }

    int siteswap;
    Hand* destination;
};

