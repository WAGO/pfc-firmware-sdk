#include <cstdio>
#include <csignal>
#include <sys/types.h>
#include <sys/wait.h>

#include <gio/gio.h>

#include <cstring>

#include <getopt.h>
#include "NetworkConfigurator.hpp"
#include "Status.hpp"
#include <gsl/gsl>
#include "Daemonizer.hpp"
#include "InterprocessCondition.h"
#include "Logger.hpp"

#define XSTR(s) #s
#define STR(s) XSTR(s)

#ifndef NETCONFD_VERSION
#define NETCONFD_VERSION development
#endif

using ::std::string;

static struct option long_options[] = { { "help", no_argument, nullptr, 'h' }, {
    "version", no_argument, nullptr, 'v' }, { "rundir", required_argument,
    nullptr, 'r' }, { "pidfile", required_argument, nullptr, 'p' }, { "daemon",
no_argument, nullptr, 'd' }, { "loglevel", required_argument, nullptr, 'l' }, {
    nullptr, 0, nullptr, 0 } /* end marker */
};

static char const * const usage_text =
    "%s: WAGO network configuration service\n"
        "Usage:   -h, --help:             Print this help message.\n"
        "         -v, --version           Print program version\n"
        "Options:\n"
        "         -d, --daemon            Daemonize this program\n"
        "         -r, --rundir=<dir>      Run directory, default = /var/run/netconfd\n"
        "         -p, --pidfile=<name>    Name of pidfile, default = netconfd.pid\n"
        "         -l, --loglevel=<level>  Level of logging, default = debug\n"
        "                                 loglevel: error, warning, info, debug\n";

static char const * const version_text =
    "%s version " STR(NETCONFD_VERSION) "\n";

int main(int argc, char *argv[]) {

  ::std::string run_dir = "/var/run/netconfd";
  ::std::string pid_file_name = "netconfd.pid";
  ::std::string loglevel = "debug";

  printf("Starting network configuration daemon... \n");

  int option_index = 0;
  auto daemonize = false;
  auto args = gsl::make_span(argv, static_cast<size_t>(argc));

  int c;
  while ((c = getopt_long(argc, args.data(), "hvrpdl:",
                          static_cast<option*>(long_options), &option_index))
      != -1) {
    switch (c) {
      case 'h':
        printf(usage_text, args[0]);
        exit(0);
        /*NOTREACHED*/
      case 'v':
        printf(version_text, args[0]);
        exit(0);
        /*NOTREACHED*/
      case 'r':
        run_dir = ::std::string(optarg);
        break;
      case 'p':
        pid_file_name = ::std::string(optarg);
        break;
      case 'd':
        daemonize = true;
        break;
      case 'l':
        loglevel = ::std::string(optarg);
        break;
      default:
        fprintf(stderr, "Illegal command line option %c", c);
        exit(EXIT_FAILURE);
        /*NOTREACHED*/
    }
  }

  netconf::SetLogSink(netconf::LogSink::SYSLOG);
  netconf::SetLogLevel(netconf::LogLevelFromString(loglevel));

  netconf::Status status;
  netconf::Daemonizer daemon(run_dir, pid_file_name);

  if (status.Ok()) {
    status = daemon.PreparePidDir();
  }

  if (status.Ok()) {
    bool pidFileIsLocked = daemon.IsPidFileLocked();
    if (pidFileIsLocked) {
      fprintf( stderr, "Pid file is locked. "
              "Probably, an instance of netconfd is already running.\n");

      exit(EXIT_FAILURE);
    }
  }

  netconf::InterprocessCondition start_condition{"netconfd_interprocess"};

  if (daemonize) {
    status = daemon.Daemonize(start_condition);

    if (status.NotOk()) {
      netconf::LogError(status.GetMessage());
      abort();
    }
  }

  if (status.Ok()) {
    status = daemon.WritePidFile();
  }

  if (status.Ok()) {
    daemon.SetUnlinkPidOnExit();
  }

  if (status.NotOk()) {
    netconf::LogWarning(status.GetMessage());
  }

  auto context = g_main_context_default();
  auto loop = g_main_loop_new(context, 0);

  try {
    netconf::NetworkConfigurator network_configurator{start_condition};
    g_main_loop_run(loop);
  } catch (::std::exception& ex) {
    ::std::string exception_message(ex.what());
    netconf::LogError("NetConf initialization exception: " + exception_message);
    exit(EXIT_FAILURE);
  } catch (...) {
    netconf::LogError("NetConf unknown initialization exception");
    exit(EXIT_FAILURE);
  }


  exit(EXIT_SUCCESS);
}
