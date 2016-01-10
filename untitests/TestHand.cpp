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

    message << testMessage_ << " current state: " <<  getStateName() << " not: " << getStateName(state);
    Assert::IsTrue(getState() == state, widend.from_bytes(message.str().c_str()).c_str()); 
}

void TestHand::assertPropState(int id, Prop::State state)
{
    std::stringstream message;
    Prop* prop(getProp(id));

    message << testMessage_ << " current state: " <<  prop->getStateName() << " not: " << Prop::getStateName(state);
    Assert::IsTrue(prop->getState() == state, widend.from_bytes(message.str().c_str()).c_str());
}


std::string TestHand::toString()
{
    std::stringstream out;
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
