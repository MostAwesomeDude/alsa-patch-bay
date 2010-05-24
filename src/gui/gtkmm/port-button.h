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

#ifndef __APB_GTKMM_PORT_BUTTON_H__
#define __APB_GTKMM_PORT_BUTTON_H__

#include <gtkmm-2.0/gtkmm.h>

namespace APB {

class Addr;

namespace GTKmm {

class ButtonArray;

class PortButton : public Gtk::Button
{
  private:
    const Addr *  _addr;
    ButtonArray * _buttonArray;
    bool          _read;
    guint         _index;
  protected:
    virtual void on_clicked ();
  public:
    PortButton (const Addr * addr,
                ButtonArray * buttonArray,
                bool read,
                guint index = 0);
    void         addr  (const Addr * addr);
    const Addr * addr  () const;
    void         index (guint index);
    guint        index () const;
};

} /* namespace GTKmm */
} /* namespace APB */

#endif /* __APB_GTKMM_PORT_BUTTON_H__ */

