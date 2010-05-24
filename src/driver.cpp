/*
 * ALSA Patch Bay
 *
 * Copyright (C) 2002 Robert Ham (node@users.sourceforge.net)
 *
 * You have permission to use this file under the GNU General
 * Public License, version 2 or later.  See the file COPYING
 * for the full text.
 *
 */

#include <cstdlib>
#include <iostream>

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#include "driver.h"

namespace APB {

Driver:: Driver ()
  : _ui (0),
    _jfd (true)
{
  int err;
  int files[2]; 

  err = pipe (files);
  if (err)
    {
      std::cerr << "error creating pipe, exiting: " << strerror (errno) << std::endl;
      exit (1);
    }
  
  _refreshReadFile = files[0];
  _refreshWriteFile = files[1];
  
  /* set the read file to not block */
  err = fcntl (_refreshReadFile, F_SETFL, O_NONBLOCK);
  if (err)
    {
      std::cerr << "error setting read end of pipe to non-blocking mode, exiting: " << strerror (errno) << std::endl;
      exit (1);
    }
}

void
Driver:: setJFD (bool jfd)
{
  _jfd = jfd;
}

void
Driver:: setUI (UI * ui)
{
  _ui = ui;
}

int
Driver:: getRefreshReadFile ()
{
  return _refreshReadFile;
}

#ifdef HAVE_LADCCA
void
Driver:: setCCAClient (cca_client_t * client)
{
  _ccaClient = client;
}

cca_client_t *
Driver:: getCCAClient ()
{
  return _ccaClient;
}
#endif /* HAVE_LADCCA */



} /* namespace APB */
