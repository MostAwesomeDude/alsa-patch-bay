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
#include "misc.h"

namespace APB {
namespace GTKmm {


UI:: UI (std::string title, int argc, char ** argv, APB::Driver * driver)
: _gtk (new Gtk::Main (argc, argv)),
  _mainWindow (title, driver, this),
  _driver (driver),
  _title (title)
{
  if (!_gtk)
    {
      std::string s (DEBUG_STRING + "failed to initialise GTKmm");
      throw APB::Exception (s);
    }

#ifdef HAVE_LADCCA
  if (_driver->getCCAClient())
    Glib::signal_idle().connect(slot(*this, &UI::idleCallback));
#endif
}  

#ifdef HAVE_LADCCA
bool
UI:: idleCallback ()
{
  cca_event_t * event;
  bool ret = true;

  if (!cca_enabled (_driver->getCCAClient()))
    return false;

  while (( event = cca_get_event (_driver->getCCAClient()) ))
    {
      switch (cca_event_get_type (event))
	{
	case CCA_Quit:
	  _gtk->quit();
	  break;
	case CCA_Server_Lost:
	  log ("LADCCA server disconnected");
	  ret = false;
	  break;
	default:
	  break;
	}

      cca_event_destroy (event);
    }

  return ret;
}
#endif /* HAVE_LADCCA */

void
UI:: log (const std::string& message)
{
  _mainWindow.log (message);
}

void
UI:: run ()
{
  _gtk->run(_mainWindow);
}

std::vector<ChoiceResult>
UI:: getChoice (std::string title, const ChoiceSpec& choiceSpec)
{
  ChoiceWindow choiceWindow (_title + ": " + title, choiceSpec);
  return choiceWindow.getChoice ();
}

void
UI::stop ()
{
  _gtk->quit ();
}


} /* namespace GTKmm */





////////////////////////////////
///////// plugin stuff /////////
////////////////////////////////


class GTKmmPlugin : public UIPlugin
{
    virtual UI *       getUI           (const std::string& title, int * argc, char *** argv, Driver * driver);
};


UI *
GTKmmPlugin:: getUI (const std::string& title, int * argc, char *** argv, Driver * driver)
{
  return new GTKmm::UI (title, *argc, *argv, driver);
}

} /* namespace APB */

APB::UIPlugin *
getUIPlugin ()
{
  return new APB::GTKmmPlugin ();
}

/* EOF */

