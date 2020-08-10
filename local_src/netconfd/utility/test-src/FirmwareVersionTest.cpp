// SPDX-License-Identifier: GPL-2.0-or-later

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "FirmwareVersion.hpp"

namespace netconf
{

  TEST(FirmwareVersion, TwoDigitFirmwareIndex)
  {
    FirmwareVersion fw{"03.04.02(11)"};

    EXPECT_EQ(11, fw.GetFirmwareIndex());
  }

  TEST(FirmwareVersion, ThreeDigitFirmwareIndex)
  {
    FirmwareVersion fw{"03.04.02(112)"};

    EXPECT_EQ(112, fw.GetFirmwareIndex());
  }

  TEST(FirmwareVersion, OneDigitFirmwareIndex)
  {
    FirmwareVersion fw{"03.04.02(6)"};

    EXPECT_EQ(6, fw.GetFirmwareIndex());
  }

  TEST(FirmwareVersion, EmptyIndex)
  {
    FirmwareVersion fw{"03.04.02()"};

    EXPECT_EQ(0, fw.GetFirmwareIndex());
  }

  TEST(FirmwareVersion, NoIndexField)
  {
    FirmwareVersion fw{"03.04.02"};

    EXPECT_EQ(0, fw.GetFirmwareIndex());
  }

  TEST(FirmwareVersion, LeadingZeros)
  {
    FirmwareVersion fw{"03.04.02(007)"};

    EXPECT_EQ(7, fw.GetFirmwareIndex());
  }

  TEST(FirmwareVersion, OtherNumberFormatIgnored)
  {
    FirmwareVersion fw{"03.04.02(0x007)"};

    EXPECT_EQ(0, fw.GetFirmwareIndex());
  }

  TEST(FirmwareVersion, NoNumericalCharacterIgnored)
  {
    FirmwareVersion fw{"03.04.02(abc)"};

    EXPECT_EQ(0, fw.GetFirmwareIndex());
  }

  TEST(FirmwareVersion, MajorMinorBugfix)
  {
    FirmwareVersion version_030402{"03.04.02(00)"};

    FirmwareVersion version_020402{"02.04.02(00)"};
    FirmwareVersion version_030302{"03.03.02(00)"};
    FirmwareVersion version_030401{"03.04.01(00)"};
    FirmwareVersion version_040402{"04.04.02(00)"};
    FirmwareVersion version_020502{"03.05.02(00)"};
    FirmwareVersion version_030403{"03.04.03(00)"};

    EXPECT_LT(version_020402, version_030402);
    EXPECT_LT(version_030302, version_030402);
    EXPECT_LT(version_030401, version_030402);

    EXPECT_GT(version_040402, version_030402);
    EXPECT_GT(version_020502, version_030402);
    EXPECT_GT(version_030403, version_030402);

    EXPECT_EQ(version_030402, version_030402);

  }

  TEST(FirmwareVersion, FirmwareWithoutIndex)
  {

    FirmwareVersion version_030402_00{"03.04.02(01)"};

    FirmwareVersion version_030402{"03.04.02"};

    EXPECT_NE(version_030402, version_030402_00);
  }

  TEST(FirmwareVersion, InvalidFirmware)
  {

    FirmwareVersion version_030402{"03.04.02"};
    FirmwareVersion version_0304{"03.04"};
    FirmwareVersion version_03{"03"};
    FirmwareVersion version_empty{""};
    FirmwareVersion version_{};

    EXPECT_FALSE(version_030402.IsValid());
    EXPECT_FALSE(version_0304.IsValid());
    EXPECT_FALSE(version_03.IsValid());
    EXPECT_FALSE(version_empty.IsValid());
    EXPECT_FALSE(version_.IsValid());
  }

  TEST(FirmwareVersion, CopyAndMove)
  {

    FirmwareVersion version_030402{"03.04.02"};
    FirmwareVersion version_copy_constructed{version_030402};
    FirmwareVersion version_copy_assigned{"01.01.01(01)"};
    version_copy_assigned = version_030402;

    EXPECT_EQ(version_030402, version_copy_constructed);
    EXPECT_EQ(version_030402, version_copy_assigned);

    FirmwareVersion version_move_constructed{version_030402};
    EXPECT_EQ(FirmwareVersion{"03.04.02"}, version_move_constructed);

  }

}
