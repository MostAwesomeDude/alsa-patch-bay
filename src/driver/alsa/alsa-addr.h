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

#ifndef __APB_ALSA_ADDR_H__
#define __APB_ALSA_ADDR_H__

#include <list>
#include <string>
#include <iostream>
#include <alsa/asoundlib.h>

#include "apb.h"
#include "driver.h"
#include "subscription.h"
#include "addr.h"

namespace APB {
namespace Alsa {

class Driver;

class Addr : public APB::Addr
{
  private:
    snd_seq_addr_t _addr;
    Driver * _driver;
  public:
                                  Addr         (const snd_seq_addr_t * addr, Driver * driver);
                                  Addr         (const Addr& addr);
            const snd_seq_addr_t& addr         () const { return _addr; }
            int                   client       () const { return _addr.client; }
            int                   port         () const { return _addr.port; }
    virtual                       ~Addr        ();
    virtual bool                  equals       (const APB::Addr * addr) const;
    virtual bool                  clientEquals (const APB::Addr * addr) const;
    virtual APB::Driver *         driver       () const { return (APB::Driver *) _driver; }
    virtual std::string           getName      () const;
};

} /* namespace Alsa */
} /* namespace APB */

#endif /* __APB_ALSA_ADDR_H__ */
