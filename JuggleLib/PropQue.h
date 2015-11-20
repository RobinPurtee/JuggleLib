#ifndef PROP_QUE_H
#define PROP_QUE_H

#include <list>
#include <memory>
#include "Prop.h"   

///
/// PropQue class
/// @remark This class is a specialized stack container of PropPtr objects. Items  
/// inserted at random indexes, as long has is not already at the given location.
/// Items are removed by popping, which decrease the index of each remaining item 
/// remaining items location in the queue. The pop may return nullptr if there
/// is no item at the current top of the queue, or if the queue is empty. To tell 
/// the difference the size of the queue needs to be checked.

class PropQue 
{
public:
    PropQue(void);
    ~PropQue(void);

    /// Add a prop into the que at the given site 
    /// <parameter>Prop* prop - the prop to add</parmeter>
    /// <parameter>int site - the siteswap value to que the throw at</parameter>
    /// <return> true if the prop was added or false of this would cause a collision</return>
    bool AddSite(Prop* prop, int site);

    /// method called to advance the que
    void Count();
    
    /// Event fired when a Prop has reached the last position in the que
    typedef void (*PropArrivingEvent)(Prop*);
    /// Event fired when a Prop has dropped out of the que
    typedef void (*CatchPropEvent)(Prop*);

private:
    struct QueItem
    {
        int swap;
        std::shared_ptr<Prop> prop;

        bool operator<(const QueItem& rhs)
        {
            return swap < rhs.swap;
        }
    };




    std::list<QueItem> theList;


};


#endif //PROP_QUE_H