extern "C"
{
#include "OsRessource.h"
}

#include "CppUTest/TestHarness.h"

TEST_GROUP(OsRessource)
{
	OsRessource osRessource;

	void setup()
	{
		osRessource = OsRessource_Create();
	}

	void teardown()
	{
		OsRessource_Destroy(osRessource);
	}
};

TEST(OsRessource, Create)
{
	FAIL("Start here");
}

