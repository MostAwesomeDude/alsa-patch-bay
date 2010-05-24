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

#ifndef __APB_JACK_ADDR_H__
#define __APB_JACK_ADDR_H__

#include <string>

#include "apb.h"
#include "driver.h"
#include "addr.h"

namespace APB {
namespace Jack {

class Driver;

class Addr : public APB::Addr
{
  private:
    std::string _portName;
    Driver * _driver;
  public:
                                  Addr         (const std::string& portName, Driver * driver);
                                  Addr         (const Addr& addr);
            std::string           client       () const;
            std::string           port         () const;
            const char *          portName     () const;
    virtual bool                  equals       (const APB::Addr * addr) const;
    virtual bool                  clientEquals (const APB::Addr * addr) const;
    virtual APB::Driver *         driver       () const { return (APB::Driver *) _driver; }
    virtual std::string           getName      () const;
    virtual                       ~Addr        ();
};

} /* namespace Jack */
} /* namespace APB */

#endif /* __APB_JACK_ADDR_H__ */
