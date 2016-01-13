#include "stdafx.h"
#include "TestHand.h"
#include "CppUnitTest.h"
#include <codecvt>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

static std::wstring_convert<std::codecvt_utf8<wchar_t>> widend;



TestHand::TestHand(int id)
:   Hand(id)
{
}


TestHand::~TestHand(void)
{
}

void TestHand::addProp(int id)
{
    props_.emplace(id, PropPtr(new Prop(id)));
}

Prop* TestHand::getProp(int id)
{
    return props_[id].get();
}

void TestHand::setTestMessage(const char* message)
{
    testMessage_  = message;
}

void TestHand::assertHandState(Hand::State state)
{
    std::stringstream message;

    message << testMessage_ << "Hand current state: " <<  getStateName() << " not: " << getStateName(state);
    Assert::IsTrue(getState() == state, widend.from_bytes(message.str().c_str()).c_str()); 
}

void TestHand::assertPropState(Prop* prop, Prop::State state)
{
    std::stringstream message;
    message << testMessage_ << "Prop(" << prop->getId() << ") current state: " <<  prop->getStateName() << " not: " << Prop::getStateName(state);
    Assert::IsTrue(prop->getState() == state, widend.from_bytes(message.str().c_str()).c_str());
}


void TestHand::assertPropState(int id, Prop::State state)
{
    assertPropState(getProp(id), state);
}


void TestHand::assertStates(Hand::State handState, Prop::State state0)
{
    PropStateList list = {state0};
    assertStates(handState, list);
 }

void TestHand::assertStates(Hand::State handState, Prop::State state0, Prop::State state1)
{
    PropStateList list = {state0, state1};
    assertStates(handState, list);
 }

void TestHand::assertStates(Hand::State handState, PropStateList propStates)
{
    assertHandState(handState);

    PropMap::iterator propPairItr(props_.begin()); 
    PropStateList::iterator stateItr(propStates.begin());
    while(propPairItr != props_.end() && stateItr != propStates.end()){
        assertPropState(propPairItr->second.get(), *stateItr);
        ++propPairItr; 
        ++stateItr;
    }
}


void TestHand::assertNumberOfProps(int num)
{
    std::stringstream message;
    message << testMessage_ << " Number of props in hand is: " <<  getNumberOfProps() << " not: " << num;
    Assert::IsTrue(getNumberOfProps() == num, widend.from_bytes(message.str().c_str()).c_str());
}


std::string TestHand::toString()
{
    std::stringstream out;
    out << testMessage_ << std::endl; 
    out << Hand::toString();
    out << "Prop List;" << std::endl;
    for(PropMap::value_type p : props_){
        out <<  p.second->toString();
    }
    return out.str();
}

std::wstring TestHand::toWstring()
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> widend;
    return widend.from_bytes(toString());
}
