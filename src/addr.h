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

#ifndef __APB_ADDRESS_H__
#define __APB_ADDRESS_H__

#include <string>

namespace APB {

class Driver;

class Addr {
  public:
    virtual ~Addr () = 0;
    virtual bool equals (const Addr *) const = 0;
    virtual bool clientEquals (const Addr *) const = 0;
    virtual Driver * driver () const = 0;
    virtual std::string getName () const = 0;
};

} /* namespace APB */

#endif /* __APB_ADDRESS_H__ */
