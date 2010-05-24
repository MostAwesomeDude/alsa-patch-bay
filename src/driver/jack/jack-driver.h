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

#ifndef __APB_JACK_DRIVER_H__
#define __APB_JACK_DRIVER_H__

#include <list>
#include <string>
#include <iostream>
#include <jack/jack.h>

#include <pthread.h>

#include "apb.h"
#include "driver.h"
#include "subscription.h"
#include "addr.h"

namespace APB {
namespace Jack {

class Addr;

class Driver : public APB::Driver {
  private:
    jack_client_t * _jackClient;
    std::list<APB::Addr *> _readPorts;
    std::list<APB::Addr *> _writePorts;
    std::list<APB::Subscription *> _subscriptions;
    
    std::string _title;
    
    void refreshPortList (std::list<APB::Addr *>& portList,  enum JackPortFlags flags);
    Addr * findWritePort (const char * portName);
    
  public:
                                                        Driver  (const std::string&, int *, char ***);
    virtual                                             ~Driver ();
    virtual std::string                                 findClientName (const APB::Addr *) const;
    virtual std::string                                 findPortName (const APB::Addr *) const;
    virtual const std::list<APB::Addr *>&               getReadPorts ();
    virtual const std::list<APB::Addr *>&               getWritePorts ();
    virtual const std::list<const APB::Subscription *>& getSubscriptions ();
    virtual void                                        refreshPorts ();
    virtual void                                        refreshSubscriptions ();
    virtual void                                        subscribePorts (const APB::Addr *, const APB::Addr *);
    virtual void                                        subscribeClients (const APB::Addr *, const APB::Addr *);
    virtual void                                        removeSubscription (const Subscription *);
};

} /* namespace Jack */
} /* namespace APB */

#endif /* __APB_JACK_DRIVER_H__ */
