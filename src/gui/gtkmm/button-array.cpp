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


#include "apb-gtkmm.h"

#include "plugin.h"
#include "driver.h"
#include "subscription.h"
#include "addr.h"
#include "port-button.h"
#include "ui.h"
#include "misc.h"

namespace APB {
namespace GTKmm {


Colour& ButtonArray::getColour (int index) {
  int lsize = _lineColours.size();
  if (index < lsize) {
    return _lineColours[index];
  }
  for (int i = lsize-1; i <= index; ++i) {
    _lineColours.push_back (Colour::getRandomColour());
  }
  return _lineColours[index];
}

ButtonArray::ButtonArray (APB::Driver * driver, APB::UI * ui)
  : _driver (driver),
    _ui (ui),
    _selectedPortButton (0),
    _selectedClientButton (0) {
  driver->refreshPorts();
  setReadPorts (driver->getReadPorts ());
  setWritePorts (driver->getWritePorts ());
  driver->refreshSubscriptions();
  setSubscriptions (driver->getSubscriptions ());

  bool(ButtonArray::* redrawFunc)(GdkEventExpose *) = &ButtonArray::redrawSubscriptions;
  _drawingArea.signal_expose_event().connect (SigC::slot (*this, redrawFunc));

  pack_start (_readBox);
  pack_start (_drawingArea);
  pack_start (_writeBox);

  show_all ();
}


void ButtonArray::refreshArray () {
  try
    {
      _driver->refreshPorts();
    }
  catch (Exception e)
    {
       Gtk::MessageDialog md (e.desc, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
       md.run ();
    }

  setReadPorts (_driver->getReadPorts());
  setWritePorts (_driver->getWritePorts ());

  try {
    _driver->refreshSubscriptions();
  } catch (Exception e) {
     Gtk::MessageDialog md (e.desc, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
     md.run ();
  }

  setSubscriptions (_driver->getSubscriptions ());
  redrawSubscriptions ();
  _drawingArea.queue_draw();
}

void ButtonArray::setReadPorts (const std::list<Addr *>& ports) {
  setPorts (_readBox, _readButtons, ports, true);
}

void ButtonArray::setWritePorts (const std::list<Addr *>& ports) {
  setPorts (_writeBox, _writeButtons, ports, false);
}

void ButtonArray::setPorts (Gtk::VBox& buttonBox,
                            std::list<PortButton *>& buttonList,
                            const std::list<Addr *>& ports,
                            bool read) {
  for (std::list<PortButton *>::iterator i = buttonList.begin();
       i != buttonList.end();
       ++i) {
    buttonBox.remove (**i);
  }
  buttonList.clear ();
  
  guint index = 0;
  for (std::list<Addr *>::const_iterator i = ports.begin();
       i != ports.end();
       ++i) {
    PortButton * portButton = manage (new PortButton (*i, this, read, index++));
    buttonBox.pack_start (*portButton, false, true, 0);
    buttonList.push_back (portButton);
  }

  show_all ();
}

void ButtonArray::setSubscriptions (const std::list<const Subscription *>& subscriptions) {
  _subscriptions.clear ();
  _subscriptions = subscriptions;
}

const PortButton * ButtonArray::findButton
  (const std::list<PortButton *>& buttonList, const Addr * addr) {
  for (std::list<PortButton *>::const_iterator i = buttonList.begin();
       i != buttonList.end();
       ++i) {
    if (!*i) {
      std::cerr << DEBUG_STRING << "portbutton is null!" << std::endl;
    }
    if (!(*i)->addr()) {
      std::cerr << DEBUG_STRING << "portbutton's addr is null!" << std::endl;
    }
    const Addr * buttonAddr = (*i)->addr();
    if (buttonAddr->equals (addr)) {
      return (*i);
    }
  }
  return 0;
}

void
ButtonArray:: getLine (const Subscription * sub, int &bx, int &by, int &ex, int &ey)
{
  PortButton * readButton = (PortButton *) findReadButton (sub->from());
  if (!readButton)
    {
      std::cerr << DEBUG_STRING << "readbutton for " << sub->from()->getName() << " not found!" << std::endl;
      return;
    }

  PortButton * writeButton = (PortButton *) findWriteButton (sub->to());
  if (!writeButton)
    {
      std::cerr << DEBUG_STRING << "writebutton for " << sub->to()->getName() << " not found!" << std::endl;
      return;
    }
  
  GtkAllocation readAlloc (readButton->get_allocation());
  GtkAllocation writeAlloc (writeButton->get_allocation());
  
  bx = 0;
  by = readAlloc.y + readAlloc.height / 2;
  ex = _drawingArea.get_allocation().width;
  ey = writeAlloc.y + writeAlloc.height / 2;
}

bool
ButtonArray::  redrawSubscriptions ()
{
  Glib::RefPtr<Gdk::Window> window = _drawingArea.get_window();
  if (window.is_null())
    {
      std::cerr << DEBUG_STRING << "window == null; returning" << std::endl;
      return false;
    }
  Glib::RefPtr<Gdk::GC> gc = Gdk::GC::create (window);
  

  int cw, ch;
  window->get_size (cw, ch);
  window->draw_rectangle (_drawingArea.get_style()->get_bg_gc(Gtk::STATE_NORMAL), 1,
                          0, 0, cw, ch);

  int bx, by, ex, ey;
  for (std::list<const Subscription *>::const_iterator i = _subscriptions.begin();
       i != _subscriptions.end();
       ++i)
    {
      getLine (*i, bx, by, ex, ey);

      PortButton * readButton = (PortButton *) findReadButton ((*i)->from());

      Colour& colour = getColour (readButton->index());
      _drawingArea.get_colormap()->alloc_color (colour);
      gc->set_foreground(colour);

      window->draw_line (gc, bx, by, ex, ey);
    }

  return true;
}


void
ButtonArray:: readButtonClicked (PortButton * portButton)
{
  if (_selectedPortButton)
    {
      if (_selectedPortButton == portButton)
        {
          ((Gtk::Label*)(_selectedPortButton->get_child()))->set_markup
            ("<span foreground=\"#0000ff\">" +
             ((Gtk::Label*)(_selectedPortButton->get_child()))->get_text()
             + "</span>");
          _selectedClientButton = _selectedPortButton;
          _selectedPortButton = 0;
          return;
        }
      else
        {
          ((Gtk::Label*)(_selectedPortButton->get_child()))->set_markup
            (((Gtk::Label*)(_selectedPortButton->get_child()))->get_text());
        }
    }
  else if (_selectedClientButton)
    {
      ((Gtk::Label*)(_selectedClientButton->get_child()))->set_markup
        (((Gtk::Label*)(_selectedClientButton->get_child()))->get_text());
      if (_selectedClientButton == portButton)
        {
          _selectedClientButton = 0;
          return;
        }
    }    
  
  _selectedPortButton = portButton;
  ((Gtk::Label*)(_selectedPortButton->get_child()))->set_markup
    ("<span foreground=\"#ff0000\">" +
     ((Gtk::Label*)(_selectedPortButton->get_child()))->get_text()
     + "</span>");
}

bool
ButtonArray:: removeSubscriptions (const Addr * writeAddr)
{
  bool did_something = false;
  for (std::list<const Subscription *>::iterator it = _subscriptions.begin();
       it != _subscriptions.end();
       ++it)
    {
      if ((*it)->to()->equals(writeAddr))
        {
          try
            {
              _driver->removeSubscription (*it);
              did_something = true;
            }
          catch (Exception e)
            {
              _ui->log (e.desc);
              Gtk::MessageDialog md (e.desc, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
              md.run();
              return did_something;
            }
        }
    }
  
  return did_something;
}

bool
ButtonArray:: subscribePorts (const Addr * readAddr, const Addr * writeAddr)
{
  try
    {
      _driver->subscribePorts (readAddr, writeAddr);
    }
  catch (Exception e)
    {
      _ui->log (std::string("Could not subscribe ports:\n  ") + e.desc);
      Gtk::MessageDialog md (e.desc, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
      md.run();
      return false;
    }
  
  return true;
}

bool
ButtonArray:: subscribeClients (const Addr * readAddr, const Addr * writeAddr)
{
  try
    {
      _driver->subscribeClients (readAddr, writeAddr);
    }
  catch (Exception e)
    {
      _ui->log (std::string("Could not subscribe ports:\n  ") + e.desc);
      Gtk::MessageDialog md (e.desc, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
      md.run();
      return false;
    }
  
  return true;
}

void
ButtonArray:: writeButtonClicked (PortButton * portButton)
{
  if (!_selectedPortButton && !_selectedClientButton)
    {
      bool did_something = removeSubscriptions (portButton->addr ());

      if (did_something)
        {
          _driver->refreshSubscriptions ();
          setSubscriptions (_driver->getSubscriptions ());
          redrawSubscriptions ();
        }
      
      return;
    }
  
  bool worked;
  if (_selectedPortButton)
    worked = subscribePorts (_selectedPortButton->addr (), portButton->addr ());
  else
    worked = subscribeClients (_selectedClientButton->addr (), portButton->addr ());
  
  if (!worked)
    return;

  _driver->refreshSubscriptions ();
  setSubscriptions (_driver->getSubscriptions ());

  if (_selectedPortButton)
    {
      ((Gtk::Label*)(_selectedPortButton->get_child()))->set_markup
          (((Gtk::Label*)(_selectedPortButton->get_child()))->get_text());
      _selectedPortButton = 0;
    }
  else
    {
      ((Gtk::Label*)(_selectedClientButton->get_child()))->set_markup
          (((Gtk::Label*)(_selectedClientButton->get_child()))->get_text());
      _selectedClientButton = 0;
    }

  redrawSubscriptions();
}

const PortButton *
ButtonArray:: findReadButton (const Addr * addr)
{
  return findButton (_readButtons, addr);
}

const PortButton *
ButtonArray:: findWriteButton (const Addr * addr)
{
  return findButton (_writeButtons, addr);
}


} /* namespace GTKmm */
} /* namespace APB */

/* EOF */

