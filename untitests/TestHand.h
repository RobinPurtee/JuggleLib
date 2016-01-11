#pragma once
#include "JuggleLib.h"
#include "Hand.h"

typedef std::vector<Prop::State> PropStateList;

class TestHand : public Hand
{
public:
    TestHand(int id);
    ~TestHand(void);

    void addProp(int id);
    Prop* getProp(int id);

    void setTestMessage(const char* message);

    void assertHandState(Hand::State state);
    void assertPropState(Prop* prop, Prop::State state);
    void assertPropState(int id, Prop::State state);
    //void assertStates(Hand::State handState, Prop::State propStates[]);
    void assertStates(Hand::State handState, PropStateList propStates);

    std::string toString();
    std::wstring toWstring();


private:
    std::string testMessage_;
    typedef std::map<int, PropPtr> PropMap;
    PropMap props_;
};

