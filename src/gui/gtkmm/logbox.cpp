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

#include "logbox.h"

namespace APB {
namespace GTKmm {


LogBox:: LogBox ()
{
  _textBuffer = Gtk::TextBuffer::create();
  set_buffer (_textBuffer);
}


void LogBox::log (const std::string& message) {

  std::cout << message << std::endl;

  if (!_textBuffer) return;

  Glib::ustring l (message + "\n" + _textBuffer->get_text(_textBuffer->begin(),
                                  _textBuffer->end(), true));
  _textBuffer->set_text (l);
}

} /* namespace GTKmm */
} /* namespace APB */

/* EOF */

