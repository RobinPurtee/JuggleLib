#pragma once
#include "JuggleLib.h"
#include "Hand.h"
#include <array>

typedef std::array<Prop::State, 5> PropStateList;

class TestHand : public Hand
{
public:
    TestHand();
    TestHand(int id);
    ~TestHand(void);

    void addProp(int id);
    Prop* getProp(int id);

    void setTestMessage(const char* message);

    void tick();

    void assertHandState(Hand::State state);
    void assertPropState(Prop* prop, Prop::State state);
    void assertPropState(int id, Prop::State state);
    void assertStates(Hand::State handState, Prop::State state0);
    void assertStates(Hand::State handState, Prop::State state0, Prop::State state1);
    void assertStates(Hand::State handState, PropStateList propStates);

    void assertNumberOfProps(int num);



    std::string toString() override;
    std::wstring toWstring() override;


private:
    std::string testMessage_;
    boost::signals2::signal<void()> tick_;

    typedef std::map<int, PropPtr> PropMap;
    PropMap props_;
};

