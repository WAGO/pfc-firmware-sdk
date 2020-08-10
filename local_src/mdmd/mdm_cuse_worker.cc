//------------------------------------------------------------------------------
// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <mutex>

#include <typelabel_API.h>
#include <poll.h>

#include "dbus_client.h"
#include "mdmd_log.h"
#include "mdm_cuse.h"
#include "mdm_cuse_worker.h"
#include "mdm_cuse_text_data.hpp"

#define MAX_REVISIONS_FILE_SIZE                  (1024*1024)

class MdmCuseWorker : public DBusClient
{
  private:
    char          _readbuf[1024];
    int           _expected_sms_pdu_length;
    std::string   _result_pipe_name;
    std::string   _request_read_buffer;
    std::string   _report_read_buffer;
    std::string   _ati_order;
    std::string   _ati_revision;
    std::mutex    _write_result_mutex;

    void dbus_get_sms_report_config(std::string &result);
    void dbus_set_sms_report_config(std::string const &command, std::string &result);
    void dbus_get_sms_storage(std::string &result);
    void dbus_set_sms_storage(std::string const &command, std::string &result);
    void dbus_get_sms_format(std::string &result);
    void dbus_set_sms_format(std::string const &command, std::string &result);
    void dbus_list_sms(std::string const &command, std::string &result);
    void dbus_read_sms(std::string const &command, std::string &result);
    void dbus_delete_sms(std::string const &command, std::string &result);
    void dbus_send_sms_prepare(std::string const &command, int &length, std::string &result);
    void dbus_send_sms(std::string const &pdu, int length, std::string &result);
    void dbus_get_reg_state(std::string &result);
    void dbus_get_signal_quality(std::string &result);
    void dbus_get_sim_status(std::string &result);

    std::string get_revision();
    void write_result(std::string &result);
    void process_requests();
    void process_reports();

  public:
    MdmCuseWorker() :
      DBusClient("de.wago.mdmd", "/de/wago/mdmd", "de.wago.mdmd", 190000 /*msec*/),
      _expected_sms_pdu_length(0),
      _result_pipe_name(MDM_CUSE_FIFO_RESULT)
    {
      char *c_order = typelabel_QUICK_GetValue("ORDER");
      if ( c_order )
      {
        _ati_order = c_order;
        typelabel_QUICK_FreeValue(c_order);
      }
      _ati_revision = get_revision();
      _request_read_buffer.reserve(1024);
      _report_read_buffer.reserve(1024);
    }
    ~MdmCuseWorker() {}

    void read_request(int fd);
    void read_report(int fd);

};


void MdmCuseWorker::dbus_get_sms_report_config(std::string &result)
{
  const gchar *method = "GetSmsReportConfig";
  mdmd_Log(MD_LOG_DBG2, "Call %s\n", method);
  GVariant *callresult = nullptr;
  if (0 == call_sync(method, nullptr, /* no method parameters */
                     &callresult) )
  {
    if (callresult != nullptr)
    {
      char printbuf[80];
      int mode, mt, bm, ds, bfr;
      g_variant_get(callresult, "(iiiii)", &mode, &mt, &bm, &ds, &bfr);
      g_variant_unref(callresult);
      snprintf(printbuf, sizeof(printbuf), "\r\n+CNMI: %d,%d,%d,%d,%d\r\n", mode, mt, bm, ds, bfr);
      result = printbuf;
      result += "\r\nOK\r\n";
    }
  }
}

void MdmCuseWorker::dbus_set_sms_report_config(std::string const &command, std::string &result)
{
  if (command.length() == 17) //expect 5 single digit parameters (no optional parameter handling)
  {
    const gchar *method = "SetSmsReportConfig";
    mdmd_Log(MD_LOG_DBG2, "Call %s (via %s)\n", method, command.c_str());
    GVariant *callresult = nullptr;
    long int mode = strtol( command.substr(8,1).c_str(), nullptr, 10 );
    long int mt = strtol( command.substr(10,1).c_str(), nullptr, 10 );
    long int bm = strtol( command.substr(12,1).c_str(), nullptr, 10 );
    long int ds = strtol( command.substr(14,1).c_str(), nullptr, 10 );
    long int bfr = strtol( command.substr(16,1).c_str(), nullptr, 10 );
    if (0 == call_sync(method, g_variant_new("(iiiii)", (int)mode, (int)mt, (int)bm, (int)ds, (int)bfr),
                       &callresult) )
    {
      if (callresult != nullptr)
      {
        g_variant_unref(callresult);
      }
      result = "\r\nOK\r\n";
    }
  }
}

void MdmCuseWorker::dbus_get_sms_storage(std::string &result)
{
  const gchar *method = "GetSmsStorage";
  mdmd_Log(MD_LOG_DBG2, "Call %s\n", method);
  GVariant *callresult = nullptr;
  if (0 == call_sync(method, nullptr, /* no method parameters */
                     &callresult) )
  {
    if (callresult != nullptr)
    {
      char printbuf[112];
      char *c_mem1 = nullptr;
      int  used1;
      int  total1;
      char *c_mem2 = nullptr;
      int  used2;
      int  total2;
      char *c_mem3 = nullptr;
      int  used3;
      int  total3;
      g_variant_get(callresult, "(siisiisii)", &c_mem1, &used1, &total1, &c_mem2, &used2, &total2, &c_mem3, &used3, &total3);
      g_variant_unref(callresult);
      snprintf(printbuf, sizeof(printbuf), "\r\n+CPMS: \"%s\",%d,%d,\"%s\",%d,%d,\"%s\",%d,%d\r\n",
               c_mem1, used1, total1, c_mem2, used2, total2, c_mem3, used3, total3);
      result = printbuf;
      result += "\r\nOK\r\n";
    }
  }
}

void MdmCuseWorker::dbus_set_sms_storage(std::string const &command, std::string &result)
{
  if (command.length() == 22) //expect 3 double character string parameters
  {
    std::string mem1 = command.substr(9,2);
    std::string mem2 = command.substr(14,2);
    std::string mem3 = command.substr(19,2);
    //allowed storages: SM, ME, MT
    if (((mem1.compare("SM") == 0) || (mem1.compare("ME") == 0) || (mem1.compare("MT") == 0)) &&
        ((mem2.compare("SM") == 0) || (mem2.compare("ME") == 0) || (mem2.compare("MT") == 0)) &&
        ((mem3.compare("SM") == 0) || (mem3.compare("ME") == 0) || (mem3.compare("MT") == 0)) )
    {
      const gchar *method = "SetSmsStorage";
      mdmd_Log(MD_LOG_DBG2, "Call %s (via %s)\n", method, command.c_str());
      GVariant *callresult = nullptr;
      if (0 == call_sync(method, g_variant_new("(sss)", mem1.c_str(), mem2.c_str(), mem3.c_str()),
                         &callresult) )
      {
        if (callresult != nullptr)
        {
          g_variant_unref(callresult);
        }
        result = "\r\nOK\r\n";
      }
    }
  }
}

void MdmCuseWorker::dbus_get_sms_format(std::string &result)
{
  result.reserve(12);
  const gchar *method = "GetSmsFormat";
  mdmd_Log(MD_LOG_DBG2, "Call %s\n", method);
  GVariant *callresult = nullptr;
  if (0 == call_sync(method, nullptr, /* no method parameters */
                     &callresult) )
  {
    if (callresult != nullptr)
    {
      char printbuf[24];
      int format;
      g_variant_get(callresult, "(i)", &format);
      g_variant_unref(callresult);
      snprintf(printbuf, sizeof(printbuf), "\r\n+CMGF: %d\r\n", format);
      result = printbuf;
      result += "\r\nOK\r\n";
    }
  }
}

void MdmCuseWorker::dbus_set_sms_format(std::string const &command, std::string &result)
{
  if (command.length() == 9) //expect one single digit parameter
  {
    const gchar *method = "SetSmsFormat";
    mdmd_Log(MD_LOG_DBG2, "Call %s (via %s)\n", method, command.c_str());
    GVariant *callresult = nullptr;
    long int format = strtol( command.substr(8,1).c_str(), nullptr, 10 );
    if (0 == call_sync(method, g_variant_new("(i)", (int)format),
                       &callresult) )
    {
      if (callresult != nullptr)
      {
        g_variant_unref(callresult);
      }
      result = "\r\nOK\r\n";
    }
  }
}

void MdmCuseWorker::dbus_list_sms(std::string const &command, std::string &result)
{
  if (command.length() == 9) //expect one single digit parameter
  {
    const gchar *method = "ListSms";
    mdmd_Log(MD_LOG_DBG2, "Call %s (via %s)\n", method, command.c_str());
    GVariant *callresult = nullptr;
    //long int state = strtol( command.substr(8).c_str(), nullptr, 10 );
    //if (0 == call_sync(method, g_variant_new("(i)", (int)state),
    // FIXME: atoi is not a secure function!
    int state = atoi( command.substr(8).c_str() );
    if (0 == call_sync(method, g_variant_new("(i)", state),
                       &callresult) )
    {
      if (callresult != nullptr)
      {
        GVariant *sms_list = g_variant_get_child_value( callresult, 0 );
        if (sms_list != nullptr)
        {
          gsize sms_list_size = g_variant_n_children(sms_list);
          gsize i;
          for (i = 0; i < sms_list_size; i++)
          {
            int sms_index;
            int length;
            char * c_pdu = nullptr;
            g_variant_get_child( sms_list, i, "(iiis)", &sms_index, &state, &length, &c_pdu);
            if (c_pdu != nullptr)
            {
              char printbuf[56];
              snprintf(printbuf, sizeof(printbuf), "\r\n+CMGL: %d,%d,,%d\r\n", sms_index, state, length);
              result += printbuf;
              result += c_pdu;
              g_free(c_pdu);
            }
          }
          if (i > 0) result += "\r\n";
          g_variant_unref(sms_list);
          result += "\r\nOK\r\n";
        }
        g_variant_unref(callresult);
      }
    }
  }
}

void MdmCuseWorker::dbus_read_sms(std::string const &command, std::string &result)
{
  if (command.length() >= 9) //expect at least one numeric parameter
  {
    const gchar *method = "ReadSms";
    mdmd_Log(MD_LOG_DBG2, "Call %s (via %s)\n", method, command.c_str());
    GVariant *callresult = nullptr;
    long int sms_index = strtol( command.substr(8).c_str(), nullptr, 10 );
    if (0 == call_sync(method, g_variant_new("(i)", (int)sms_index),
                       &callresult) )
    {
      if (callresult != nullptr)
      {
        int state;
        int length;
        char * c_pdu = nullptr;
        g_variant_get(callresult, "(iis)", &state, &length, &c_pdu);
        g_variant_unref(callresult);
        if (c_pdu != nullptr)
        {
          char printbuf[40];
          snprintf(printbuf, sizeof(printbuf), "\r\n+CMGR: %d,,%d\r\n", state, length);
          result = printbuf;
          result += c_pdu;
          result += "\r\n";
          g_free(c_pdu);
          result += "\r\nOK\r\n";
        }
      }
    }
  }
}

void MdmCuseWorker::dbus_delete_sms(std::string const &command, std::string &result)
{
  if (command.length() >= 9) //expect at least one numeric parameter
  {
    const gchar *method = "DeleteSms";
    mdmd_Log(MD_LOG_DBG2, "Call %s (via %s)\n", method, command.c_str());
    GVariant *callresult = nullptr;
    long int sms_index;
    long int delflag;
    size_t pos = command.find(',');
    if (pos == std::string::npos)
    {
      sms_index = strtol( command.substr(8).c_str(), nullptr, 10 );
      delflag = 0;
    }
    else
    {
      sms_index = strtol( command.substr(8, (pos - 8)).c_str(), nullptr, 10 );
      delflag = strtol( command.substr(pos +1).c_str(), nullptr, 10 );
    }
    if (0 == call_sync(method, g_variant_new("(ii)", (int)sms_index, (int)delflag),
                       &callresult) )
    {
      if (callresult != nullptr)
      {
        g_variant_unref(callresult);
      }
      result = "\r\nOK\r\n";
    }
  }
}

void MdmCuseWorker::dbus_send_sms_prepare(std::string const &command, int &length, std::string &result)
{
  if (command.length() >= 9) //expect at least one numeric parameter
  {
    mdmd_Log(MD_LOG_DBG2, "Call SendSmsPrepare (via %s)\n", command.c_str());
    length = (int)strtol( command.substr(8).c_str(), nullptr, 10 );
    if (length > 0)
    {
      result = "> ";
    }
    else
    {
      length = 0;
    }
  }
}

void MdmCuseWorker::dbus_send_sms(std::string const &pdu, int length, std::string &result)
{
  const gchar *method = "SendSms";
  mdmd_Log(MD_LOG_DBG2, "Call %s\n", method);
  GVariant *callresult = nullptr;
  if (0 == call_sync(method, g_variant_new("(is)", length, pdu.c_str()),
                     &callresult) )
  {
    if (callresult != nullptr)
    {
      char printbuf[24];
      int msgref;
      g_variant_get(callresult, "(i)", &msgref);
      g_variant_unref(callresult);
      snprintf(printbuf, sizeof(printbuf), "\r\n+CMGS: %d\r\n", msgref);
      result = printbuf;
      result += "\r\nOK\r\n";
    }
  }
}

void MdmCuseWorker::dbus_get_reg_state(std::string &result)
{
  const gchar *method = "GetOperState";
  mdmd_Log(MD_LOG_DBG2, "Call %s\n", method);
  GVariant *callresult = nullptr;
  if (0 == call_sync(method, nullptr, /* no method parameters */
                     &callresult) )
  {
    if (callresult != nullptr)
    {
      char printbuf[24];
      int regstate, selmode, operid, act, quality;
      char * c_name = nullptr;
      char * c_lac = nullptr;
      char * c_cid = nullptr;
      std::string lac;
      std::string cid;
      g_variant_get(callresult, "(iiiiisss)", &regstate, &selmode, &operid, &act, &quality, &c_name, &c_lac, &c_cid);
      g_variant_unref(callresult);
      if (c_name != nullptr)
      {
        g_free(c_name);
      }
      if (c_lac != nullptr)
      {
        lac = c_lac;
        g_free(c_lac);
      }
      if (c_cid != nullptr)
      {
        cid = c_cid;
        g_free(c_cid);
      }
      snprintf(printbuf, sizeof(printbuf), "\r\n+CREG: 2,%d", regstate);
      result = printbuf;
      if ((regstate == 1) || (regstate == 5))
      {
        result += ",\"";
        result += lac;
        result += "\",\"";
        result += cid;
        result += "\"";
        if (act >= 0)
        {
          snprintf(printbuf, sizeof(printbuf), ",%d", act);
          result += printbuf;
        }
      }
      result += "\r\n\r\nOK\r\n";
    }
  }
}

void MdmCuseWorker::dbus_get_signal_quality(std::string &result)
{
  const gchar *method = "GetSignalQuality";
  mdmd_Log(MD_LOG_DBG2, "Call %s\n", method);
  GVariant *callresult = nullptr;
  if (0 == call_sync(method, nullptr, /* no method parameters */
                     &callresult) )
  {
    if (callresult != nullptr)
    {
      char printbuf[40];
      int rssi, ber;
      g_variant_get(callresult, "(ii)", &rssi, &ber);
      g_variant_unref(callresult);
      snprintf(printbuf, sizeof(printbuf), "\r\n+CSQ: %d,%d\r\n", rssi, ber);
      result = printbuf;
      result += "\r\nOK\r\n";
    }
  }
}

void MdmCuseWorker::dbus_get_sim_status(std::string &result)
{
  const gchar *method = "GetSimState";
  mdmd_Log(MD_LOG_DBG2, "Call %s\n", method);
  GVariant *callresult = nullptr;
  if (0 == call_sync(method, nullptr, /* no method parameters */
                     &callresult) )
  {
    if (callresult != nullptr)
    {
      int state, attempts;
      g_variant_get(callresult, "(ii)", &state, &attempts);
      g_variant_unref(callresult);
      switch (state)
      {
        case 0:
          //no break; fall through
        default:
          break;
        case 1:
          result = "\r\n+CPIN: SIM PIN\r\n\r\nOK\r\n";
          break;
        case 2:
          result = "\r\n+CPIN: SIM PUK\r\n\r\nOK\r\n";
          break;
        case 3:
          result = "\r\n+CPIN: READY\r\n\r\nOK\r\n";
          break;
      }
    }
  }
}


std::string MdmCuseWorker::get_revision()
{
  std::string revision;
  const char *szFilename = "/etc/REVISIONS"; //todo: define file name in global header
  FILE* fFile = fopen(szFilename, "r");
  struct  stat fileAttributes;
  if(fFile != nullptr)
  {
    // get file size to allocate read buffer
    if(stat(szFilename, &fileAttributes) != -1)
    {
      // allocate buffer for all characters of file and additionally end-of-string (but not more than defined max.)
      size_t const fileSize = fileAttributes.st_size <= MAX_REVISIONS_FILE_SIZE ? (size_t)fileAttributes.st_size
                                                                                : MAX_REVISIONS_FILE_SIZE;
      char *szReadBuffer = (char*)malloc(fileSize + 1);
      if(szReadBuffer != nullptr)
      {
        char *szLine = szReadBuffer;
        // initialize memory with 0 (end-of-string) and read whole file-content into buffer
        memset(szReadBuffer, 0, fileSize + 1);
        (void)fread(szReadBuffer, fileSize, 1, fFile);

        //find line beginning with "FIRMWARE="
        for (;;)
        {
          char *szNextLine;
          for (szNextLine = szLine; (*szNextLine != '\0'); szNextLine++)
          {
            if (*szNextLine == '\n')
            {
              *szNextLine = '\0'; //insert end-of-string here to terminate szLine
              szNextLine++;
              break;
            }
          }
          if (*szLine == 'F') {
            szLine++;
            if (*szLine == 'I') {
              szLine++;
              if (*szLine == 'R') {
                szLine++;
                if (*szLine == 'M') {
                  szLine++;
                  if (*szLine == 'W') {
                    szLine++;
                    if (*szLine == 'A') {
                      szLine++;
                      if (*szLine == 'R') {
                        szLine++;
                        if (*szLine == 'E') {
                          szLine++;
                          if (*szLine == '=') {
                            szLine++;
                            revision = szLine;
                            break; /* for (;;) */
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }

          if (*szNextLine != '\0')
          {
            szLine = szNextLine;
          }
          else
          {
            break; /* for (;;) */
          }
        }
		free(szReadBuffer);
      }
    }
    fclose(fFile);
  }
  return revision;
}


void MdmCuseWorker::write_result(std::string &result)
{
  if (result.length() > 0)
  {
    _write_result_mutex.lock();
    int fd = ::open(_result_pipe_name.c_str(), O_WRONLY|O_NONBLOCK);
    if (fd < 0)
    {
      mdmd_Log(MD_LOG_WRN, "%s: Open %s failed: %s",
             __func__, _result_pipe_name.c_str(), strerror(errno));
    }
    else
    {
      ssize_t res;
      while (((res = ::write(fd, result.c_str(), result.length())) < 0) && (errno == EINTR));
      if (res == -1)
      {
        mdmd_Log(MD_LOG_WRN, "%s: Write to %s failed: %s",
               __func__, _result_pipe_name.c_str(), strerror(errno));
      }
      ::close(fd);
    }
    _write_result_mutex.unlock();
  }
}


void MdmCuseWorker::process_requests()
{
  std::string result;
  for (;;)
  {
    result.clear();
    if (_expected_sms_pdu_length > 0)
    { //we are waiting for SMS PDU
      mdmd_Log(MD_LOG_DBG2, "Wait for SMS PDU (expected %i body characters)\n", _expected_sms_pdu_length);
      std::string pdu;
      int const pduResult = try_to_get_pdu(_request_read_buffer, pdu);
      if(pduResult < 0)
      {
        if(pduResult == -1)
        { //read_buffer is empty or PDU is not yet complete
          break;
        }
        else
        { //invalid PDU
          result = "\r\nERROR\r\n";
        }
      }
      else
      {
        if (pdu.length() == 0)
        { //abort or nothing to send
          result = "\r\nOK\r\n";
        }
        else
        {
          dbus_send_sms(pdu, _expected_sms_pdu_length, result);
        }
      }
      write_result(result);
      _expected_sms_pdu_length = 0;
      process_reports();  //reports may be enqueued in meantime
    }
    else
    { //we are not waiting for SMS PDU, parse line for commands
      size_t pos = 0;
      std::string command;
      if (0 != get_line_from_buffer(_request_read_buffer, command))
      { //read_buffer is empty or command line is not yet complete
        break;
      }
      mdmd_Log(MD_LOG_DBG2, "Got command line: \"%s\"\n", command.c_str());
      if (command.length() > pos)
      {
        switch (command.at(pos))
        {
          case 'a':
          case 'A':
            pos++;
            if (command.length() > pos)
            {
              switch (command.at(pos))
              {
                case 't':
                case 'T':
                  pos++;
                  if (command.length() > pos)
                  {
                    switch (command.at(pos))
                    {
                      case 'e':
                      case 'E':
                        pos++;
                        if (command.length() > pos)
                        {
                          if (command.at(pos) == '0')
                          { //ate0
                            result = "\r\nOK\r\n";
                          }
                        }
                        break;
                      case 'i':
                      case 'I':
                        //ati
                        result = "\r\nWAGO\r\n";
                        result += _ati_order;
                        result += "\r\nRevision: ";
                        result += _ati_revision;
                        result += "\r\n\r\nOK\r\n";
                        break;
                      case '+':
                        pos++;
                        if (command.length() > pos)
                        {
                          switch (command.at(pos))
                          {
                            case 'c':
                              //at+c
                            case 'C':
                              pos++;
                              if (command.length() > pos)
                              {
                                switch (command.at(pos))
                                {
                                  case 'n':
                                    //at+cn
                                  case 'N':
                                    pos++;
                                    if (command.length() > pos)
                                    {
                                      switch (command.at(pos))
                                      {
                                        case 'm':
                                          //at+cnm
                                        case 'M':
                                          pos++;
                                          if (command.length() > pos)
                                          {
                                            switch (command.at(pos))
                                            {
                                              case 'i':
                                                //at+cnmi
                                              case 'I':
                                                pos++;
                                                if (command.length() > pos)
                                                {
                                                  switch (command.at(pos))
                                                  {
                                                    case '?':
                                                      //at+cnmi?
                                                      if (command.length() == (pos+1))
                                                        dbus_get_sms_report_config(result);
                                                      break;
                                                    case '=':
                                                      //at+cnmi=
                                                      dbus_set_sms_report_config(command, result);
                                                      break;
                                                    default:
                                                      break;
                                                  }
                                                }
                                                break;
                                              default:
                                                break;
                                            }
                                          }
                                          break;
                                        default:
                                          break;
                                      }
                                    }
                                    break;
                                  case 'm':
                                    //at+cm
                                  case 'M':
                                    pos++;
                                    if (command.length() > pos)
                                    {
                                      switch (command.at(pos))
                                      {
                                        case 'g':
                                          //at+cmg
                                        case 'G':
                                          pos++;
                                          if (command.length() > pos)
                                          {
                                            switch (command.at(pos))
                                            {
                                              case 'd':
                                                //at+cmgd
                                              case 'D':
                                                pos++;
                                                if (command.length() > pos)
                                                {
                                                  if (command.at(pos) == '=')
                                                  { //at+cmgd=
                                                    dbus_delete_sms(command, result);
                                                  }
                                                }
                                                break;
                                              case 'f':
                                                //at+cmgf
                                              case 'F':
                                                pos++;
                                                if (command.length() > pos)
                                                {
                                                  switch (command.at(pos))
                                                  {
                                                    case '?':
                                                      //at+cmgf?
                                                      if (command.length() == (pos+1))
                                                        dbus_get_sms_format(result);
                                                      break;
                                                    case '=':
                                                      //at+cmgf=
                                                      dbus_set_sms_format(command, result);
                                                      break;
                                                    default:
                                                      break;
                                                  }
                                                }
                                                break;
                                              case 'l':
                                                //at+cmgl
                                              case 'L':
                                                pos++;
                                                if (command.length() > pos)
                                                {
                                                  if (command.at(pos) == '=')
                                                  { //at+cmgl=
                                                    dbus_list_sms(command, result);
                                                  }
                                                }
                                                break;
                                              case 'r':
                                                //at+cmgr
                                              case 'R':
                                                pos++;
                                                if (command.length() > pos)
                                                {
                                                  if (command.at(pos) == '=')
                                                  { //at+cmgr=
                                                    dbus_read_sms(command, result);
                                                  }
                                                }
                                                break;
                                              case 's':
                                                //at+cmgs
                                              case 'S':
                                                pos++;
                                                if (command.length() > pos)
                                                {
                                                  if (command.at(pos) == '=')
                                                  { //at+cmgs=
                                                    dbus_send_sms_prepare(command, _expected_sms_pdu_length, result);
                                                  }
                                                }
                                                break;
                                              default:
                                                break;
                                            }
                                          }
                                          break;
                                        default:
                                          break;
                                      }
                                    }
                                    break;
                                  case 'p':
                                    //at+cp
                                  case 'P':
                                    pos++;
                                    if (command.length() > pos)
                                    {
                                      switch (command.at(pos))
                                      {
                                        case 'i':
                                          //at+cpi
                                        case 'I':
                                          pos++;
                                          if (command.length() > pos)
                                          {
                                            switch (command.at(pos))
                                            {
                                              case 'n':
                                                //at+cpin
                                              case 'N':
                                                pos++;
                                                if (command.length() > pos)
                                                {
                                                  switch (command.at(pos))
                                                  {
                                                    case '?':
                                                      //at+cpin?
                                                      if (command.length() == (pos+1))
                                                        dbus_get_sim_status(result);
                                                      break;
                                                    case '=':
                                                      //at+cpin=
                                                      //PIN settings only via config-tool interface (WBM)
                                                      break;
                                                    default:
                                                      break;
                                                  }
                                                }
                                                break;
                                              default:
                                                break;
                                            }
                                          }
                                          break;
                                        case 'm':
                                          //at+cpm
                                        case 'M':
                                          pos++;
                                          if (command.length() > pos)
                                          {
                                            switch (command.at(pos))
                                            {
                                              case 's':
                                                //at+cpms
                                              case 'S':
                                                pos++;
                                                if (command.length() > pos)
                                                {
                                                  switch (command.at(pos))
                                                  {
                                                    case '?':
                                                      //at+cpms?
                                                      if (command.length() == (pos+1))
                                                        dbus_get_sms_storage(result);
                                                      break;
                                                    case '=':
                                                      //at+cpms=
                                                      dbus_set_sms_storage(command, result);
                                                      break;
                                                    default:
                                                      break;
                                                  }
                                                }
                                                break;
                                              default:
                                                break;
                                            }
                                          }
                                          break;
                                        default:
                                          break;
                                      }
                                    }
                                    break;
                                  case 'r':
                                    //at+cr
                                  case 'R':
                                    pos++;
                                    if (command.length() > pos)
                                    {
                                      switch (command.at(pos))
                                      {
                                        case 'e':
                                          //at+cre
                                        case 'E':
                                          pos++;
                                          if (command.length() > pos)
                                          {
                                            switch (command.at(pos))
                                            {
                                              case 'g':
                                                //at+creg
                                              case 'G':
                                                pos++;
                                                if (command.length() > pos)
                                                {
                                                  if (command.at(pos) == '?')
                                                  { //at+creg?
                                                    if (command.length() == (pos+1))
                                                      dbus_get_reg_state(result);
                                                  }
                                                }
                                                break;
                                              default:
                                                break;
                                            }
                                          }
                                          break;
                                        default:
                                          break;
                                      }
                                    }
                                    break;
                                  case 's':
                                    //at+cs
                                  case 'S':
                                    pos++;
                                    if (command.length() > pos)
                                    {
                                      switch (command.at(pos))
                                      {
                                        case 'q':
                                          //at+csq
                                        case 'Q':
                                          if (command.length() == (pos+1))
                                            dbus_get_signal_quality(result);
                                          break;
                                        default:
                                          break;
                                      }
                                    }
                                    break;
                                  default:
                                    break;
                                }
                              }
                              break;
                            default:
                              break;
                          }
                        }
                        break;
                      default:
                        break;
                    }
                  }
                  break;
                default:
                  break;
              }
            }
            break;
          default:
            break;
        }
        if (result.length() == 0)
        {
          result = "\r\nERROR\r\n";
        }
        write_result(result);
      }
    }
  }
}

void MdmCuseWorker::process_reports()
{
  if (_expected_sms_pdu_length == 0)
  { //we are not waiting for SMS PDU, forward any report to result pipe
    for (;;)
    {
      std::string line;
      if (0 != get_line_from_buffer(_report_read_buffer, line))
      { //read_buffer is empty or report is not yet complete
        break;
      }
      if (line.length() > 0)
      {
        std::string result;
        result.reserve(line.length() + 4);
        result += "\r\n";
        result += line;
        result += "\r\n";
        write_result(result);
      }
    }
  }
}

void MdmCuseWorker::read_request(int fd)
{
  ssize_t len;
  while (((len = read(fd, _readbuf, sizeof(_readbuf))) < 0) && (errno == EINTR));
  if (len > 0)
  {
    _request_read_buffer.append(_readbuf, (size_t)len);
    process_requests();
  }
}

void MdmCuseWorker::read_report(int fd)
{
  ssize_t len = read(fd, _readbuf, sizeof(_readbuf));
  if (len > 0)
  {
    _report_read_buffer.append(_readbuf, (size_t)len);
    process_reports();
  }
}


gpointer mdm_cuse_worker_main(gpointer data)
{
  (void)data; //unused parameter
  const char *pipe_request = MDM_CUSE_FIFO_REQUEST;
  const char *pipe_report = MDM_CUSE_FIFO_EVENT_REPORT;

  //create pipes if not yet exist
  if( access( pipe_request, F_OK ) == -1 )
  {
    int err = mkfifo(pipe_request, S_IWUSR | S_IRUSR );
    if (err != 0)
    {
      mdmd_Log(MD_LOG_ERR, "%s: Create pipe %s failed: %s",
               __func__, pipe_request, strerror(err));
      pipe_request = nullptr;
    }
  }
  if( access( pipe_report, F_OK ) == -1 )
  {
    int err = mkfifo(pipe_report, S_IWUSR | S_IRUSR );
    if (err != 0)
    {
      mdmd_Log(MD_LOG_ERR, "%s: Create pipe %s failed: %s",
               __func__, pipe_report, strerror(err));
      pipe_report = nullptr;
    }
  }
  if ((pipe_request != nullptr) && (pipe_report != nullptr))
  {
    int fd_fifo_request;
    int fd_fifo_report;
    MdmCuseWorker worker;

    fd_fifo_request = open(MDM_CUSE_FIFO_REQUEST, O_RDWR|O_NONBLOCK);
    if (fd_fifo_request < 0)
    {
      mdmd_Log(MD_LOG_ERR, "%s: Open %s failed: %s",
             __func__, MDM_CUSE_FIFO_REQUEST, strerror(errno));
    }
    else
    {
      fd_fifo_report = open(MDM_CUSE_FIFO_EVENT_REPORT, O_RDWR|O_NONBLOCK);
      if (fd_fifo_report < 0)
      {
        mdmd_Log(MD_LOG_ERR, "%s: Open %s failed: %s",
               __func__, MDM_CUSE_FIFO_EVENT_REPORT, strerror(errno));
      }
      else
      {
        mdmd_Log(MD_LOG_INF, "%s: Select loop running...",
                 __func__);
        for (;;)
        {
          fd_set readfds;
          int maxfd = (fd_fifo_request > fd_fifo_report) ? fd_fifo_request : fd_fifo_report;
          FD_ZERO(&readfds);
          FD_SET(fd_fifo_request, &readfds);
          FD_SET(fd_fifo_report, &readfds);
          (void)select(maxfd + 1, &readfds, nullptr, nullptr, nullptr);
          if (FD_ISSET(fd_fifo_request, &readfds))
          {
            worker.read_request(fd_fifo_request);
          }
          if (FD_ISSET(fd_fifo_report, &readfds))
          {
            worker.read_report(fd_fifo_report);
          }
        }
        mdmd_Log(MD_LOG_INF, "%s: Select loop end.",
                 __func__);
        close(fd_fifo_report);
      }
      close(fd_fifo_request);
    }
  }
  return nullptr;
}
