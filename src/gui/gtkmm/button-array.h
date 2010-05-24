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

#ifndef __APB_BUTTON_ARRAY_H__
#define __APB_BUTTON_ARRAY_H__

#include <iostream>
#include <list>
#include <string>

#include <gtkmm-2.0/gtkmm.h>

#include "colour.h"

namespace APB {

class Addr;
class Subscription;
class Driver;
class UI;

namespace GTKmm {

class PortButton;

class ButtonArray : public Gtk::HBox {
  private:
    std::vector<Colour>             _lineColours;
    std::list<PortButton *>         _readButtons;
    std::list<PortButton *>         _writeButtons;
    std::list<const Subscription *> _subscriptions;
    Gtk::VBox                       _readBox;
    Gtk::VBox                       _writeBox;
    Gtk::DrawingArea                _drawingArea;
    
    APB::Driver *                   _driver;
    APB::UI *                       _ui;
    
    PortButton *                    _selectedPortButton;
    PortButton *                    _selectedClientButton;
    
    void               setPorts (Gtk::VBox&, std::list<PortButton *>&, const std::list<Addr *>&, bool);
    bool               redrawSubscriptions (GdkEventExpose * event) { return redrawSubscriptions (); }
    bool               redrawSubscriptions ();
    const PortButton * findButton (const std::list<PortButton *>&, const Addr *);
    const PortButton * findReadButton (const Addr * addr);
    const PortButton * findWriteButton (const Addr * addr);
    void               readButtonClicked (PortButton *);
    void               writeButtonClicked (PortButton *);

    bool               removeSubscriptions (const Addr * writeAddr);
    bool               subscribePorts (const Addr * readAddr, const Addr * writeAddr);
    bool               subscribeClients (const Addr * readAddr, const Addr * writeAddr);
  public:
            ButtonArray (APB::Driver *, APB::UI *);
    Colour& getColour (int);
    void    getLine (const Subscription *, int &, int &, int &, int &);
    void    setReadPorts (const std::list<Addr *>&);
    void    setWritePorts (const std::list<Addr *>&);
    void    setSubscriptions (const std::list<const Subscription *>&);
    void    refreshArray ();


  friend class APB::GTKmm::PortButton;
};

} /* namespace GTKmm */
} /* namespace APB */

#endif /* __APB_BUTTON_ARRAY_H__ */

