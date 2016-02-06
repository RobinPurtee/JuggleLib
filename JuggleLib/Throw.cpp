#include "pch.h"
#include "JuggleLib.h"
#include "Throw.h"
namespace JuggleLib
{
    namespace std = ::std;

    Throw::Throw() 
    : siteswap_(0)
    , destination_(nullptr) 
    {}

    Throw::Throw(int swap, Hand* dest) 
    :  siteswap_(swap)
    ,  destination_(dest)
    {}

    void Throw::clear()
    {
        CriticalSection lock(lock_);
        siteswap_ = 0;
        destination_ = nullptr;
    }

    bool Throw::isZero()
    {
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
