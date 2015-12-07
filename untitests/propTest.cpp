#include "stdafx.h"
#include "CppUnitTest.h"
#include "Prop.h"
#include "Pass.h"
#include "Hand.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace untitests
{		
	TEST_CLASS(UnitTest1)
	{
        static const int test_id = 0xdeadbeef;

        class PropResponder : public IPropResponder
        {
        public:
            PropResponder()
            : has_tossed(false)
            , has_caught(false)
            , has_dropped(false)
            , id_error(false)
            {}

            void Tossed(int id_)
            {
                has_tossed = true;
                id_error = id_ != test_id;
            }

            void Catch(int id_, Hand* destination_)
            {
                has_caught = true;
                id_error = id_ != test_id;
            }

            void Dropped(int id_)
            {
                has_dropped = true;
                id_error = id_ != test_id;
            }

            bool has_tossed;
            bool has_caught;
            bool has_dropped;
            bool id_error;


        };

	public:
		
		TEST_METHOD(test_drop)
		{
            PropResponder responder;
	        Prop prop(test_id, &responder);
            Hand hand;

            Pass pass(3, &hand);

            prop.Toss(&pass);
            Assert::IsFalse(responder.id_error);
            Assert::IsTrue(responder.has_tossed);
            prop.Tick();
            Assert::IsFalse(responder.id_error);
            Assert::IsFalse(responder.has_caught);
            prop.Tick();
            Assert::IsFalse(responder.id_error);
            Assert::IsFalse(responder.has_caught);
            prop.Tick();
            Assert::IsFalse(responder.id_error);
            Assert::IsTrue(responder.has_caught);
            prop.Tick();
            Assert::IsFalse(responder.id_error);
            Assert::IsTrue(responder.has_dropped);
		}

	};
}