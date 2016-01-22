#include "pch.h"
#include "JuggleLib.h"

namespace JuggleLib
{
    const char* DropTypeNames[] = {
        "Prop missed by a catch", 
        "Prop collision",
        "The hand having Prop in it", 
        "Hand dropped the Prop",
        "Generic drop"
    };

    /// initializing constructor type and prop
    DropReport::DropReport(DropReport::DropType type, Prop* prop)
    :   type_(type)
    ,   hand_(nullptr)
    {
        addProp(prop);
    }
    /// initializing constructor type, prop, and hand
    DropReport::DropReport(DropReport::DropType type, Prop* prop, Hand* hand)
    :   type_(type)
    ,   hand_(hand)
    {
        addProp(prop);
    }
    /// destructor
    DropReport::~DropReport(void)
    {
    }
    /// Get the type of drop type
    DropReport::DropType DropReport::getType()  {return type_;}
    /// get the prop of the given ID
    Prop* DropReport::getProp(int id)
    {
        return *std::find_if(props_.begin(), props_.end(), [id](Prop*& prop)->bool{return prop->getId() == id;});
    }

    RawPropList::iterator DropReport::getProps(){return props_.begin();}

    /// Add a Prop the the Prop list
    void DropReport::addProp(Prop* prop)        
    {
        if(nullptr != prop){
            props_.push_front(prop);
        }
    }
    /// set the hand involved
    Hand* DropReport::getHand()                 {return hand_;}
    /// get the hand involved
    void  DropReport::setHand(Hand* hand)       {hand_ = hand;}
    /// construct a status string (mostly used for debug logs)
    std::string  DropReport::toString()
    {
        std::stringstream out;

        out << "A drop caused by: " << DropTypeNames[static_cast<int>(type_)] << std::endl;
        if(nullptr != hand_){
            out << "   Hand: " <<  hand_->toString();
        }
        for(Prop* prop : props_){
            out << "   Prop: " << prop->toString();
        }
        return out.str();
    }
    /// construct a wstring version of the status string
    std::wstring DropReport::toWString()
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> widend;
        return widend.from_bytes(toString());
    }
}