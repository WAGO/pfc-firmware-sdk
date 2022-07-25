//------------------------------------------------------------------------------
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// This file is part of project common-header (PTXdist package libcommonheader).
//
// Copyright (c) 2021-2022 WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     test_Structuring.cc
///
///  \brief    Test for structuring helper macros targeting classes.
///
///  \author   PEn: WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "wc/structuring.h"

#include <type_traits>
#include <gtest/gtest.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
class interface
{
  WC_INTERFACE_CLASS(interface);
};

class derived_from_interface : public interface
{

};

class without_limitations
{

};

class with_disabled_move
{
public:
  with_disabled_move() = default;
  with_disabled_move(with_disabled_move const&) = default;
  with_disabled_move& operator=(with_disabled_move const&) = default;
  WC_DISBALE_CLASS_MOVE(with_disabled_move);
};

class with_disabled_move_assign
{
public:
  with_disabled_move_assign() = default;
  with_disabled_move_assign(with_disabled_move_assign const&) = default;
  with_disabled_move_assign& operator=(with_disabled_move_assign const&) = default;
  WC_DISBALE_CLASS_MOVE_ASSIGN(with_disabled_move_assign);
};

class with_disabled_copy_assign
{
public:
  with_disabled_copy_assign() = default;
  WC_DISBALE_CLASS_COPY_AND_ASSIGN(with_disabled_copy_assign);
  with_disabled_copy_assign(with_disabled_copy_assign&&) = default;
  with_disabled_copy_assign& operator=(with_disabled_copy_assign&&) = default;
};

class with_disabled_copy_assign_move
{
public:
  with_disabled_copy_assign_move() = default;
  WC_DISBALE_CLASS_COPY_ASSIGN_AND_MOVE(with_disabled_copy_assign_move);
  with_disabled_copy_assign_move& operator=(with_disabled_copy_assign_move&&) = default;
};

class with_disabled_copy_assign_move_assign
{
public:
  with_disabled_copy_assign_move_assign() = default;
  WC_DISBALE_CLASS_COPY_ASSIGN_AND_MOVE_ASSIGN(with_disabled_copy_assign_move_assign);
};

//------------------------------------------------------------------------------
// test implementation
//------------------------------------------------------------------------------
TEST(Structuring, class_interface)
{
  // For expected behavior of C++ interfaces see also:
  // - https://stackoverflow.com/a/30885842
  // - https://softwareengineering.stackexchange.com/questions/235674/what-is-the-pattern-for-a-safe-interface-in-c

  // Interface
  EXPECT_TRUE(std::is_polymorphic<interface>::value);

  EXPECT_FALSE(std::is_default_constructible<interface>::value);

  EXPECT_FALSE(std::is_copy_constructible<interface>::value);
  EXPECT_FALSE(std::is_copy_assignable<interface>::value);

  EXPECT_FALSE(std::is_move_constructible<interface>::value);
  EXPECT_FALSE(std::is_move_assignable<interface>::value);

  // Derived class from interface
  EXPECT_TRUE(std::is_polymorphic<derived_from_interface>::value);

  EXPECT_TRUE(std::is_copy_constructible<derived_from_interface>::value);
  EXPECT_TRUE(std::is_copy_assignable<derived_from_interface>::value);

  EXPECT_TRUE(std::is_move_constructible<derived_from_interface>::value);
  EXPECT_TRUE(std::is_move_assignable<derived_from_interface>::value);

  derived_from_interface test_instance;
}

TEST(Structuring, class_without_limitations)
{
  EXPECT_FALSE(std::is_polymorphic<without_limitations>::value);

  EXPECT_TRUE(std::is_copy_constructible<without_limitations>::value);
  EXPECT_TRUE(std::is_copy_assignable<without_limitations>::value);

  EXPECT_TRUE(std::is_move_constructible<without_limitations>::value);
  EXPECT_TRUE(std::is_move_assignable<without_limitations>::value);
}

TEST(Structuring, class_with_disabled_move)
{
  EXPECT_FALSE(std::is_polymorphic<without_limitations>::value);

  EXPECT_TRUE(std::is_copy_constructible<with_disabled_move>::value);
  EXPECT_TRUE(std::is_copy_assignable<with_disabled_move>::value);

  EXPECT_FALSE(std::is_move_constructible<with_disabled_move>::value);
  EXPECT_TRUE(std::is_move_assignable<with_disabled_move>::value);
}

TEST(Structuring, class_with_disabled_move_assign)
{
  EXPECT_FALSE(std::is_polymorphic<without_limitations>::value);

  EXPECT_TRUE(std::is_copy_constructible<with_disabled_move_assign>::value);
  EXPECT_TRUE(std::is_copy_assignable<with_disabled_move_assign>::value);

  EXPECT_FALSE(std::is_move_constructible<with_disabled_move_assign>::value);
  EXPECT_FALSE(std::is_move_assignable<with_disabled_move_assign>::value);
}

TEST(Structuring, class_with_disabled_copy_assign)
{
  EXPECT_FALSE(std::is_polymorphic<without_limitations>::value);

  EXPECT_FALSE(std::is_copy_constructible<with_disabled_copy_assign>::value);
  EXPECT_FALSE(std::is_copy_assignable<with_disabled_copy_assign>::value);

  EXPECT_TRUE(std::is_move_constructible<with_disabled_copy_assign>::value);
  EXPECT_TRUE(std::is_move_assignable<with_disabled_copy_assign>::value);
}

TEST(Structuring, class_with_disabled_copy_assign_move)
{
  EXPECT_FALSE(std::is_polymorphic<without_limitations>::value);

  EXPECT_FALSE(std::is_copy_constructible<with_disabled_copy_assign_move>::value);
  EXPECT_FALSE(std::is_copy_assignable<with_disabled_copy_assign_move>::value);

  EXPECT_FALSE(std::is_move_constructible<with_disabled_copy_assign_move>::value);
  EXPECT_TRUE(std::is_move_assignable<with_disabled_copy_assign_move>::value);
}

TEST(Structuring, class_with_disabled_copy_assign_move_assign)
{
  EXPECT_FALSE(std::is_polymorphic<without_limitations>::value);

  EXPECT_FALSE(std::is_copy_constructible<with_disabled_copy_assign_move_assign>::value);
  EXPECT_FALSE(std::is_copy_assignable<with_disabled_copy_assign_move_assign>::value);

  EXPECT_FALSE(std::is_move_constructible<with_disabled_copy_assign_move_assign>::value);
  EXPECT_FALSE(std::is_move_assignable<with_disabled_copy_assign_move_assign>::value);
}


//---- End of source file ------------------------------------------------------

