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

#ifndef __APB_SUBSCRIPTION_H__
#define __APB_SUBSCRIPTION_H__

#include "config.h"

#include <map>
#include <string>

namespace APB {

class Addr;

class Subscription
{
  private:
    Addr *                     _from;
    Addr *                     _to;
    std::map<std::string, int> _attributes;

  public:
                                      Subscription (Addr * from, Addr * to);
                                      Subscription (const Subscription& subscription);
                                      ~Subscription ();
    const Addr *                      from () const;
    const Addr *                      to () const;
    std::string                       getName () const;
    bool                              operator == (const Subscription& sub) const;
    const std::map<std::string, int>& getAttributes () const;
    int                               getAttribute (const std::string& attr) const;
    void                              setAttribute (const std::string& attr, int num);
#ifdef HAVE_LADCCA
    void                              setNotice (bool notice);
    bool                              getNotice () const;
#endif
};

} /* namespace APB */


#endif /* __APB_SUBSCRIPTION_H__ */
