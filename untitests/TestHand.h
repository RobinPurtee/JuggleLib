#pragma once
#include "JuggleLib.h"
#include "Hand.h"

class TestHand : public Hand
{
public:
    TestHand(int id);
    ~TestHand(void);

    void addProp(int id);
    Prop* getProp(int id);

    void setTestMessage(const char* message);

    void assertHandState(Hand::State state);
    void assertPropState(int id, Prop::State state);

    std::string toString();
    std::wstring toWstring();


private:
    std::string testMessage_;
    typedef std::map<int, PropPtr> PropMap;
    PropMap props_;
};

