#pragma once
 namespace JuggleLib
{
    namespace std = ::std;

    class Hand;
    /**
     * This class represents a passing action to be taken by a hand
     * The intent is for the hand to contain a list of these that it circles through
     * to provide the actions of the pattern
     *
     */
    class Throw
    {
    public:
        Throw();
        Throw(int swap, Hand* dest); 
        int getSiteswap()           {return siteswap_;}
        Hand* getDestination()      {return destination_;}
        /// clear the throw of data
        void clear();
        /// test if the count is 0
        bool isZero();
        /// decrement the current site swap value
        bool decrementSiteswap();


    private:
        int siteswap_;
        Hand* destination_;
    };
}