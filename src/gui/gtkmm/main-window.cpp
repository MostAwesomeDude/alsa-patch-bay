/*
 * ALSA Patch Bay
 *
 * Copyright (C) 2002,2003 Robert Ham (node@users.sourceforge.net)
 *
 * You have permission to use this file under the GNU General
 * Public License, version 2 or later.  See the file COPYING
 * for the full text.
 *
 */

#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "main-window.h"
#include "driver.h"
#include "button-array.h"
#include "apb-gtkmm.h"

namespace APB {
namespace GTKmm {

void
MainWindow:: jfdToggled ()
{
  _driver->setJFD (_jfdButton.get_active ());
}

void
MainWindow:: log (const std::string& message)
{
  _logBox.log (message);
}

bool
MainWindow:: refreshIdle ()
{
  char refresh;
  ssize_t err;

  usleep (100);
  
/*  std::cerr << "attempting read from fd" << _driver->getRefreshReadFile()
            << std::endl; */
  err = read (_driver->getRefreshReadFile(), &refresh, sizeof (refresh));
  if (err == -1)
    {
      if (errno == EAGAIN) /* no data */
        return true;

      log (std::string ("Error reading from refresh pipe, disabling refresh callback: ") + strerror (errno));
      return false;
    }
  
  if (err != sizeof (refresh))
    {
      log ("Couldn't read entire char from refresh pipe (err.. this should, like, never ever happen)");
      return true;
    }
  
  if (!refresh)
    {
      log ("The refresh thread exited; disabling refresh callback");
      return false;
    }
  
  /* finally do the refresh */
  _buttonArray.refreshArray ();

  return true;
}

MainWindow:: MainWindow (std::string title, APB::Driver * driver, APB::UI * ui)
: _logBox(),
  _buttonArray (driver, ui),
  _jfdButton ("JFD"),
  _driver (driver) {

  set_title (Glib::ustring (title));
  set_default_size (550, 300);
  std::string iconFilename (std::string(APB_DESKTOP_PREFIX) + "/pixmaps/5-pin-din.png");

  try
    {
      set_icon (Gdk::Pixbuf::create_from_file (iconFilename));
    }
  catch (Glib::FileError e)
    {
      std::cerr << "could not load icon " << iconFilename
           << ": " << strerror ((int) e.code()) << std::endl;
    }
  catch (Gdk::PixbufError e)
    {
      std::cerr << "could not load icon " << iconFilename
           << " due to dodgey image data" << std::endl;
    }

  Gtk::VBox * vbox = manage (new Gtk::VBox());
  Gtk::VPaned * vpane = manage (new Gtk::VPaned());
  vpane->unset_flags (Gtk::CAN_FOCUS);

  Gtk::ScrolledWindow * scrollWindow =
    manage (new Gtk::ScrolledWindow ());
  scrollWindow->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  Gtk::Adjustment * ha = manage (new Gtk::Adjustment (0.5, 0.0, 1.0));
  Gtk::Adjustment * va = manage (new Gtk::Adjustment (0.5, 0.0, 1.0));
  Gtk::Viewport * viewport = manage (new Gtk::Viewport(*ha, *va));

  Gtk::HandleBox * toolbarHandle = manage (new Gtk::HandleBox ());

  Gtk::Toolbar * toolbar = manage (new Gtk::Toolbar());
  toolbar->set_toolbar_style (Gtk::TOOLBAR_BOTH);
  toolbar->set_icon_size (Gtk::IconSize (Gtk::ICON_SIZE_SMALL_TOOLBAR));

  Gtk::Toolbar_Helpers::StockElem * quitButton =
    new Gtk::Toolbar_Helpers::StockElem (Gtk::StockID (Gtk::Stock::QUIT),
                                         SigC::slot(*(GTKmm::UI *)ui, &UI::stop),
                                         Glib::ustring ("Quit from Alsa Patch Bay"),
                                         Glib::ustring ("Button to quit from Alsa Patch Bay"));

  Gtk::Toolbar_Helpers::StockElem * refreshButton =
    new Gtk::Toolbar_Helpers::StockElem (Gtk::StockID (Gtk::Stock::REFRESH),
                                         SigC::slot(_buttonArray, &ButtonArray::refreshArray),
                                         Glib::ustring ("Refresh the ports and subscriptions"),
                                         Glib::ustring ("Button to refresh the ports and subscriptions"));



    
  _jfdButton.set_active (true);
  _jfdButton.unset_flags (Gtk::CAN_FOCUS);
  _jfdButton.signal_clicked().connect (SigC::slot(*this, &MainWindow::jfdToggled));

  Gtk::Toolbar_Helpers::Element * jfdButton =
    new Gtk::Toolbar_Helpers::Element (_jfdButton,
                                       Glib::ustring ("Whether or not to be asked for subscription options"));

  Gtk::ScrolledWindow * logScroll = 
    manage (new Gtk::ScrolledWindow ());
  logScroll->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  logScroll->unset_flags (Gtk::CAN_FOCUS);

  _logBox.set_editable (false);
  _logBox.set_wrap_mode (Gtk::WRAP_WORD);
  _logBox.unset_flags (Gtk::CAN_FOCUS);

  viewport->add (_buttonArray);
  scrollWindow->add (*viewport);
  logScroll->add (_logBox);
  vpane->pack2 (*logScroll, Gtk::SHRINK);
  vpane->pack1 (*scrollWindow, Gtk::EXPAND);
  toolbar->tools().push_back (*refreshButton);
  toolbar->tools().push_back (*quitButton);
  toolbar->tools().push_back (Gtk::Toolbar_Helpers::Space ());
  toolbar->tools().push_back (*jfdButton);
  toolbarHandle->add (*toolbar);
  vbox->pack_start (*toolbarHandle, false, true, 0);
  vbox->pack_start (*vpane);
  add (*vbox);

  /* add the refresh listener */
  Glib::signal_idle().connect(slot(*this, &MainWindow::refreshIdle));
   

  show_all ();
}

} /* namespace GTKmm */
} /* namespace APB */

/* EOF */

