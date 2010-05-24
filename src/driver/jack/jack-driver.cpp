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

#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include "jack-driver.h"
#include "jack-addr.h"
#include "misc.h"
#include "plugin.h"
#include "ui.h"

namespace APB {
namespace Jack {

static int
jackGraphOrderCallback (void * data)
{
  char refresh = 1;
  ssize_t err;
  int refreshWriteFile = *(int *)data;
  
  err = write (refreshWriteFile, &refresh, sizeof (refresh));
  if (err == -1)
    {
      std::cerr << "error writing to refresh pipe: "
                << strerror (errno) 
                << std::endl;
      return 1;
    }
  
  return 0;
}

Driver:: Driver (const std::string& title, int * argc, char *** argv)
{
  char * client_name;
  char * ptr;

  client_name = strdup (title.c_str ());
  ptr = client_name;
  while ( (ptr = strchr (ptr, ' ')) )
    *ptr = '_';
  
  _jackClient = jack_client_new (client_name);
  if (!_jackClient)
    {
      std::cerr << __FUNCTION__
                << ": could not connect to jackd"
                << std::endl;
      abort ();
    }
  
  free (client_name);
  
  jack_set_graph_order_callback (_jackClient, &jackGraphOrderCallback, &_refreshWriteFile);
  
  jack_activate (_jackClient);
}

Driver:: ~Driver ()
{
  jack_deactivate (_jackClient);
  
  jack_client_close (_jackClient);
}

std::string
Driver:: findClientName (const APB::Addr * addr) const
{
  const Addr * a = (const Addr *) addr;

  return std::string (a->client ());
}

std::string
Driver:: findPortName (const APB::Addr * addr) const
{
  const Addr * a = (const Addr *) addr;

  return std::string (a->port ());
}

const std::list<APB::Addr *>&
Driver:: getReadPorts ()
{
  return _readPorts;
}

const std::list<APB::Addr *>&
Driver:: getWritePorts ()
{
  return _writePorts;
}

const std::list<const APB::Subscription *>&
Driver:: getSubscriptions ()
{
  static std::list<const APB::Subscription *> subs;

  subs.clear ();
  for (std::list<APB::Subscription *>::iterator i = _subscriptions.begin ();
       i != _subscriptions.end();
       ++i)
    {
      if ((*i)->from () == 0)
        std::cerr << DEBUG_STRING << "null from()" << std::endl;
      if ((*i)->to () == 0)
        std::cerr << DEBUG_STRING << "null to()" << std::endl;
      subs.push_back (*i);
    }
  
  return subs;
}

void
Driver:: refreshPorts ()
{
  refreshPortList (_readPorts, JackPortIsOutput);
  refreshPortList (_writePorts, JackPortIsInput);
  refreshSubscriptions ();
}

void
Driver:: refreshPortList (std::list<APB::Addr *>& portList, enum JackPortFlags flags)
{
  for (std::list<APB::Addr *>::iterator ad = portList.begin();
       ad != portList.end ();
       ++ad)
    {
/*      delete *ad; */
    }
  portList.clear ();
  
  const char ** jack_ports = jack_get_ports (_jackClient, NULL, NULL, flags);
  
  if (!jack_ports)
    return;
  
  Addr * addr;
  for (unsigned long i = 0; jack_ports[i]; ++i)
    {
      addr = new Addr (std::string (jack_ports[i]), this);
      
      portList.push_back (addr);
    }
  
  free (jack_ports);
}

Addr *
Driver:: findWritePort (const char * portName)
{
  Addr * addr;
  std::string port_name (portName);  
  for (std::list<APB::Addr *>::const_iterator writeAddrIter = _writePorts.begin ();
       writeAddrIter != _writePorts.end ();
       writeAddrIter++)
    {
      addr = (Addr *) *writeAddrIter;
      
      if (port_name == addr->getName ())
        return addr;
    }
  
  return 0;
}

void
Driver:: refreshSubscriptions ()
{
  for (std::list<APB::Subscription *>::iterator s = _subscriptions.begin ();
       s != _subscriptions.end ();
       ++s)
    {
      delete *s;
    }
  _subscriptions.clear ();

  jack_port_t * port;
  Addr * addr;
  const char ** connections;
  
  for (std::list<APB::Addr *>::const_iterator readAddrIter = _readPorts.begin ();
       readAddrIter != _readPorts.end ();
       ++readAddrIter)
    {
      addr = (Addr *) *readAddrIter;
      port = jack_port_by_name (_jackClient, addr->portName ());
      
      if (!port)
        {
          std::cerr << __FUNCTION__
                    << ": could not find port '" 
                    << addr->portName ()
                    << "'"
                    << std::endl;
          continue;
        }
      
      connections = jack_port_get_all_connections (_jackClient, port);
      
      if (!connections)
        continue;
        
      for (unsigned long i = 0; connections[i]; ++i)
        {
          Addr * waddr = findWritePort (connections[i]);
          
          if (!waddr)
            continue;
          
          Subscription * sub = new Subscription (addr, waddr);
          _subscriptions.push_back (sub);
        }
      
      free (connections);
    }
}

void
Driver:: subscribePorts (const APB::Addr * readAddr, const APB::Addr * writeAddr)
{
  Addr * raddr = (Addr *) readAddr;
  Addr * waddr = (Addr *) writeAddr;
  
  int err = jack_connect (_jackClient, raddr->portName (), waddr->portName ());
  
  if (err)
    {
      throw Exception ("Jack server could not connect ports");
    }

  _ui->log (std::string ("Subscribed ports '") + raddr->portName () + "' and '"
                                               + waddr->portName () + "'");
}

void
Driver:: subscribeClients (const APB::Addr * readAddr, const APB::Addr * writeAddr)
{
  Addr * raddr;
  Addr * waddr = NULL;
  std::string rclient (((const Addr *)readAddr)->client ());
  std::string wclient (((const Addr *)writeAddr)->client ());

  unsigned long rcount = 0;
  for (std::list<APB::Addr *>::iterator riter = _readPorts.begin ();
       riter != _readPorts.end ();
       ++riter)
    {
      raddr = (Addr *) *riter;
      if (raddr->client() == rclient)
        {
          unsigned long wcount = 0;
          std::list<APB::Addr *>::iterator witer;
          for (witer = _writePorts.begin ();
               witer != _writePorts.end ();
               ++witer)
            {
              waddr = (Addr *) *witer;
              if (waddr->client() == wclient)
                {
                  if (wcount == rcount)
                    break;
                  
                  ++wcount;
                }
            }
          
          if (witer == _writePorts.end () || wcount != rcount)
            continue;
           
          subscribePorts (raddr, waddr);
          
          rcount++;
        }
    }
}

void
Driver:: removeSubscription (const Subscription * sub)
{
  Addr * raddr = (Addr *) sub->from ();
  Addr * waddr = (Addr *) sub->to ();
  
  int err = jack_disconnect (_jackClient, raddr->portName (), waddr->portName ());
  
  if (err)
    {
      throw Exception ("Jack server could not disconnect ports");
    }

  _ui->log (std::string ("Removed subscription ") + sub->getName ());
}

} /* namespace Jack */

class JackPlugin : public DriverPlugin
{
  public:
    virtual Driver *   getDriver       (const std::string& title, int * argc, char *** argv);
};


Driver *
JackPlugin:: getDriver (const std::string& title, int * argc, char *** argv)
{
  return new Jack::Driver (title, argc, argv);
}

} /* namespace APB */

APB::DriverPlugin *
getDriverPlugin ()
{
  return new APB::JackPlugin ();
}

/* EOF */

