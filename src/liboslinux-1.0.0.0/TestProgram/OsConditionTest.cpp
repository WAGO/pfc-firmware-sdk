extern "C"
{
#include "OsCondition.h"
}

#include "CppUTest/TestHarness.h"

TEST_GROUP(OsCondition)
{
	OsCondition osCondition;

	void setup()
	{
		osCondition = OsCondition_Create();
	}

	void teardown()
	{
		OsCondition_Destroy(osCondition);
	}
};

TEST(OsCondition, Create)
{
	FAIL("Start here");
}

