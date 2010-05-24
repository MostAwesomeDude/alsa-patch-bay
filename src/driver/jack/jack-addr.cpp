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
#include <cstring>
#include <iostream>

#include "jack-addr.h"

namespace APB {
namespace Jack {

Addr:: Addr (const std::string &portName, Driver * driver)
: _portName (portName),
  _driver (driver)
{
}

Addr:: Addr (const Addr& addr)
: _portName (addr._portName),
  _driver (addr._driver)
{
}

Addr:: ~Addr ()
{
}

std::string
Addr:: client () const
{
  char * client;
  char * ptr;

  client = strdup (_portName.c_str());
  
  ptr = strchr (client, ':');
  if (ptr)
    *ptr = '\0';
  
  
  std::string clients (client);
  free (client);
  
  return clients;
}

std::string
Addr:: port () const
{
  char * ptr;

  ptr = strchr ((char*)_portName.c_str(), ':');
  ptr++;
  
  std::string ports (ptr);
  
  return ports;
}

const char *
Addr:: portName () const
{
  return _portName.c_str ();
}

bool
Addr:: equals (const APB::Addr * addr) const
{
  const Addr * a = (const Addr *) addr;
  
  return _portName == a->_portName;
}

bool
Addr:: clientEquals (const APB::Addr * addr) const
{
  const Addr * a = (const Addr *) addr;
  
  return client() == a->client();
}

std::string
Addr:: getName () const
{
  return _portName;
}

} /* namespace Jack */
} /* namespace APB */

/* EOF */

