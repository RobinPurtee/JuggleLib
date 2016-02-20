#include "pch.h"
#include "JuggleLib.h"
#include "Throw.h"
namespace JuggleLib
{
    namespace std = ::std;
    /// default contructor
    Throw::Throw() 
    : siteswap_(0)
    , destination_(nullptr) 
    {}
    /// initializing constructor
    Throw::Throw(int swap, Hand* dest) 
    :  siteswap_(swap)
    ,  destination_(dest)
    {}
    /// copy constructor
    Throw::Throw(const Throw& rhs)
    :   siteswap_(rhs.siteswap_)
    ,   destination_(rhs.destination_)
    {}

    /// copy operator
    Throw& Throw::operator=(const Throw& rhs)
    {
        if(&rhs != this)
        {
            siteswap_ = rhs.siteswap_;
            destination_ = rhs.destination_;
        }
        return *this;
    }

    void Throw::clear()
    {
        CriticalSection lock(lock_);
        siteswap_ = 0;
        destination_ = nullptr;
    }

    bool Throw::isZero()
    {
        CriticalSection lock(lock_);
        return 0 == siteswap_;
    }


    bool Throw::decrementSiteswap()
    {
        CriticalSection lock(lock_);
        bool bRet(isZero());
        if(!bRet){
            --siteswap_;
            bRet = isZero();
        }
        DebugOut() << "Throw::decrementSiteswap: current siteswap: " << siteswap_ << " Return: " << std::boolalpha << bRet;
        return bRet; 
    }
}
