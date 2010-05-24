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

#ifndef __APB_DRIVER_H__
#define __APB_DRIVER_H__

#include "config.h"

#include <string>
#include <list>

#ifdef HAVE_LADCCA
# include <ladcca/ladcca.h>
#endif /* HAVE_LADCCA */

#include "subscription.h"

namespace APB {

class UI;
class Addr;
class Subscription;

class Driver
{
  protected:
    UI *           _ui;
    bool           _jfd;
    int            _refreshWriteFile;
    int            _refreshReadFile;
#ifdef HAVE_LADCCA
    cca_client_t * _ccaClient;
#endif
    Driver ();
    
    
  public:
            void                     setJFD (bool jfd);
            void                     setUI (UI * ui);
            int                      getRefreshReadFile ();
#ifdef HAVE_LADCCA
            void                     setCCAClient (cca_client_t * client);
            cca_client_t *           getCCAClient ();
#endif
    virtual std::string              findClientName (const Addr *) const = 0;
    virtual std::string              findPortName (const Addr *) const = 0;
    virtual const std::list<Addr *>& getReadPorts () = 0;
    virtual const std::list<Addr *>& getWritePorts () = 0;
    virtual const std::list<const Subscription *>& getSubscriptions () = 0;
    virtual void                     refreshPorts () = 0;
    virtual void                     refreshSubscriptions () = 0;
    virtual void                     subscribePorts (const Addr *, const Addr *) = 0;
    virtual void                     subscribeClients (const Addr *, const Addr *) = 0;
    virtual void                     removeSubscription (const Subscription *) = 0;
};

} /* namespace APB */


#endif /* __APB_DRIVER_H__ */
