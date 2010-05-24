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

#include <iostream>

#include <sys/poll.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "alsa-driver.h"
#include "plugin.h"
#include "ui.h"
#include "misc.cpp"

namespace APB {
namespace Alsa {


Driver:: Driver (const std::string& title, int * argc, char *** argv)
{
  int err;
  err = snd_seq_open(&_seq, "default",
                     SND_SEQ_OPEN_DUPLEX,
                     SND_SEQ_NONBLOCK);
  if (err)
    {
      std::string s (std::string (__FUNCTION__) + ": couldn't open alsa sequencer");
      throw APB::Exception (s);
    }
  
  snd_seq_set_client_name (_seq, title.c_str());

  /* create the refresh thread */
  err = pthread_create (&_listenThread, NULL, &Driver::refreshMain, this);
  if (err)
    {
      /* tell the gui not to listen to refreshes */
      char refresh = 0;
      write (_refreshWriteFile, &refresh, sizeof (refresh));
    
      std::cerr << "couldn't start refresh thread" << std::endl;
    }
}

std::string
Driver:: findClientName (const APB::Addr * addr) const
{
  snd_seq_client_info_t * cinfo;

  snd_seq_client_info_alloca (&cinfo);
  
  int err = snd_seq_get_any_client_info (((Driver *)addr->driver())->_seq, ((const Addr *)addr)->client(), cinfo);
  if (err)
    {
      throw APB::Exception (std::string("could not find client in order to resolve its name:\n ")
                            + snd_strerror (err));
    }
  
  return std::string (snd_seq_client_info_get_name (cinfo));
}

std::string
Driver:: findPortName (const APB::Addr * addr) const
{
  snd_seq_port_info_t * pinfo;
  
  snd_seq_port_info_alloca (&pinfo);
  
  int err = snd_seq_get_any_port_info (((Driver *)addr->driver())->_seq,
            ((const Addr *)addr)->client(), ((const Addr *)addr)->port(), pinfo);
  if (err)
    {
      throw APB::Exception (std::string("could not find port in order to resolve its name:\n ")
                            + snd_strerror (err));
    }
  return std::string (snd_seq_port_info_get_name (pinfo));
}

void Driver::refreshPorts ()
{
  refreshPorts (_readPorts, SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_DUPLEX);
  refreshPorts (_writePorts, SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_DUPLEX);
  refreshSubscriptions ();
}

void
Driver:: refreshPorts (std::list<APB::Addr *>& portList, unsigned int capabilities)
{
  for (std::list<APB::Addr *>::iterator ad = portList.begin();
       ad != portList.end ();
       ++ad)
    {
/*      delete *ad; */
    }
  portList.clear ();

  snd_seq_client_info_t * cinfo;
  snd_seq_client_info_alloca (&cinfo);
  snd_seq_client_info_set_client (cinfo, -1);

  snd_seq_port_info_t * pinfo;
  snd_seq_port_info_alloca (&pinfo);
  
  
  while (snd_seq_query_next_client (_seq, cinfo) >= 0)
    {
      snd_seq_port_info_set_client(pinfo, snd_seq_client_info_get_client(cinfo));
      snd_seq_port_info_set_port (pinfo, -1);

      while (snd_seq_query_next_port (_seq, pinfo) >= 0)
        {
          int caps = snd_seq_port_info_get_capability (pinfo);

          if (caps & SND_SEQ_PORT_CAP_NO_EXPORT)
            continue;
    
          if (caps & capabilities) {
            Alsa::Addr * portAddr  = new Alsa::Addr (snd_seq_port_info_get_addr (pinfo), this);
            portList.push_back (portAddr);
          }
        }
    }
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
Driver::  refreshSubscriptions ()
{
  for (std::list<APB::Subscription *>::iterator s = _subscriptions.begin ();
       s != _subscriptions.end ();
       ++s)
    {
      delete *s;
    }
  _subscriptions.clear ();

  snd_seq_query_subscribe_t * subsinfo;
  snd_seq_query_subscribe_alloca (&subsinfo);

  for (std::list<APB::Addr *>::iterator i = _readPorts.begin ();
       i != _readPorts.end ();
       ++i)
    {
      snd_seq_query_subscribe_set_root(subsinfo, &((Addr *)*i)->addr());
      snd_seq_query_subscribe_set_index (subsinfo, 0);

      while (!snd_seq_query_port_subscribers (_seq, subsinfo))
        {
          const snd_seq_addr_t * alsa_waddr = snd_seq_query_subscribe_get_addr (subsinfo);
          APB::Addr * apb_waddr = 0;
          Addr waddr (alsa_waddr, this);

          for (std::list<APB::Addr *>::iterator it = _writePorts.begin ();
               it != _writePorts.end ();
               ++it)
            {
              if ((*it)->equals (&waddr))
                {
                  apb_waddr = *it;
                  break;
                }
            }
          
          if (apb_waddr)
            {
              Subscription * sub = new Subscription (*i, apb_waddr);
              sub->setAttribute ("exclusive", snd_seq_query_subscribe_get_exclusive (subsinfo));
              sub->setAttribute ("time update", snd_seq_query_subscribe_get_time_update (subsinfo));
              sub->setAttribute ("realtime", snd_seq_query_subscribe_get_time_real (subsinfo));
              _subscriptions.push_back (sub);
            }
      
          snd_seq_query_subscribe_set_index (subsinfo, snd_seq_query_subscribe_get_index (subsinfo) + 1);
        }
    }
}

void
Driver:: doPortSubscription (snd_seq_port_subscribe_t * subs,
                             const APB::Addr * from,
                             const APB::Addr * to)
{
  Subscription * subscription;

  if (snd_seq_get_port_subscription(_seq, subs) == 0)
    {
      // already subscribed
      _ui->log (std::string("Subscription ") + from->getName() + " -> "
                         + to->getName() + " already exists");
      return;
    }
  
  int err = snd_seq_subscribe_port(_seq, subs);
  if (err < 0)
    {
      std::string s (std::string("Subscription ") + from->getName() + " -> "
                     + to->getName() + " failed:\n  " + snd_strerror (err));
      throw Exception (s);
      return;
    }

  subscription = new Subscription (new Addr (*(Alsa::Addr *)from), new Addr (*(Alsa::Addr *)to));
  
  _subscriptions.push_back (subscription);
  _ui->log (std::string("Subscription '") + from->getName() + "' -> '"
                        + to->getName() + "' succeeded");

}

void Driver::subscribePorts (const APB::Addr * from,
                             const APB::Addr * to)
{
  snd_seq_port_subscribe_t * subs;
  
  snd_seq_port_subscribe_malloc(&subs);
  snd_seq_port_subscribe_set_sender(subs, &((Alsa::Addr *)from)->addr());
  snd_seq_port_subscribe_set_dest(subs, &((Alsa::Addr *)to)->addr());
  
  ChoiceSpec choiceSpec ("Subscription options", ANY);
  std::vector<ChoiceResult> results;
  if (_jfd)
    {
      for (int j = 0; j < 3; ++j)
        results.push_back (std::pair<bool, int> (false, 0));
    }
  else
    {
      choiceSpec.addChoice (Choice ("Exclusive"));
      choiceSpec.addChoice (Choice ("Time update"));
      choiceSpec.addChoice (Choice ("Real time"));
      results = _ui->getChoice ("Subscription options", choiceSpec);
    }

  snd_seq_port_subscribe_set_exclusive(subs, (int) results[0].first);
  snd_seq_port_subscribe_set_time_update(subs, (int) results[1].first);
  snd_seq_port_subscribe_set_time_real(subs, (int) results[2].first);

  
  doPortSubscription (subs, from, to);
}

void
Driver:: subscribeClients (const APB::Addr * readAddr, const APB::Addr * writeAddr)
{
  Addr * raddr;
  Addr * waddr = NULL;
  int rclient (((const Addr *)readAddr)->client ());
  int wclient (((const Addr *)writeAddr)->client ());

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

void Driver::removeSubscription (const Subscription * subscription)
{
  snd_seq_port_subscribe_t * subs;
  snd_seq_port_subscribe_alloca(&subs);
  snd_seq_port_subscribe_set_sender(subs, &((Alsa::Addr *)(subscription->from()))->addr());
  snd_seq_port_subscribe_set_dest(subs, &((Alsa::Addr *)(subscription->to()))->addr());

  snd_seq_port_subscribe_set_exclusive(subs, subscription->getAttribute("exclusive"));
  snd_seq_port_subscribe_set_time_update(subs, subscription->getAttribute("time update"));
  snd_seq_port_subscribe_set_time_real(subs, subscription->getAttribute("realtime"));

  int err = snd_seq_get_port_subscription(_seq, subs);
  if (err != 0)
    {
      std::string s (std::string ("Subscription ") + subscription->getName() + " does not exist");
      throw Exception (s, err);
      return;
    }
  
  err = snd_seq_unsubscribe_port(_seq, subs);
  if (err < 0)
    {
      std::string s (std::string ("Could not remove subscription ") +
                     subscription->getName() + ":\n  " +
                     snd_strerror(err));
      throw Exception (s, err);
    }
  
  
  _subscriptions.remove ((Subscription *) subscription);
  _ui->log (std::string("Unsubscription ") + subscription->getName() + " succeeded");
  delete subscription;
}


/******* listen stuff ***********/

int
Driver:: createListenPort (void)
{
  int err;
  snd_seq_port_info_t * port_info;
  
  snd_seq_port_info_alloca (&port_info);
  snd_seq_port_info_set_name (port_info, "System Announcement Reciever");
  snd_seq_port_info_set_capability (port_info,
    SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE|SND_SEQ_PORT_CAP_NO_EXPORT);
  snd_seq_port_info_set_type (port_info, SND_SEQ_PORT_TYPE_APPLICATION);
 
  err = snd_seq_create_port (_seq, port_info);
  if (err)
    {
      char refresh = 0;
      write (_refreshWriteFile, &refresh, sizeof (refresh));

      std::cerr << "error creating alsa port: " << snd_strerror (err) << std::endl;
      return -1;
    }

  /* subscribe the port to the system announcer */
  err = snd_seq_connect_from (_seq,
                              snd_seq_port_info_get_port (port_info),
                              SND_SEQ_CLIENT_SYSTEM,          /* the "System" kernel client */
                              SND_SEQ_PORT_SYSTEM_ANNOUNCE);  /* the "Announce" port */
  if (err)
    {
      char refresh = 0;
      write (_refreshWriteFile, &refresh, sizeof (refresh));
            

      std::cerr << "could not connect to system announcer port: "
                << snd_strerror (err)
                << std::endl;
      return -1;
    }

  return 0;
}

void
Driver:: sendRefresh (void)
{
  char refresh = 1;
  ssize_t err;
  
  err = write (_refreshWriteFile, &refresh, sizeof (refresh));
  if (err == -1)
    {
      std::cerr << "error writing to refresh pipe: "
                << strerror (errno) 
                << std::endl;
    }
}

void
Driver:: getEvent (void)
{
  snd_seq_event_t* ev;
  int err;
  
  err = snd_seq_event_input (_seq, &ev);
  
  switch (ev->type)
    {
    case SND_SEQ_EVENT_PORT_START:
    case SND_SEQ_EVENT_PORT_EXIT:
    case SND_SEQ_EVENT_PORT_CHANGE:
    case SND_SEQ_EVENT_PORT_SUBSCRIBED:
    case SND_SEQ_EVENT_PORT_UNSUBSCRIBED:
      sendRefresh ();
      break;
    default:
      break;
    }
}

void
Driver:: refreshIMain (void)
{
  int err;
  struct pollfd * pfds;
  int nfds, i;
  unsigned short * revents;
  
  err = createListenPort ();
  if (err)
    return;
  
  nfds = snd_seq_poll_descriptors_count(_seq, POLLIN);
  pfds = new struct pollfd [nfds];
  revents = new unsigned short [nfds];
  snd_seq_poll_descriptors (_seq, pfds, nfds, POLLIN);

  for (;;)
    {
      err = poll (pfds, nfds, 1000);
      if (err == -1)
        {
          if (errno == EINTR)
            continue;
          
          std::cerr << "error polling alsa sequencer: "
                    << strerror (errno)
                    << std::endl;
          continue;
        }

      err = snd_seq_poll_descriptors_revents (_seq, pfds, nfds, revents);
      if (err)
        {
          std::cerr << "error getting alsa sequencer poll revents: "
 	            << snd_strerror (err)
                    << std::endl;
          continue;
        }

      for (i = 0; i < nfds; i++)
        {
          if (revents[i] > 0)
            getEvent ();
        }
    
    }
}

void *
Driver:: refreshMain (void * data)
{
  ((Driver *) data)->refreshIMain ();
  
  return NULL;
}



/************ plugin stuff **************/



} /* namespace Alsa */

class AlsaPlugin : public DriverPlugin
{
  public:
    virtual Driver *   getDriver       (const std::string& title, int * argc, char *** argv);
};


Driver *
AlsaPlugin:: getDriver (const std::string& title, int * argc, char *** argv)
{
  return new Alsa::Driver (title, argc, argv);
}

} /* namespace APB */

APB::DriverPlugin *
getDriverPlugin ()
{
  return new APB::AlsaPlugin ();
}


/* EOF */

