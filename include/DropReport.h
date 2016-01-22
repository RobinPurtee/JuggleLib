#pragma once
#include <forward_list>

namespace JuggleLib
{
    class Prop;
    class Hand;
    namespace std = ::std;

    typedef std::forward_list<Prop*> RawPropList;

    class DropReport
    {
    public:
        enum DropType:int {CATCH_MISSED = 0, MID_AIR_COLLISION, HAND_FULL, HAND_DROPPED, DROP};

        DropReport(DropType type, Prop* prop);
        DropReport(DropType type, Prop* prop, Hand* hand);
        virtual ~DropReport(void);

        DropType getType();

        Prop* getProp(int id);
        RawPropList::iterator getProps();
        void  addProp(Prop* prop);
    
        Hand* getHand();
        void  setHand(Hand* hand);

        virtual std::string  toString();
        virtual std::wstring toWString();

    protected:
        DropType type_;
        Hand* hand_;
        RawPropList props_;

    };

    typedef std::shared_ptr<DropReport> DropReportPtr;
}
