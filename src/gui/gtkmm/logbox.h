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

#ifndef __APB_GTKMM_LOGBOX_H__
#define __APB_GTKMM_LOGBOX_H__

#include <string>

#include <gtkmm-2.0/gtkmm.h>

namespace APB {
namespace GTKmm {

class LogBox : public Gtk::TextView
{
  private:
    Glib::RefPtr<Gtk::TextBuffer> _textBuffer;
    
  public:
         LogBox ();
    void log    (const std::string&);
};

} /* namespace GTKmm */
} /* namespace APB */

#endif /* __APB_GTKMM_LOGBOX_H__ */

