// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG


#include <gtest/gtest.h>

#include "../mdm_diagnostic.h"
#include "testutil_diagnostic.h"
#include "diagnostic/mdmd_diag.h"

class MdmDiagnosticTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
      test_log_EVENT_init();
    }
};

/* ANF_01:
   Statusänderungen der LEDs müssen differentiell signalisiert, d.h. es dürfen nicht jedes mal alle LEDS vollständig neu gesetzt werden.
   Dazu müssen mit Start des Modem-Managers einmalig alle LEDs definiert gesetzt werden.
   -> Initialzustand ist "keine Verbindung", d.h. NET-LED rot, alle Signal-LEDs aus sein
   Ziel: Anzahl der Diagnosemeldungen und damit die Ressourcen-Auslastung des Gerätes allgemein minimieren.
*/
TEST_F(MdmDiagnosticTest, Initialization)
{
  MdmDiagnostic mdmDiag;

  // Expect Diagnostic API initialization
  EXPECT_STREQ(test_get_log_EVENT_ProgramName(), "mdmd");
  EXPECT_EQ(mdmDiag.get_error_state(), MdmErrorState::NONE);
  EXPECT_EQ(mdmDiag.get_access_class(), MdmAccessClass::NONE);
  EXPECT_EQ(mdmDiag.get_signal_quality_level(), MdmSignalQualityLevel::NONE);

  log_tEventId log_event_id;
  bool         log_event_set;
  // Expect initial events that switches all SIGNAL-LEDs off in descending order
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_6_OFF);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_5_OFF);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_4_OFF);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_3_OFF);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_2_OFF);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_1_OFF);
  EXPECT_TRUE(log_event_set);
  // Expect initial event that set NET-LED to static RED (NO NET)
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_NO_NET);
  EXPECT_TRUE(log_event_set);

  // Expect no further event
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}
/* ANF_02:
   Wenn entweder kein Mobilfunkstandard oder keine Signalqualität gesetzt ist,
   dann muss der entsprechende Zustand "keine Verbindung" angezeigt werden, 
   d.h. NET-LED rot, alle Signal-LEDs aus sein
   Fall A): Signalqualität wird gesetzt, Mobilfunkstandard ist noch nicht gesetzt
*/
TEST_F(MdmDiagnosticTest, SetSignalLevelWithoutNet)
{
  MdmDiagnostic mdmDiag;

  // Push initialization events => 6 for SIGNAL LEDs + 1 for NET LED (see Test Initialization)
  log_tEventId log_event_id;
  bool         log_event_set;
  for(int i=0; i<7; i++)
  {
    EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  }

  // Set signal strength 0 -> 1
  mdmDiag.set_signal_quality_level(MdmSignalQualityLevel::MARGINAL);
  EXPECT_EQ(mdmDiag.get_signal_quality_level(), MdmSignalQualityLevel::MARGINAL);
  // Expect no event while network is not registered
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}

/* ANF_02:
   Wenn entweder kein Mobilfunkstandard oder keine Signalqualität gesetzt ist,
   dann muss der entsprechende Zustand "keine Verbindung" angezeigt werden, 
   d.h. NET-LED rot, alle Signal-LEDs aus sein
   Fall B): Mobilfunkstandard wird gesetzt, Signalqualität ist noch nicht gesetzt
*/
TEST_F(MdmDiagnosticTest, SetAccessClassWithoutSignal)
{
  MdmDiagnostic mdmDiag;

  // Push initialization events => 6 for SIGNAL LEDs + 1 for NET LED (see Test Initialization)
  log_tEventId log_event_id;
  bool         log_event_set;
  for(int i=0; i<7; i++)
  {
    EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  }

  // Set access class 2G
  mdmDiag.set_access_class(MdmAccessClass::GSM);
  EXPECT_EQ(mdmDiag.get_access_class(), MdmAccessClass::GSM);
  // Expect no event, NET LED should be still static RED (NO NET) while signal level is zero
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}

/* ANF_03:
   Wenn Mobilfunkstandard und Signalqualität gleichzeitig gesetzt sind,
   dann muss der zu diesem Zeitpunkt entsprechende Zustand angezeigt werden, 
   d.h. NET-LED zeigt den Mobilfunkstandard und die Signal-LEDs zeigen die Signalqualität.
   Fall A): Mobilfunkstandard wird nach Setzen der Signalqualität ebenfalls gesetzt
*/
TEST_F(MdmDiagnosticTest, RegistrationAfterSignal)
{
  MdmDiagnostic mdmDiag;

  // Push initialization events => 6 for SIGNAL LEDs + 1 for NET LED (see Test Initialization)
  log_tEventId log_event_id;
  bool         log_event_set;
  for(int i=0; i<7; i++)
  {
    EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  }

/*registration in mobile network*/
  // Set signal strength 0 -> 3
  mdmDiag.set_signal_quality_level(MdmSignalQualityLevel::SUFFICIENT);
  EXPECT_EQ(mdmDiag.get_signal_quality_level(), MdmSignalQualityLevel::SUFFICIENT);
  // Set access class 2G
  mdmDiag.set_access_class(MdmAccessClass::GSM);
  EXPECT_EQ(mdmDiag.get_access_class(), MdmAccessClass::GSM);
  //Expect event that set NET-LED according to this access class
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_NET_2G);
  EXPECT_TRUE(log_event_set);
  //Expect events that switches all SIGNAL-LEDs on in ascending order
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_1_ON);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_2_ON);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_3_ON);
  EXPECT_TRUE(log_event_set);

  // Expect no further event
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}

/* ANF_03:
   Wenn Mobilfunkstandard und Signalqualität gleichzeitig gesetzt sind,
   dann muss der zu diesem Zeitpunkt entsprechende Zustand angezeigt werden, 
   d.h. NET-LED zeigt den Mobilfunkstandard und die Signal-LEDs zeigen die Signalqualität.
   Fall B): Signalqualität wird nach Setzen des Mobilfunkstandard ebenfalls gesetzt
*/
TEST_F(MdmDiagnosticTest, SetSignalAfterRegistration)
{
  MdmDiagnostic mdmDiag;

  // Push initialization events => 6 for SIGNAL LEDs + 1 for NET LED (see Test Initialization)
  log_tEventId log_event_id;
  bool         log_event_set;
  for(int i=0; i<7; i++)
  {
    EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  }

/*registration in mobile network*/
  // Set access class 2G
  mdmDiag.set_access_class(MdmAccessClass::GSM);
  EXPECT_EQ(mdmDiag.get_access_class(), MdmAccessClass::GSM);
  // Set signal strength 0 -> 3
  mdmDiag.set_signal_quality_level(MdmSignalQualityLevel::SUFFICIENT);
  EXPECT_EQ(mdmDiag.get_signal_quality_level(), MdmSignalQualityLevel::SUFFICIENT);
  //Expect event that set NET-LED according to this access class
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_NET_2G);
  EXPECT_TRUE(log_event_set);
  //Expect events that switches all SIGNAL-LEDs on in ascending order
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_1_ON);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_2_ON);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_3_ON);
  EXPECT_TRUE(log_event_set);

  // Expect no further event
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}

/* ANF_02:
   Wenn entweder kein Mobilfunkstandard oder keine Signalqualität gesetzt ist,
   dann muss der entsprechende Zustand "keine Verbindung" angezeigt werden, 
   d.h. NET-LED rot, alle Signal-LEDs aus sein
   Fall C): Mobilfunkstandard wird zurückgesetzt, Signalqualität ist weiter gesetzt
*/
TEST_F(MdmDiagnosticTest, RegistrationLoss)
{
  MdmDiagnostic mdmDiag;

  // Push initialization events => 6 for SIGNAL LEDs + 1 for NET LED (see Test Initialization)
  log_tEventId log_event_id;
  bool         log_event_set;
  for(int i=0; i<7; i++)
  {
    EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  }

/*registration in mobile network*/
  // Set signal strength 0 -> 4
  mdmDiag.set_signal_quality_level(MdmSignalQualityLevel::STABLE);
  EXPECT_EQ(mdmDiag.get_signal_quality_level(), MdmSignalQualityLevel::STABLE);
  // Set access class 2G
  mdmDiag.set_access_class(MdmAccessClass::GSM);
  EXPECT_EQ(mdmDiag.get_access_class(), MdmAccessClass::GSM);
  //Expect event that set NET-LED according to this access class
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_NET_2G);
  EXPECT_TRUE(log_event_set);
  //Expect events that switches all SIGNAL-LEDs on in ascending order
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_1_ON);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_2_ON);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_3_ON);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_4_ON);
  EXPECT_TRUE(log_event_set);

/*de-registration from mobile network*/
  // Set access class NONE
  mdmDiag.set_access_class(MdmAccessClass::NONE);
  EXPECT_EQ(mdmDiag.get_access_class(), MdmAccessClass::NONE);
  //Expect events that switches all SIGNAL-LEDs off in descending order
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_4_OFF);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_3_OFF);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_2_OFF);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_1_OFF);
  EXPECT_TRUE(log_event_set);
  // Expect event that set NET-LED to static RED (NO NET)
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_NO_NET);
  EXPECT_TRUE(log_event_set);

  // Expect no further event
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}

/* ANF_03:
   Wenn Mobilfunkstandard und Signalqualität gleichzeitig gesetzt sind,
   dann muss der zu diesem Zeitpunkt entsprechende Zustand angezeigt werden, 
   d.h. NET-LED zeigt den Mobilfunkstandard und die Signal-LEDs zeigen die Signalqualität.
   Fall C): Änderung der Signalqualität
*/
TEST_F(MdmDiagnosticTest, ChangeSignalAfterRegistration)
{
  MdmDiagnostic mdmDiag;

  // Push initialization events => 6 for SIGNAL LEDs + 1 for NET LED (see Test Initialization)
  log_tEventId log_event_id;
  bool         log_event_set;
  for(int i=0; i<7; i++)
  {
    EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  }

/*registration in mobile network*/
  // Set signal strength 0 -> 2
  mdmDiag.set_signal_quality_level(MdmSignalQualityLevel::BAD);
  EXPECT_EQ(mdmDiag.get_signal_quality_level(), MdmSignalQualityLevel::BAD);
  // Set access class 3G
  mdmDiag.set_access_class(MdmAccessClass::UMTS);
  EXPECT_EQ(mdmDiag.get_access_class(), MdmAccessClass::UMTS);
  //Expect event that set NET-LED according to this access class
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_NET_3G);
  EXPECT_TRUE(log_event_set);
  //Expect events that switches all SIGNAL-LEDs on in ascending order
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_1_ON);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_2_ON);
  EXPECT_TRUE(log_event_set);

  // Set signal strength 2 -> 6
  mdmDiag.set_signal_quality_level(MdmSignalQualityLevel::EXCELLENT);
  EXPECT_EQ(mdmDiag.get_signal_quality_level(), MdmSignalQualityLevel::EXCELLENT);
  //Expect events that switches further SIGNAL-LEDs on in ascending order
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_3_ON);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_4_ON);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_5_ON);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_6_ON);
  EXPECT_TRUE(log_event_set);

  // Set signal strength 6 -> 4
  mdmDiag.set_signal_quality_level(MdmSignalQualityLevel::STABLE);
  EXPECT_EQ(mdmDiag.get_signal_quality_level(), MdmSignalQualityLevel::STABLE);
  //Expect events that switches SIGNAL-LEDs off again in descending order
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_6_OFF);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_5_OFF);
  EXPECT_TRUE(log_event_set);

  // Expect no further event
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}

/* ANF_01:
   Statusänderungen der LEDs müssen differentiell signalisiert, d.h. es dürfen nicht jedes mal alle LEDS vollständig neu gesetzt werden.
   Fall A) Die gleiche Signalqualität wird erneut gesetzt, z.B. in Folge sehr kleiner Schwankungen.
*/
TEST_F(MdmDiagnosticTest, SameSignalAgain)
{
  MdmDiagnostic mdmDiag;

  // Push initialization events => 6 for SIGNAL LEDs + 1 for NET LED (see Test Initialization)
  log_tEventId log_event_id;
  bool         log_event_set;
  for(int i=0; i<7; i++)
  {
    EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  }

/*registration in mobile network*/
  // Set signal strength 0 -> 2
  mdmDiag.set_signal_quality_level(MdmSignalQualityLevel::BAD);
  EXPECT_EQ(mdmDiag.get_signal_quality_level(), MdmSignalQualityLevel::BAD);
  // Set access class 3G
  mdmDiag.set_access_class(MdmAccessClass::UMTS);
  EXPECT_EQ(mdmDiag.get_access_class(), MdmAccessClass::UMTS);
  //Expect event that set NET-LED according to this access class
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_NET_3G);
  EXPECT_TRUE(log_event_set);
  //Expect events that switches all SIGNAL-LEDs on in ascending order
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_1_ON);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_2_ON);
  EXPECT_TRUE(log_event_set);

  // Set signal strength 2 again
  mdmDiag.set_signal_quality_level(MdmSignalQualityLevel::BAD);
  // Expect no event when signal is not changed
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}

/* ANF_02:
   Wenn entweder kein Mobilfunkstandard oder keine Signalqualität gesetzt ist,
   dann muss der entsprechende Zustand "keine Verbindung" angezeigt werden, 
   d.h. NET-LED rot, alle Signal-LEDs aus sein
   Fall D): Signalqualität wird zurückgesetzt, Mobilfunkstandard ist weiter gesetzt
*/
TEST_F(MdmDiagnosticTest, SignalLoss)
{
  MdmDiagnostic mdmDiag;

  // Push initialization events => 6 for SIGNAL LEDs + 1 for NET LED (see Test Initialization)
  log_tEventId log_event_id;
  bool         log_event_set;
  for(int i=0; i<7; i++)
  {
    EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  }

/*registration in mobile network*/
  // Set signal strength 0 -> 3
  mdmDiag.set_signal_quality_level(MdmSignalQualityLevel::SUFFICIENT);
  EXPECT_EQ(mdmDiag.get_signal_quality_level(), MdmSignalQualityLevel::SUFFICIENT);
  // Set access class 2G
  mdmDiag.set_access_class(MdmAccessClass::GSM);
  EXPECT_EQ(mdmDiag.get_access_class(), MdmAccessClass::GSM);
  //Expect event that set NET-LED according to this access class
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_NET_2G);
  EXPECT_TRUE(log_event_set);
  //Expect events that switches all SIGNAL-LEDs on in ascending order
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_1_ON);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_2_ON);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_3_ON);
  EXPECT_TRUE(log_event_set);

  // Set signal strength 3 -> 0 (signal loss)
  mdmDiag.set_signal_quality_level(MdmSignalQualityLevel::NONE);
  EXPECT_EQ(mdmDiag.get_signal_quality_level(), MdmSignalQualityLevel::NONE);
  //Expect events that switches all SIGNAL-LEDs off in descending order
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_3_OFF);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_2_OFF);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_1_OFF);
  EXPECT_TRUE(log_event_set);
  // Expect event that set NET-LED to static RED (NO NET)
  //-> same behaviour as de-registration
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_NO_NET);
  EXPECT_TRUE(log_event_set);

  // Expect no further event
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}

/* ANF_03:
   Wenn Mobilfunkstandard und Signalqualität gleichzeitig gesetzt sind,
   dann muss der zu diesem Zeitpunkt entsprechende Zustand angezeigt werden, 
   d.h. NET-LED zeigt den Mobilfunkstandard und die Signal-LEDs zeigen die Signalqualität.
   Fall D): Änderung des Mobilfunkstandards von 2G zu 3G
*/
TEST_F(MdmDiagnosticTest, ChangeAccessClass2GTo3G)
{
  MdmDiagnostic mdmDiag;

  // Push initialization events => 6 for SIGNAL LEDs + 1 for NET LED (see Test Initialization)
  log_tEventId log_event_id;
  bool         log_event_set;
  for(int i=0; i<7; i++)
  {
    EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  }

  // Set signal strength 0 -> 1
  mdmDiag.set_signal_quality_level(MdmSignalQualityLevel::MARGINAL);
  EXPECT_EQ(mdmDiag.get_signal_quality_level(), MdmSignalQualityLevel::MARGINAL);
  // Set access class 2G
  mdmDiag.set_access_class(MdmAccessClass::GSM);
  EXPECT_EQ(mdmDiag.get_access_class(), MdmAccessClass::GSM);
  //Expect event for access class and signal level
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_NET_2G);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_1_ON);
  EXPECT_TRUE(log_event_set);

  // Set access class 3G
  mdmDiag.set_access_class(MdmAccessClass::UMTS);
  EXPECT_EQ(mdmDiag.get_access_class(), MdmAccessClass::UMTS);
  //Expect event for access class and signal level
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_NET_3G);
  EXPECT_TRUE(log_event_set);

  // Expect no further event
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}

/* ANF_03:
   Wenn Mobilfunkstandard und Signalqualität gleichzeitig gesetzt sind,
   dann muss der zu diesem Zeitpunkt entsprechende Zustand angezeigt werden, 
   d.h. NET-LED zeigt den Mobilfunkstandard und die Signal-LEDs zeigen die Signalqualität.
   Fall E): Änderung des Mobilfunkstandards von 3G zu 2G
*/
TEST_F(MdmDiagnosticTest, ChangeAccessClass3GTo2G)
{
  MdmDiagnostic mdmDiag;

  // Push initialization events => 6 for SIGNAL LEDs + 1 for NET LED (see Test Initialization)
  log_tEventId log_event_id;
  bool         log_event_set;
  for(int i=0; i<7; i++)
  {
    EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  }

  // Set signal strength 0 -> 5
  mdmDiag.set_signal_quality_level(MdmSignalQualityLevel::GOOD);
  EXPECT_EQ(mdmDiag.get_signal_quality_level(), MdmSignalQualityLevel::GOOD);
  // Set access class 3G
  mdmDiag.set_access_class(MdmAccessClass::UMTS);
  EXPECT_EQ(mdmDiag.get_access_class(), MdmAccessClass::UMTS);
  //Expect event for access class and signal level
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_NET_3G);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_1_ON);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_2_ON);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_3_ON);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_4_ON);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_5_ON);
  EXPECT_TRUE(log_event_set);

  // Set access class 2G
  mdmDiag.set_access_class(MdmAccessClass::GSM);
  EXPECT_EQ(mdmDiag.get_access_class(), MdmAccessClass::GSM);
  //Expect event for access class and signal level
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_NET_2G);
  EXPECT_TRUE(log_event_set);

  // Expect no further event
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}

/* ANF_01:
   Statusänderungen der LEDs müssen differentiell signalisiert, d.h. es dürfen nicht jedes mal alle LEDS vollständig neu gesetzt werden.
   Fall B) Der gleiche Mobilfunkstandard wird erneut gesetzt, in Folge eines anderen Netztyps innerhalb des Standards (z.B. GSM->EDGE oder UMTS->HSDPA).
*/
TEST_F(MdmDiagnosticTest, SameAccessClassAgain)
{
  MdmDiagnostic mdmDiag;

  // Push initialization events => 6 for SIGNAL LEDs + 1 for NET LED (see Test Initialization)
  log_tEventId log_event_id;
  bool         log_event_set;
  for(int i=0; i<7; i++)
  {
    EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  }

  // Set signal strength 0 -> 1
  mdmDiag.set_signal_quality_level(MdmSignalQualityLevel::MARGINAL);
  EXPECT_EQ(mdmDiag.get_signal_quality_level(), MdmSignalQualityLevel::MARGINAL);
  // Set access class 2G
  mdmDiag.set_access_class(MdmAccessClass::GSM);
  EXPECT_EQ(mdmDiag.get_access_class(), MdmAccessClass::GSM);
  //Expect event for access class and signal level
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_NET_2G);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_1_ON);
  EXPECT_TRUE(log_event_set);

  // Set access class 2G again
  mdmDiag.set_access_class(MdmAccessClass::GSM);
  // Expect no event when access class is not changed
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}

/* ANF_04:
   Relevante Fehlerzustände müssen über die NET-LED signalisiert werden.
   Fall A) Modemschnittstelle ist nicht verfügbar
*/
TEST_F(MdmDiagnosticTest, SetPortError)
{
  MdmDiagnostic mdmDiag;

  // Push initialization events => 6 for SIGNAL LEDs + 1 for NET LED (see Test Initialization)
  log_tEventId log_event_id;
  bool         log_event_set;
  for(int i=0; i<7; i++)
  {
    EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  }

  // Set error PORT_NOT_READY
  mdmDiag.set_error_state(MdmErrorState::PORT_NOT_READY);
  EXPECT_EQ(mdmDiag.get_error_state(), MdmErrorState::PORT_NOT_READY);
  // Expect event for this error
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_ERR_PORT_NOT_READY);
  EXPECT_TRUE(log_event_set);

  // Expect no further event
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}

/* ANF_04:
   Relevante Fehlerzustände müssen über die NET-LED signalisiert werden.
   Fall B) Initialisierung des Modems ist fehlgeschlagen
*/
TEST_F(MdmDiagnosticTest, SetInitError)
{
  MdmDiagnostic mdmDiag;

  // Push initialization events => 6 for SIGNAL LEDs + 1 for NET LED (see Test Initialization)
  log_tEventId log_event_id;
  bool         log_event_set;
  for(int i=0; i<7; i++)
  {
    EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  }

  // Set error INIT_FAILED
  mdmDiag.set_error_state(MdmErrorState::INIT_FAILED);
  EXPECT_EQ(mdmDiag.get_error_state(), MdmErrorState::INIT_FAILED);
  // Expect event for this error
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_ERR_INIT_FAIL);
  EXPECT_TRUE(log_event_set);

  // Expect no further event
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}

/* ANF_04:
   Relevante Fehlerzustände müssen über die NET-LED signalisiert werden.
   Fall C) Zurücksetzen bzw. Neustart des Modems ist fehlgeschlagen
*/
TEST_F(MdmDiagnosticTest, SetResetError)
{
  MdmDiagnostic mdmDiag;

  // Push initialization events => 6 for SIGNAL LEDs + 1 for NET LED (see Test Initialization)
  log_tEventId log_event_id;
  bool         log_event_set;
  for(int i=0; i<7; i++)
  {
    EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  }

  // Set error RESET_FAILED
  mdmDiag.set_error_state(MdmErrorState::RESET_FAILED);
  EXPECT_EQ(mdmDiag.get_error_state(), MdmErrorState::RESET_FAILED);
  // Expect event for this error
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_ERR_RESET_FAIL);
  EXPECT_TRUE(log_event_set);

  // Expect no further event
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}

/* ANF_04:
   Relevante Fehlerzustände müssen über die NET-LED signalisiert werden.
   Fall D) Keine SIM-Karte eingelegt.
*/
TEST_F(MdmDiagnosticTest, SetNoSimError)
{
  MdmDiagnostic mdmDiag;

  // Push initialization events => 6 for SIGNAL LEDs + 1 for NET LED (see Test Initialization)
  log_tEventId log_event_id;
  bool         log_event_set;
  for(int i=0; i<7; i++)
  {
    EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  }

  // Set error SIM_REMOVED
  mdmDiag.set_error_state(MdmErrorState::SIM_REMOVED);
  EXPECT_EQ(mdmDiag.get_error_state(), MdmErrorState::SIM_REMOVED);
  // Expect event for this error
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_ERR_NOSIM);
  EXPECT_TRUE(log_event_set);

  // Expect no further event
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}

/* ANF_04:
   Relevante Fehlerzustände müssen über die NET-LED signalisiert werden.
   Fall E) Ungültige SIM-Karte eingelegt.
*/
TEST_F(MdmDiagnosticTest, SetSimError)
{
  MdmDiagnostic mdmDiag;

  // Push initialization events => 6 for SIGNAL LEDs + 1 for NET LED (see Test Initialization)
  log_tEventId log_event_id;
  bool         log_event_set;
  for(int i=0; i<7; i++)
  {
    EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  }

  // Set error SIM_FAILURE
  mdmDiag.set_error_state(MdmErrorState::SIM_FAILURE);
  EXPECT_EQ(mdmDiag.get_error_state(), MdmErrorState::SIM_FAILURE);
  // Expect event for this error
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_ERR_SIMAUTH);
  EXPECT_TRUE(log_event_set);

  // Expect no further event
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}

/* ANF_05:
   Fehlerzustände, die über die NET-LED signalisiert werden, müssen zurücksetzbar sein und 
   der eigentliche Netzstatus bzw. Mobilfunkstandard wieder angezeigt werden.
   Fall A) Netzstatus/Mobilfunkstandard war noch gar nicht gesetzt
*/
TEST_F(MdmDiagnosticTest, UnsetError)
{
  MdmDiagnostic mdmDiag;

  // Push initialization events => 6 for SIGNAL LEDs + 1 for NET LED (see Test Initialization)
  log_tEventId log_event_id;
  bool         log_event_set;
  for(int i=0; i<7; i++)
  {
    EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  }

  // Set error SIM_FAILURE
  mdmDiag.set_error_state(MdmErrorState::SIM_FAILURE);
  EXPECT_EQ(mdmDiag.get_error_state(), MdmErrorState::SIM_FAILURE);
  // Expect event for this error
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_ERR_SIMAUTH);
  EXPECT_TRUE(log_event_set);

  // Set error NONE
  mdmDiag.set_error_state(MdmErrorState::NONE);
  EXPECT_EQ(mdmDiag.get_error_state(), MdmErrorState::NONE);
  // Expect event for initial NET LED
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_NO_NET);
  EXPECT_TRUE(log_event_set);

  // Expect no further event
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}

/* ANF_06:
   Fehlerzustände müssen über die NET-LED signalisiert, 
   der eigentliche signalisierte Netzstatus bzw. Mobilfunkstandard dabei überschrieben werden.
*/
TEST_F(MdmDiagnosticTest, SetSimErrorAfterRegistration)
{
  MdmDiagnostic mdmDiag;

  // Push initialization events => 6 for SIGNAL LEDs + 1 for NET LED (see Test Initialization)
  log_tEventId log_event_id;
  bool         log_event_set;
  for(int i=0; i<7; i++)
  {
    EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  }

  // Set signal strength 0 -> 1
  mdmDiag.set_signal_quality_level(MdmSignalQualityLevel::MARGINAL);
  EXPECT_EQ(mdmDiag.get_signal_quality_level(), MdmSignalQualityLevel::MARGINAL);
  // Set access class 2G
  mdmDiag.set_access_class(MdmAccessClass::GSM);
  EXPECT_EQ(mdmDiag.get_access_class(), MdmAccessClass::GSM);
  //Expect event for access class and signal level
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_NET_2G);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_1_ON);
  EXPECT_TRUE(log_event_set);

  // Set error SIM_FAILURE
  mdmDiag.set_error_state(MdmErrorState::SIM_FAILURE);
  EXPECT_EQ(mdmDiag.get_error_state(), MdmErrorState::SIM_FAILURE);
  // Expect event for this error
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_ERR_SIMAUTH);
  EXPECT_TRUE(log_event_set);
  // Expect no event, SIGNAL LEDs must be unchanged while NET LED blinks error code
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}

/* ANF_05:
   Fehlerzustände, die über die NET-LED signalisiert werden, müssen zurücksetzbar sein und 
   der eigentliche Netzstatus bzw. Mobilfunkstandard wieder angezeigt werden.
   Fall B) Netzstatus/Mobilfunkstandard war zuvor gesetzt
*/
TEST_F(MdmDiagnosticTest, UnsetSimErrorAfterRegistration)
{
  MdmDiagnostic mdmDiag;

  // Push initialization events => 6 for SIGNAL LEDs + 1 for NET LED (see Test Initialization)
  log_tEventId log_event_id;
  bool         log_event_set;
  for(int i=0; i<7; i++)
  {
    EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  }

  // Set signal strength 0 -> 1
  mdmDiag.set_signal_quality_level(MdmSignalQualityLevel::MARGINAL);
  EXPECT_EQ(mdmDiag.get_signal_quality_level(), MdmSignalQualityLevel::MARGINAL);
  // Set access class 2G
  mdmDiag.set_access_class(MdmAccessClass::GSM);
  EXPECT_EQ(mdmDiag.get_access_class(), MdmAccessClass::GSM);
  //Expect event for access class and signal level
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_NET_2G);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_1_ON);
  EXPECT_TRUE(log_event_set);

  // Set error SIM_FAILURE
  mdmDiag.set_error_state(MdmErrorState::SIM_FAILURE);
  EXPECT_EQ(mdmDiag.get_error_state(), MdmErrorState::SIM_FAILURE);
  // Expect event for this error
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_ERR_SIMAUTH);
  EXPECT_TRUE(log_event_set);

  // Set error NONE
  mdmDiag.set_error_state(MdmErrorState::NONE);
  EXPECT_EQ(mdmDiag.get_error_state(), MdmErrorState::NONE);
  // Expect event for previous NET LED
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_NET_2G);
  EXPECT_TRUE(log_event_set);

  // Expect no further event
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}

/* ANF_05:
   Fehlerzustände, die über die NET-LED signalisiert werden, müssen zurücksetzbar sein und 
   der eigentliche Netzstatus bzw. Mobilfunkstandard wieder angezeigt werden.
   Fall C) Netzstatus/Mobilfunkstandard ändert sich während des Fehlerzustandes
*/
TEST_F(MdmDiagnosticTest, ChangeAccessClassWhileError)
{
  MdmDiagnostic mdmDiag;

  // Push initialization events => 6 for SIGNAL LEDs + 1 for NET LED (see Test Initialization)
  log_tEventId log_event_id;
  bool         log_event_set;
  for(int i=0; i<7; i++)
  {
    EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  }

  // Set signal strength 0 -> 1
  mdmDiag.set_signal_quality_level(MdmSignalQualityLevel::MARGINAL);
  EXPECT_EQ(mdmDiag.get_signal_quality_level(), MdmSignalQualityLevel::MARGINAL);
  // Set access class 2G
  mdmDiag.set_access_class(MdmAccessClass::GSM);
  EXPECT_EQ(mdmDiag.get_access_class(), MdmAccessClass::GSM);
  //Expect event for access class and signal level
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_NET_2G);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_1_ON);
  EXPECT_TRUE(log_event_set);

  // Set error SIM_FAILURE
  mdmDiag.set_error_state(MdmErrorState::SIM_FAILURE);
  EXPECT_EQ(mdmDiag.get_error_state(), MdmErrorState::SIM_FAILURE);
  // Expect event for this error
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_ERR_SIMAUTH);
  EXPECT_TRUE(log_event_set);

  // Set access class 3G
  mdmDiag.set_access_class(MdmAccessClass::UMTS);
  EXPECT_EQ(mdmDiag.get_access_class(), MdmAccessClass::UMTS);
  // Expect no event while error is still set and NET LED blinks error code
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));

  // Set error NONE
  mdmDiag.set_error_state(MdmErrorState::NONE);
  EXPECT_EQ(mdmDiag.get_error_state(), MdmErrorState::NONE);
  // Expect event for NET LED according to changed access class
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_NET_3G);
  EXPECT_TRUE(log_event_set);

  // Expect no further event
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}

/* ANF_07:
   Fehlerzustände müssen über die NET-LED signalisiert, 
   bei mehreren Fehler muss der letzte Fehlerzustand den vorherigen überschreiben.
*/
TEST_F(MdmDiagnosticTest, ChangeError)
{
  MdmDiagnostic mdmDiag;

  // Push initialization events => 6 for SIGNAL LEDs + 1 for NET LED (see Test Initialization)
  log_tEventId log_event_id;
  bool         log_event_set;
  for(int i=0; i<7; i++)
  {
    EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  }

  // Set signal strength 0 -> 1
  mdmDiag.set_signal_quality_level(MdmSignalQualityLevel::MARGINAL);
  EXPECT_EQ(mdmDiag.get_signal_quality_level(), MdmSignalQualityLevel::MARGINAL);
  // Set access class 2G
  mdmDiag.set_access_class(MdmAccessClass::GSM);
  EXPECT_EQ(mdmDiag.get_access_class(), MdmAccessClass::GSM);
  //Expect event for access class and signal level
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_NET_2G);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_1_ON);
  EXPECT_TRUE(log_event_set);

  // Set error SIM_FAILURE
  mdmDiag.set_error_state(MdmErrorState::SIM_FAILURE);
  EXPECT_EQ(mdmDiag.get_error_state(), MdmErrorState::SIM_FAILURE);
  // Expect event for this error
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_ERR_SIMAUTH);
  EXPECT_TRUE(log_event_set);

  // Set new error RESET_FAILED
  mdmDiag.set_error_state(MdmErrorState::RESET_FAILED);
  EXPECT_EQ(mdmDiag.get_error_state(), MdmErrorState::RESET_FAILED);
  // Expect event for new error
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_ERR_RESET_FAIL);
  EXPECT_TRUE(log_event_set);

  // Expect no further event
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}

/* ANF_01:
   Statusänderungen der LEDs müssen differentiell signalisiert, d.h. es dürfen nicht jedes mal alle LEDS vollständig neu gesetzt werden.
   Fall C) Der gleiche Fehler wird erneut gesetzt, z.B. in Folge eines erneuten Zugriffsversuches auf eine ungültige SIM-Karte
*/
TEST_F(MdmDiagnosticTest, SetSimFailureTwice)
{
  MdmDiagnostic mdmDiag;

  // Push initialization events => 6 for SIGNAL LEDs + 1 for NET LED (see Test Initialization)
  log_tEventId log_event_id;
  bool         log_event_set;
  for(int i=0; i<7; i++)
  {
    EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  }

  // Set signal strength 0 -> 1
  mdmDiag.set_signal_quality_level(MdmSignalQualityLevel::MARGINAL);
  EXPECT_EQ(mdmDiag.get_signal_quality_level(), MdmSignalQualityLevel::MARGINAL);
  // Set access class 2G
  mdmDiag.set_access_class(MdmAccessClass::GSM);
  EXPECT_EQ(mdmDiag.get_access_class(), MdmAccessClass::GSM);
  //Expect event for access class and signal level
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_NET_2G);
  EXPECT_TRUE(log_event_set);
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_OPER_SIGNAL_1_ON);
  EXPECT_TRUE(log_event_set);

  // Set error SIM_FAILURE
  mdmDiag.set_error_state(MdmErrorState::SIM_FAILURE);
  EXPECT_EQ(mdmDiag.get_error_state(), MdmErrorState::SIM_FAILURE);
  // Expect event for this error
  EXPECT_TRUE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
  EXPECT_EQ(log_event_id, DIAG_3GMM_ERR_SIMAUTH);
  EXPECT_TRUE(log_event_set);

  // Set same error again
  mdmDiag.set_error_state(MdmErrorState::SIM_FAILURE);
  // Expect no event when error is not changed
  EXPECT_FALSE(test_log_EVENT_LogId_get_first(&log_event_id, &log_event_set));
}

//---- End of source file ------------------------------------------------------

