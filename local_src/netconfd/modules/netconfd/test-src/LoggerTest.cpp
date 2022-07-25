// SPDX-License-Identifier: GPL-2.0-or-later
////------------------------------------------------------------------------------
/////  \file     BridgeConfigTransformatorTest.cpp
/////
/////  \brief    <short description of the file contents>
/////
/////  \author   <author> : WAGO GmbH & Co. KG
////------------------------------------------------------------------------------
//
////------------------------------------------------------------------------------
//// include files
////------------------------------------------------------------------------------
//#include <gtest/gtest.h>
//#include "Logger.hpp"
////------------------------------------------------------------------------------
//// defines; structure, enumeration and type definitions
////------------------------------------------------------------------------------
//
////------------------------------------------------------------------------------
//// function prototypes
////------------------------------------------------------------------------------
//
////------------------------------------------------------------------------------
//// macros
////------------------------------------------------------------------------------
//
////------------------------------------------------------------------------------
//// variables' and constants' definitions
////------------------------------------------------------------------------------
//
////------------------------------------------------------------------------------
//// function implementation
////------------------------------------------------------------------------------
//namespace netconf {
//
//
//class ALogger : public ::testing::Test {
// public:
//
//  Logger logger_;
//
//  ALogger()
//      : logger_(LogSink::terminal, LogLevel::warning) {
//
//  }
//
//};
//
//TEST_F(ALogger, tests) {
//
//  logger_.SetLogLevel(LogLevel::debug);
//
//  logger_.LogDebug("-----> Debug message");
//  logger_.LogInfo("-----> Info message");
//  logger_.LogWarning("-----> Warning message");
//  logger_.LogError("-----> Error message");
//
//}
//
//class ALogger_Target : public ::testing::Test {
// public:
//
//  Logger logger_;
//
//  ALogger_Target()
//      : logger_(LogSink::syslog, LogLevel::info) {
//
//  }
//};
//
//TEST_F(ALogger_Target, tests) {
//
//  logger_.LogError("---------LogLevel: Debug-----------");
//  logger_.SetLogLevel(LogLevel::debug);
//
//  logger_.LogDebug("-----> Debug message");
//  logger_.LogInfo("-----> Info message");
//  logger_.LogWarning("-----> Warning message");
//  logger_.LogError("-----> Error message");
//
//  logger_.LogError("---------LogLevel: Info-----------");
//  logger_.SetLogLevel(LogLevel::info);
//
//  logger_.LogDebug("-----> Debug message");
//  logger_.LogInfo("-----> Info message");
//  logger_.LogWarning("-----> Warning message");
//  logger_.LogError("-----> Error message");
//
//  logger_.LogError("---------LogLevel: Warning-----------");
//  logger_.SetLogLevel(LogLevel::warning);
//
//  logger_.LogDebug("-----> Debug message");
//  logger_.LogInfo("-----> Info message");
//  logger_.LogWarning("-----> Warning message");
//  logger_.LogError("-----> Error message");
//
//  logger_.LogError("---------LogLevel: Error-----------");
//  logger_.SetLogLevel(LogLevel::error);
//
//  logger_.LogDebug("-----> Debug message");
//  logger_.LogInfo("-----> Info message");
//  logger_.LogWarning("-----> Warning message");
//  logger_.LogError("-----> Error message");
//
//}
//
//}  // namespace netconf
////---- End of source file ------------------------------------------------------
//
