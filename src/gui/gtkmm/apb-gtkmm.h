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

#ifndef __APB_GTKMM_H__
#define __APB_GTKMM_H__

#include "config.h"

#include <string>
#include <vector>

#include <gtkmm-2.0/gtkmm.h>

#include "ui.h"
#include "main-window.h"
#include "choice.h"

namespace APB {

class Driver;

namespace GTKmm {


class UI : public APB::UI, public SigC::Object
{
  private:
    Gtk::Main *    _gtk;
    MainWindow     _mainWindow;
    APB::Driver *  _driver;
    std::string    _title;
  
  public:
                                      UI           (std::string, int, char **, Driver *);
    virtual void                      log          (const std::string& message);
    virtual void                      run          ();
    virtual std::vector<ChoiceResult> getChoice    (std::string title, const ChoiceSpec& choiceSpec);
            void                      stop         ();

#ifdef HAVE_LADCCA
            bool                      idleCallback ();
#endif

};

} /* namespace GTKmm */
} /* namespace APB */

#endif /* __APB_GTKMM_H__ */

