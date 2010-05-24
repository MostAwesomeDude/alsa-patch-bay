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

#ifndef __APB_GTKMM_MAIN_WINDOW_H__
#define __APB_GTKMM_MAIN_WINDOW_H__

#include <gtkmm-2.0/gtkmm.h>

#include "logbox.h"
#include "button-array.h"

namespace APB {

class Driver;

namespace GTKmm {

/*
class JFDButton : public Gtk::Toolbar_Helpers::ToggleElem
{
  public:
    JFDButton (const SigC::Slot0<void>& callback)
    : Gtk::Toolbar_Helpers::ToggleElem (Glib::ustring ("JFD"),
        callback, Glib::ustring ("Whether or not to be asked for subscription options")) {}
        
    bool get_active ()
    {
      return ((Gtk::ToggleButton *) widget_)->get_active ();
    }
};
*/

class MainWindow : public Gtk::Window
{
  protected:
    LogBox                             _logBox;
    ButtonArray                        _buttonArray;
    Gtk::ToggleButton                  _jfdButton;
    APB::Driver *                      _driver;
  public:
         MainWindow (std::string, APB::Driver *, APB::UI *);
    void log        (const std::string& message);
    void jfdToggled ();
    bool refreshIdle ();
};

} /* namespace GTKmm */
} /* namespace APB */

#endif /* __APB_GTKMM_MAIN_WINDOW_H__ */

