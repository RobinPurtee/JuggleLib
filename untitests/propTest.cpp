#include "stdafx.h"
#include "CppUnitTest.h"
#include "JuggleLib.h"
#include "Prop.h"
#include "Throw.h"
#include "Hand.h"
#include "DebugStringStream.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace untitests
{		
    TEST_CLASS(PropTests)
    {
        static const int test_id_ = 0xdeadbeef;

        class PropResponder 
        {
        public:
            PropResponder()
            : has_tossed_(false)
            , has_caught_(false)
            , has_dropped_(false)
            {}


            void Tossed(Prop* prop)
            {
                has_tossed_ = true;
                DebugOut() << "In Tossed callback: " << prop->toString();
            }

            void Catch(Prop* prop)
            {
                has_caught_ = true;
                DebugOut() << "In Catch callback: " << prop->toString();
            }

            void Dropped(DropReportPtr drp)
            {
                has_dropped_ = true;
                DebugOut() << "In Dropped callback: " << drp->toString();
            }

            bool has_tossed_;
            bool has_caught_;
            bool has_dropped_;


        };


        void connect_prop_responder(Prop& prop, PropResponder* responder)
        {
            prop.connectToToss(std::bind(&PropResponder::Tossed, responder, std::placeholders::_1));
            prop.connectToDrop(std::bind(&PropResponder::Dropped, responder, std::placeholders::_1));
            prop.connectToCatch(std::bind(&PropResponder::Catch, responder, std::placeholders::_1));
        }


        void run_til_catch(Prop& prop, PropResponder& responder)
        {
            int siteswap(1);
            Throw pass(siteswap, nullptr);

            DebugOut() << "propTest::run_til_catch: Initial state: " << prop.toString(); 
            prop.Pickup(nullptr);
            DebugOut() << "propTest::run_til_catch: After Pickup state: " << prop.toString(); 
            prop.Toss(pass);
            DebugOut() << "propTest::run_til_catch: After Toss state: " << prop.toString(); 
            Assert::IsTrue(responder.has_tossed_);
            while(0 < siteswap)
            {
                Assert::IsFalse(responder.has_caught_, L"The Prop trigger the catch notification early");
                prop.Tick();
                DebugOut() << "propTest::run_til_catch: After Tick state: " << prop.toString(); 
                --siteswap;
            }
        }


    public:
        
        TEST_METHOD(test_drop)
        {
            PropResponder responder;
            Prop prop(test_id_);

            connect_prop_responder(prop, &responder);
            run_til_catch(prop, responder);
            prop.Tick();
            DebugOut() << "propTest::test_drop: After Dropping Tick state: " << prop.toString(); 
            Assert::IsTrue(responder.has_dropped_, L"test_drop: The prop was NOT dropped");
        }

        TEST_METHOD(test_catch)
        {
            PropResponder responder;
            Prop prop(test_id_);

            connect_prop_responder(prop, &responder);
            run_til_catch(prop, responder);
            prop.Caught();
            Assert::IsFalse(responder.has_dropped_, L"The prop was dropped");
        }

        TEST_METHOD(invalid_transition_test)
        {
            PropResponder responder;
            Prop prop(test_id_);

            connect_prop_responder(prop, &responder);
  
            Assert::IsTrue(prop.isDropped(), L"The did not start on the ground");
            prop.Caught();
            Assert::IsTrue(prop.isDropped(), L"The Prop was caught from the ground which is wrong");
        }

        TEST_METHOD(dropped_once_caught_test)
        {
            PropResponder responder;
            Prop prop(test_id_);
            int siteswap(1);
            Throw pass(siteswap, nullptr);

            connect_prop_responder(prop, &responder);

            DebugOut() << "Initial state: " << prop.toString();
            prop.Pickup(nullptr);
            DebugOut() << "After Pickup: " << prop.toString();
            Assert::IsTrue(Prop::State::DWELL == prop.getState(), L"The state is not DWELL after Pickup");
            prop.Toss(pass);
            DebugOut() << "After Toss: " << prop.toString();
            Assert::IsTrue(responder.has_tossed_, L"Prop did not trick the tossed notification");
            Assert::IsTrue(prop.isInFlight(), L"Prop is not in Flight");
            prop.Tick();
            DebugOut() << "After Tick: " << prop.toString();
            Assert::IsFalse(prop.isInFlight(), L"Prop is still Flight when it should be in Catch");
            Assert::IsTrue(responder.has_caught_, L"Prop has not notified of coming catch");
            Assert::IsTrue(Prop::State::CATCH == prop.getState(), L"Prop is not in CATCH state after ticked out Catch call");
            prop.Caught();
            DebugOut() << "After Caught: " << prop.toString();
            Assert::IsTrue(responder.has_caught_, L"The Catch did not trigger the catch notification");
            Assert::IsFalse(prop.isDropped(), L"Prop has dropped during catch");
            Assert::IsFalse(prop.isInFlight(), L"Prop thinks it is still in flight when should be Dwell");
            prop.Collision();
            DebugOut() << "After Collision: " << prop.toString();
            Assert::IsTrue(responder.has_dropped_, L"Prop has not sent the drop notification");
            Assert::IsTrue(prop.isDropped(), L"The Prop was not dropped by the collision");
        }

        TEST_METHOD(dropped_on_toss_test)
        {
            PropResponder responder;
            Prop prop(test_id_);
            int siteswap(1);
            Throw pass(siteswap, nullptr);

            connect_prop_responder(prop, &responder);

            prop.Pickup(nullptr);
            prop.Toss(pass);
            Assert::IsTrue(responder.has_tossed_, L"Prop did not trick the tossed notification");
            Assert::IsTrue(prop.isInFlight(), L"Prop is not in Flight");
            prop.Collision();
            Assert::IsTrue(responder.has_dropped_, L"Prop has not sent the drop notification");
            Assert::IsTrue(prop.isDropped(), L"The Prop was not dropped by the collision");
        }

        TEST_METHOD(missed_catch_test)
        {
            PropResponder responder;
            Prop prop(test_id_);
            int siteswap(1);
            Throw pass(siteswap, nullptr);

            connect_prop_responder(prop, &responder);

            prop.Pickup(nullptr);
            DebugOut() << "After Pickup: " << prop.toString();
            prop.Toss(pass);
            DebugOut() << "After Toss: " << prop.toString();
            Assert::IsTrue(responder.has_tossed_, L"Prop did not tick the tossed notification");
            Assert::IsTrue(prop.isInFlight(), L"Prop is not in Flight");
            prop.Tick();
            DebugOut() << "After first Tick: " << prop.toString();
            Assert::IsFalse(prop.isInFlight(), L"Prop is still Flight when it should be in Catch");
            Assert::IsTrue(responder.has_caught_, L"Prop has not notified of coming catch");
            prop.Tick();
            DebugOut() << "After second Tick: " << prop.toString();
            Assert::IsTrue(responder.has_dropped_, L"Prop has not sent the drop notification");
            Assert::IsTrue(prop.isDropped(), L"The Prop was not dropped by missing the catch");
        }

    };
}