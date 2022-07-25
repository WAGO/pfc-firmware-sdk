//------------------------------------------------------------------------------
/// Copyright (c) 2000 - 2022 WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     get_dns_server.c
///
///  \brief    
///
///  \author   Stefanie Meihöfer : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "config_tool_lib.h"

#define SHOW_ERRORS                         0

#define MAX_LENGTH_DNS_SERVER_NAME          200

#define RESOLV_CONF_FILENAME                "/etc/resolv.conf"
#define STATIC_DNS_FILENAME                 "/etc/dns_static_list"
#define SHOW_FORMS_INPUT_COMMAND            "show-forms"
#define SHOW_ALL_INPUT_COMMAND              "all"
#define GET_COMPLETE_COMMAND                "get-complete"
#define GET_DYNAMIC_COMMAND                 "get-dynamic"
#define DHCP_TEMPLATE                       "dhcp-bootp-data-"

typedef struct
{
  GString *file_path;
  GSList *dns_servers;
  GError *error;
} dns_file_result;

void ShowHelpText(void)
//
// Show describtion and usage of program on stdout
//
{
  printf("\n* Get the names of the configured DNS-server *\n\n");
  printf("Usage: get_dns_server < DNS-Server-Number | \"%s\" | \"%s\" | \"%s\" >\n\n", SHOW_FORMS_INPUT_COMMAND,
         SHOW_ALL_INPUT_COMMAND, GET_COMPLETE_COMMAND, GET_DYNAMIC_COMMAND);
  printf("DNS-Server-Number: sequential number of DNS-server in resolv.conf-file\n");
  printf("\n\"%s\": return values of static assigned DNS server in json array\n", SHOW_ALL_INPUT_COMMAND);
  printf("\n\"%s\": if this text is given as parameter, the html-code for\n", SHOW_FORMS_INPUT_COMMAND);
  printf("a selection-menu about all existing DNS-Server is written to stdout\n");
  printf("\n\"%s\": return values of all DNS server in json array\n", GET_COMPLETE_COMMAND);
  printf("\n\"%s\": return values of all DNS server gotten from DHCP server (tab-separated list)\n",
         GET_DYNAMIC_COMMAND);
  printf("\n\n");
}

int GetDnsServerNameByNr(int requestedDnsServerNr,
                         char *pOutputString,
                         char *fileName)
//
// Get name of dns-server from resolv.conf-file by its sequential number in file.
//
// input: requestetDnsServerNr: sequential number of dns-server in resolv.conf-file
//
// output: pOutputString: string with the dns-server-name
//
// return: error-code
//
{
  char *pFileContent = NULL;
  int status = SUCCESS;

  // check input-parameter
  if((requestedDnsServerNr == 0) || (pOutputString == NULL))
  {
    return (INVALID_PARAMETER);
  }

  // initialise output-string
  pOutputString[0] = '\0';

  // get the content of the resolv-conf-file in a buffer 
  pFileContent = FileContent_Get(fileName);

  if(pFileContent == NULL)
  {
    status = FILE_OPEN_ERROR;
  }
  else
  {
    // loop over the lines of resolv-file, until the requested parameter was found
    // stop if end of file was reached or the actual tested dns-server-number is already bigger than the requested one
    char confFileLine[MAX_LINE_LENGTH] = "";
    int lineNr = 0;
    int dnsServerNr = 0;

    lineNr = 1;
    dnsServerNr = 1;
    while((SUCCESS == SystemCall_GetLineByNr(pFileContent, lineNr, confFileLine)) && (strlen(pOutputString) == 0)
          && (dnsServerNr <= requestedDnsServerNr))
    {
      char dnsServerIntroString[] = "nameserver ";

      //printf("%s\n", confFileLine);
      if(strstr(confFileLine, dnsServerIntroString) != NULL)
      {
        if(dnsServerNr == requestedDnsServerNr)
        {
          strncpy(pOutputString, strstr(confFileLine, dnsServerIntroString) + strlen(dnsServerIntroString),
          MAX_LENGTH_DNS_SERVER_NAME);
          CutWord(pOutputString, MAX_LENGTH_DNS_SERVER_NAME);
        }
        ++dnsServerNr;
      }
      ++lineNr;
    }

    FileContent_Destruct(&pFileContent);
  }

  return (status);
}

static int ShowDnsServerForms(char *filename)
//
// Deliver the html-code of one form for each dns-server from resolv.conf-file. Each form includes the code to change or delete the dns-server.
//
// return: error-code
//
{
  char dnsServerName[MAX_LENGTH_DNS_SERVER_NAME] = "";
  int dnsServerNr = 1;
  int status = SUCCESS;

  // loop over all dns-server of resolv.conf-file, get the dns-server-names depending on their sequential number
  status = GetDnsServerNameByNr(dnsServerNr, dnsServerName, filename);
  if(SUCCESS != status)
    return status;

  while(strlen(dnsServerName) > 0)
  {
    printf("\n");
    printf("        <form action=\"tcpip.ssi\" method=\"POST\">\n");
    printf("          <input type=\"hidden\" name=\"form-name\" value=\"edit_dns_server\">\n");
    printf("          <input type=\"hidden\" name=\"dns-server-nr\" value=\"%d\" >\n", dnsServerNr);
    printf("          <tr>\n");
    printf("            <td>DNS-Server %d:</td>\n", dnsServerNr);
    printf(
        "            <td><input class=\"formular_feld\" type=\"TEXT\" maxlength=\"32\" name=\"dns-server-name\" value=\"%s\" ></td>\n",
        dnsServerName);
    printf("            <td>\n");
    printf("              <input class=\"tab_formular_button\" type=\"SUBMIT\" value=\"CHANGE\" name=\"change\">\n");
    printf("              <input class=\"tab_formular_button\" type=\"SUBMIT\" value=\"DELETE\" name=\"delete\">\n");
    printf("            </td>\n");
    printf("          </tr>\n");
    printf("        </form>\n");

    ++dnsServerNr;
    GetDnsServerNameByNr(dnsServerNr, dnsServerName, filename);
  }
  return SUCCESS;
}

static void __freeGStr(gpointer data)
{
  g_string_free((GString*)data, TRUE);
}

// NOTE: GCompareFunc shall return 0 if a equals b
static gint __compareGStrings(gconstpointer a,
                              gconstpointer b)
{
  return (gint)!g_string_equal((const GString*)a, (const GString*)b);
}

static void __printDnsServerFromDHCP(gpointer data,
                                     gpointer user_data)
{
  GString *dnsServerIp = (GString*)data;
  GSList *staticDnsServerList = (GSList*)user_data;

  if(!g_slist_find_custom(staticDnsServerList, dnsServerIp, __compareGStrings))
  {
    printf("%s\t", (char*)dnsServerIp->str);
  }
}

static void FreeDnsFileResult(gpointer item)
{
  dns_file_result *result = (dns_file_result*)item;
  g_string_free((GString*)result->file_path, TRUE);
  if(result->error)
  {
    g_error_free((GError*)result->error);
  }

  g_slist_free_full(result->dns_servers, __freeGStr);
}

static gint CompareDnsFileResultByFilePath(gconstpointer a,
                                           gconstpointer b)
{
  dns_file_result *lhs = (dns_file_result*)a;
  dns_file_result *rhs = (dns_file_result*)b;

  return g_ascii_strcasecmp(lhs->file_path->str, rhs->file_path->str);
}

static void PrintIterator(gpointer item)
{
  GString *i = (GString*)item;
  printf("%s\t", i->str);
}

static void PrintDnsFileResultIterator(gpointer item)
{
  dns_file_result *result = (dns_file_result*)item;
  g_slist_foreach(result->dns_servers, (GFunc)PrintIterator, NULL);
}

static void CollectErrorMessages(gpointer item,
                                 gpointer result_string)
{
  dns_file_result *result = (dns_file_result*)item;
  GString *message = (GString*)result_string;

  if(result->error)
  {
    g_string_append_printf(message, "%s\n", result->error->message);
  }
}

static void ReadDnsServersFromFile(gpointer item)
{
  dns_file_result *result = (dns_file_result*)item;

  gchar *content;
  gboolean success = g_file_get_contents(result->file_path->str, &content, NULL, &(result->error));

  if(success)
  {
    GMatchInfo *matchInfo;
    GRegex *regex;

    regex = g_regex_new("DNS_SERVER_\\d+=([\\w.]+)", 0, 0, &(result->error));
    g_regex_match(regex, content, 0, &matchInfo);

    while(g_match_info_matches(matchInfo))
    {
      gchar *dns_server = g_match_info_fetch(matchInfo, 1);

      result->dns_servers = g_slist_append(result->dns_servers, g_string_new((const gchar*)dns_server));

      g_match_info_next(matchInfo, &(result->error));
      g_free(dns_server);
    }

    g_free(content);
  }
}

static void ReadDhcpLeaseFiles(GSList **dns_file_result_list,
                               GError **error)
{
  GDir *directory = NULL;
  const gchar *file_name;

  directory = g_dir_open("/tmp/", 0, error);

  if(directory != NULL)
  {

    while(file_name = g_dir_read_name(directory))
    {
      if(g_str_has_prefix(file_name, DHCP_TEMPLATE))
      {

        dns_file_result *result = (dns_file_result*)malloc(sizeof(dns_file_result));
        result->dns_servers = NULL;
        result->file_path = g_string_new("");
        result->error = NULL;

        g_string_printf(result->file_path, "/tmp/%s", file_name);
        *dns_file_result_list = g_slist_insert_sorted(*dns_file_result_list, result,
                                                      (GCompareFunc)CompareDnsFileResultByFilePath);
      }
    }
  }
}

static int GetDnsServersViaDhcp()
{
  GSList *dns_file_result_list = NULL;
  GError *error = NULL;
  int status = SUCCESS;

  ReadDhcpLeaseFiles(&dns_file_result_list, &error);

  if(error == NULL)
  {
    g_slist_foreach(dns_file_result_list, (GFunc)ReadDnsServersFromFile, NULL);

    g_slist_foreach(dns_file_result_list, (GFunc)PrintDnsFileResultIterator, NULL);

    GString *error_messages = g_string_new("");

    g_slist_foreach(dns_file_result_list, CollectErrorMessages, error_messages);

    if(error_messages->len > 0)
    {
      SetLastError(error_messages->str);
      status = FILE_READ_ERROR;
    }

    g_slist_free_full(dns_file_result_list, FreeDnsFileResult);

  }
  else
  {
    SetLastError(error->message);
    status = NOT_FOUND;
  }

  return status;
}

static int GetDnsServerJSON(char *filename)
//
// Get values of all DNS server in one go
//
// return: error-code
//
{
  char dnsServerName[MAX_LENGTH_DNS_SERVER_NAME] = "";
  int dnsServerNr = 1;
  int status = SUCCESS;

  // loop over all dns-server of resolv.conf-file, get the dns-server-names depending on their sequential number
  status = GetDnsServerNameByNr(dnsServerNr, dnsServerName, filename);
  if(SUCCESS != status)
    return status;

  printf("[ ");

  while(strlen(dnsServerName) > 0)
  {
    // more than one line -> write array member separator
    if(dnsServerNr > 1)
    {
      printf(", ");
    }

    //printf("%d", dnsServerNr);
    printf("\"%s\"", dnsServerName);

    ++dnsServerNr;
    GetDnsServerNameByNr(dnsServerNr, dnsServerName, filename);
  }

  printf(" ]");

  return SUCCESS;
}

static int file_exist(char *filename)
{
  struct stat buffer;
  return (stat(filename, &buffer) == 0);
}

int main(int argc,
         char **argv)
{
  int status = SUCCESS;
  int hasStaticFile = 0;

  // help-text requested?
  if((argc == 2) && ((strcmp(argv[1], "--help") == 0) || strcmp(argv[1], "-h") == 0))
  {
    ShowHelpText();
  }
  else
  {
    // check if the count of parameters is valid
    if(argc < 2)
    {
      status = MISSING_PARAMETER;
    }
    else
    {
      // check if static file exist else fallback to old method
      if(file_exist(STATIC_DNS_FILENAME))
      {
        hasStaticFile = 1;
      }

      // the dns-server-forms should be delivered
      if(strcmp(argv[1], SHOW_FORMS_INPUT_COMMAND) == 0)
      {
        if(hasStaticFile)
        {
          status = ShowDnsServerForms(STATIC_DNS_FILENAME);
        }
        else
        {
          status = ShowDnsServerForms(RESOLV_CONF_FILENAME);
        }
      }

      else if(strcmp(argv[1], SHOW_ALL_INPUT_COMMAND) == 0)
      {
        if(hasStaticFile)
        {
          //Use old interface...but only show the static ones.
          status = GetDnsServerJSON(STATIC_DNS_FILENAME);
        }
        else
        {
          status = GetDnsServerJSON(RESOLV_CONF_FILENAME);
        }
      }

      else if(strcmp(argv[1], GET_COMPLETE_COMMAND) == 0)
      {
        status = GetDnsServerJSON(RESOLV_CONF_FILENAME);
      }
      else if(strcmp(argv[1], GET_DYNAMIC_COMMAND) == 0)
      {
        //status = GetDnsServersDhcp(RESOLV_CONF_FILENAME, STATIC_DNS_FILENAME);
        status = GetDnsServersViaDhcp();
      }

      // one single dns-server-name should be delivered
      else
      {
        int dnsServerNr = 0;
        char outputString[MAX_LENGTH_DNS_SERVER_NAME] = "";

        // get the requested sequential number from commandline-parameter and determin the dns-server-name
        sscanf(argv[1], "%d", &dnsServerNr);
        status = GetDnsServerNameByNr(dnsServerNr, outputString, STATIC_DNS_FILENAME);
        printf("%s", outputString);
      }
    }
  }

#if SHOW_ERRORS
  if(status != SUCCESS)
  {
    ShowErrorText(status, "");
    ShowHelpText();
  }
  #endif

  return (status);
}
