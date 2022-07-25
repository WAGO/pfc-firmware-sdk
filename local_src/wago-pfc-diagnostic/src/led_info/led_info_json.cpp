//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///  \file     led_info_json.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include "led_info_json.h"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <cstdlib>
#include <syslog.h>

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// variables' and constants' definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------
int hello(int j)
{
  for(auto i=0;i<j;i++)
  {
    std::cout << "hello " << i << std::endl;
  }
}


typedef struct {
    char  name[4] ;
    char  startid[11];
}tLeds;
typedef struct{
    char  groupname[32];
    tLeds leds[42];
}tLedGroups;

struct tLED{
    std::string name;
    std::string startid;
    std::vector<std::string> alias;
};

void from_json(const nlohmann::json& j, tLED& led) {
  led.name    = j.at("name");
  led.startid = j.at("startid");
  if(j.count("alias") != 0)
  {
    led.alias   = j.at("alias").get<std::vector<std::string>>();
  }
}

struct LEDGroup {
    std::string name;
    std::vector<tLED> leds;
};

void from_json(const nlohmann::json& j, LEDGroup& grp) {
  grp.name = j.at("name");
  grp.leds = j.at("leds").get<std::vector<tLED>>();
}

struct LEDSetup {
    std::vector<LEDGroup> groups;
};

void from_json(const nlohmann::json& j, LEDSetup& stp) {
  stp.groups = j.at("ledgroups").get<std::vector<LEDGroup>>();
}

GList * getInfoLedListFromFile(const char * led_info_file_name)
{
  GList * ledlist = NULL;
  std::ifstream led_info_stream(led_info_file_name);
  if(led_info_stream.good())
  {
    try {
      nlohmann::json led_info_json = nlohmann::json::parse(led_info_stream);
      LEDSetup setup = led_info_json.get<LEDSetup >();
      for(auto &group: setup.groups)
      {
         for(auto &led: group.leds)
         {
           tLEDInfo * newInfo =(tLEDInfo*)malloc(sizeof(tLEDInfo));
           newInfo->name=strdup(led.name.c_str());
           newInfo->startid=std::strtoul(led.startid.c_str(),nullptr,16);
           newInfo->alias = NULL;
           for(auto &alias:led.alias)
           {
             newInfo->alias = g_list_append(newInfo->alias,strdup(alias.c_str()));
           }
           ledlist = g_list_append(ledlist, newInfo);
         }
      }

    }
    catch(nlohmann::json::exception &e)
    {
      syslog(LOG_USER|LOG_ERR,"ERROR: Parsing Settings-File failed");
      syslog(LOG_USER|LOG_ERR,"Error ID: %d",e.id);
      syslog(LOG_USER|LOG_ERR,"Message: %s",e.what());
    }
  }
  else
  {
    syslog(LOG_USER|LOG_ERR,"ERROR: Unable to Read LED-Info-File (%s)",led_info_file_name);
  }

  return ledlist;
}

static void freeAliasListElement(gpointer data)
{
  free(data);
}

static void freeInfoLedListElement(gpointer data)
{
  tLEDInfo * info = (tLEDInfo*)data;
  free(info->name);
  g_list_free_full (info->alias,freeAliasListElement);
  free(info);
}


void cleanUpInfoLedList(GList * list)
{
  g_list_free_full (list,freeInfoLedListElement);
}


//---- End of source file ------------------------------------------------------

