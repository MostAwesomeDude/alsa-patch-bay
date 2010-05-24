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

#include "alsa-addr.h"
#include "alsa-driver.h"

namespace APB {
namespace Alsa {

Addr:: Addr (const snd_seq_addr_t * addr, Driver * driver)
: _addr   (*addr),
  _driver (driver)
{
}

Addr:: Addr (const Addr& addr)
: _addr   (addr._addr),
  _driver (addr._driver)
{
}

Addr:: ~Addr ()
{
  return;
}

bool
Addr:: equals (const APB::Addr * addr) const
{
  const Addr * a = (const Addr *) addr;

  return a->addr().client == _addr.client &&
         a->addr().port   == _addr.port;
}

bool
Addr:: clientEquals (const APB::Addr * addr) const
{
  const Addr * a = (const Addr *) addr;

  return a->addr().client == _addr.client;
}

std::string
Addr:: getName () const
{
  const static unsigned short break_size = 50;
  
  std::string s(_driver->findClientName (this) + "::" + _driver->findPortName (this));
  
  if (s.size() < break_size)
    {
        return s;
    }
  else
    {
      return _driver->findClientName (this) + "::\n" + _driver->findPortName (this);
    }
}


} /* namespace Alsa */
} /* namespace APB */


/* EOF */

