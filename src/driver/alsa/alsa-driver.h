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

#ifndef __APB_ALSA_DRIVER_H__
#define __APB_ALSA_DRIVER_H__

#include <list>
#include <string>
#include <iostream>
#include <alsa/asoundlib.h>

#include <pthread.h>

#include "apb.h"
#include "driver.h"
#include "subscription.h"
#include "addr.h"
#include "alsa-addr.h"

namespace APB {
namespace Alsa {

class Driver : public APB::Driver {
  private:
    snd_seq_t * _seq;
    std::list<APB::Addr *> _readPorts;
    std::list<APB::Addr *> _writePorts;
    std::list<APB::Subscription *> _subscriptions;
    
    std::string _title;
    
    void refreshPorts (std::list<APB::Addr *>&, unsigned int);
    void doPortSubscription (snd_seq_port_subscribe_t *,
                             const APB::Addr *,
                             const APB::Addr *);
    void doPortUnsubscription (snd_seq_port_subscribe_t *,
                             const APB::Addr *,
                             const APB::Addr *);

    int       createListenPort ();
    void      sendRefresh ();
    void      getEvent ();
    pthread_t _listenThread;
  public:
    Driver (const std::string&, int *, char ***);
    virtual ~Driver () {
    }
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
    static  void *                                      refreshMain (void *);
            void                                        refreshIMain (void);
};

} /* namespace Alsa */
} /* namespace APB */

#endif /* __APB_ALSA_DRIVER_H__ */
