// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#define FUSE_USE_VERSION 31

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include <linux/termios.h>
#include <fuse.h>
#include <fuse_opt.h>
#include <cuse_lowlevel.h>

#include "mdm_cuse.h"
#include "mdm_cuse_worker.h"

namespace {

static const int cm_syslog_level = LOG_DEBUG;

typedef struct mdm_cuse_ctx {
  struct cuse_lowlevel_ops cuse_ops;
  struct fuse_session *session;
  int                  fd_fifo_result;
  struct termios       termios_settings;
} mdm_cuse_ctx_t;

void mdm_cuse_init(void *userdata, struct fuse_conn_info *conn)
{
  (void)conn; //unused parameter
  syslog(LOG_DEBUG, "%s", __func__);
  const char* pipe_name = MDM_CUSE_FIFO_RESULT;
  mdm_cuse_ctx_t *ctx = (mdm_cuse_ctx_t *)userdata;

  //create pipe if not yet exist
  if( access( pipe_name, F_OK ) == -1 )
  {
    int err = mkfifo(pipe_name, S_IWUSR | S_IRUSR );
    if (err != 0)
    {
      syslog(LOG_ERR, "%s: Create pipe %s failed: %s",
             __func__, pipe_name, strerror(err));
      pipe_name = nullptr;
    }
  }
  if ((ctx != nullptr) && (pipe_name != nullptr))
  {
    ctx->fd_fifo_result = open(pipe_name, O_RDONLY|O_NONBLOCK);
    if (ctx->fd_fifo_result < 0)
    {
      syslog(LOG_ERR, "%s: Open %s failed: %s",
             __func__, pipe_name, strerror(errno));
    }
  }
}

void mdm_cuse_destroy(void *userdata)
{
  syslog(LOG_DEBUG, "%s", __func__);
  if (userdata)
  {
    mdm_cuse_ctx_t *ctx = (mdm_cuse_ctx_t *)userdata;
    if (ctx->fd_fifo_result >= 0) close(ctx->fd_fifo_result);
  }
}

void mdm_cuse_open(fuse_req_t req, struct fuse_file_info *fi)
{
  syslog(LOG_DEBUG, "%s", __func__);
  fuse_reply_open(req, fi);
}

void mdm_cuse_read(fuse_req_t req, size_t size, off_t off,
                    struct fuse_file_info *fi)
{
  char *buf = (char*)malloc(size);
  int reply_result;
  (void)off; (void)fi; //unused parameter
  if (buf == nullptr)
  {
    reply_result = fuse_reply_buf(req, nullptr, 0);
  }
  else
  {
    ssize_t buf_read = 0;
    mdm_cuse_ctx_t *ctx = (mdm_cuse_ctx_t *)fuse_req_userdata(req);
    if (ctx && (ctx->fd_fifo_result >= 0))
    {
      while (((buf_read = read(ctx->fd_fifo_result, buf, size)) < 0) && (errno == EINTR));
      //if (buf_read > 0) syslog(LOG_DEBUG, "%s: read %d bytes", __func__, buf_read);
      //
      if (buf_read < 0)
      { 
          buf_read = 0;
          //FIFO is nonblocking, when no data available then read returns with EAGAIN.
          if (errno != EAGAIN)
          {
              syslog(LOG_ERR, "%s: Unable to read buffer:%s", __func__, strerror(errno));
          }
      }
    }
    reply_result = fuse_reply_buf(req, buf, (size_t)buf_read);
    free(buf);
  }
  if (reply_result != 0)
  {
      syslog(LOG_ERR, "%s: Fuse reply failed: %s", __func__, strerror(errno));
  }
}

void mdm_cuse_write(fuse_req_t req, const char *buf, size_t size,
                     off_t off, struct fuse_file_info *fi)
{
  void *userdata;
  //syslog(LOG_DEBUG, "%s: write %d bytes", __func__, size);
  (void)off; (void)fi; //unused parameter
  userdata = fuse_req_userdata(req);
  if ((userdata != nullptr) && (buf != nullptr) && (size > 0))
  {
    const char * pipe_name = MDM_CUSE_FIFO_REQUEST;
    int fd = open(pipe_name, O_WRONLY|O_NONBLOCK);
    if (fd < 0)
    {
      syslog(LOG_WARNING, "%s: open %s failed: %s",
             __func__, pipe_name, strerror(errno));
      size = -1;
    }
    else
    {
      ssize_t res = write(fd, buf, size);
      if (res == -1)
      {
        syslog(LOG_WARNING, "%s: write to %s failed: %s",
               __func__, pipe_name, strerror(errno));
      }
      close(fd);
    }
  }
  fuse_reply_write(req, size);
}

void mdm_cuse_ioctl(fuse_req_t req, int cmd, void *arg,
                     struct fuse_file_info *fi, unsigned flags,
                     const void *in_buf, size_t in_bufsz, size_t out_bufsz)
{
  void *userdata;
  (void)fi;
  userdata = fuse_req_userdata(req);
  if (userdata)
  {
    mdm_cuse_ctx_t *ctx = (mdm_cuse_ctx_t *)userdata;

    //syslog(LOG_DEBUG, "%s: cmd %d, arg %x, in_buf %x, in_bufsz %d, out_bufsz %d, size %d",
    //       __func__, cmd, arg, in_buf, in_bufsz, out_bufsz, sizeof(ctx->termios_settings));

    if (flags & FUSE_IOCTL_COMPAT) {
      fuse_reply_err(req, ENOSYS);
      return;
    }

    switch (cmd) {
    case TCGETS:
      if (!out_bufsz) {
        struct iovec iov = { arg, sizeof(ctx->termios_settings) };
        fuse_reply_ioctl_retry(req, nullptr, 0, &iov, 1);
      } else {
        fuse_reply_ioctl(req, 0, &ctx->termios_settings, sizeof(ctx->termios_settings));
      }
      break;
    case TCSETS:
    case TCSETSW:
    case TCSETSF:
      if (!in_bufsz) {
        struct iovec iov = { arg, sizeof(ctx->termios_settings) };
        fuse_reply_ioctl_retry(req, &iov, 1, nullptr, 0);
      } else {
        memcpy(&ctx->termios_settings, in_buf, sizeof(ctx->termios_settings));
        fuse_reply_ioctl(req, 0, nullptr, 0);
      }
      break;

    default:
      fuse_reply_ioctl(req, 0, nullptr, 0);
    }
  }
  else
  {
    fuse_reply_ioctl(req, 0, nullptr, 0);
  }
}


int mdm_cuse_setup(int argc, char **argv, mdm_cuse_ctx_t *ctx)
{
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    struct cuse_info ci;
    int multithreaded;
    size_t devname_len = strlen(MDM_CUSE_DEV_NAME);
    char *dev_info_name = (char*)malloc(sizeof(char)*(devname_len + 8 + 1));
    const char *dev_info_argv[] = { dev_info_name };

    ctx->fd_fifo_result = -1;
    if (dev_info_name == nullptr)
    {
      syslog(LOG_ERR, "%s: Out of memory", __func__);
      return -1;
    }

    memset(&ctx->termios_settings, 0, sizeof(ctx->termios_settings));

    snprintf(dev_info_name, (devname_len + 8 +1), "DEVNAME=%s", MDM_CUSE_DEV_NAME);
    memset(&ci, 0, sizeof(ci));
    ci.dev_info_argc = 1;
    ci.dev_info_argv = dev_info_argv;
    ci.flags = CUSE_UNRESTRICTED_IOCTL;

    memset(&ctx->cuse_ops, 0, sizeof(ctx->cuse_ops));
    ctx->cuse_ops.init      = mdm_cuse_init;
    ctx->cuse_ops.destroy   = mdm_cuse_destroy;
    ctx->cuse_ops.open      = mdm_cuse_open;
    ctx->cuse_ops.read      = mdm_cuse_read;
    ctx->cuse_ops.write     = mdm_cuse_write;
    ctx->cuse_ops.ioctl     = mdm_cuse_ioctl;

    ctx->session = cuse_lowlevel_setup(args.argc, args.argv, &ci, &ctx->cuse_ops,
                                   &multithreaded, ctx);

    free(dev_info_name);
    if (ctx->session == nullptr)
    {
      syslog(LOG_ERR, "%s: Start session failed.", __func__);
      return -1;
    }
    return 0;
}

} //namespace

int main(int argc, char *argv[])
{
  int status = EXIT_SUCCESS;
  mdm_cuse_ctx_t ctx;

  setlogmask (LOG_UPTO(cm_syslog_level));
  openlog ("mdm_cuse", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER);
  syslog(LOG_NOTICE, "%s: Started by user %u", __func__, getuid ());

  if (0 != mdm_cuse_setup(argc, argv, &ctx))
  {
    status = EXIT_FAILURE;
  }
  else
  {
    if (0 != fuse_set_signal_handlers(ctx.session))
    {
      status = EXIT_FAILURE;
      syslog(LOG_ERR, "%s: Set signal handler failed", __func__);
    }
    else
    {
      syslog(LOG_INFO, "%s: Session loop running...", __func__);
      int err = fuse_session_loop(ctx.session);
      if (0 != err)
      {
        status = EXIT_FAILURE;
        syslog(LOG_ERR, "%s: Error in session loop: %d", __func__, err);
      }
      fuse_remove_signal_handlers(ctx.session);
    }
    fuse_session_destroy(ctx.session);
  }

  syslog(LOG_NOTICE, "Program exit");
  closelog();
  exit(status);
}
