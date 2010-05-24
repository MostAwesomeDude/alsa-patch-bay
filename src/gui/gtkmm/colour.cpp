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

#include <gtkmm-2.0/gtkmm.h>

#include "misc.h"
#include "colour.h"

namespace APB {
namespace GTKmm {

Colour Colour::_currentColour (Glib::ustring("white"));

const Colour&
Colour:: getRandomColour ()
{
  static Colour colour;
  colour.set_red (randomNumber(0, 65535));
  colour.set_green (randomNumber(0, 65535));
  colour.set_blue (randomNumber(0, 65535));
  return colour;
}


const Colour&
Colour:: getNextColour ()
{
  int i = _currentColour.get_red ();

  if (i == 0)
    {
      i = _currentColour.get_green ();
      if (i == 0)
        {
          i = _currentColour.get_blue ();
          if (i == 0)
            {
              _currentColour.set ("white");
            }
          else 
            {
              i -= _incr;
              if (i < 0)
                i = 0;
              _currentColour.set_blue (i);
            }
        }
      else
        {
          i -= _incr;
          if (i < 0)
            i = 0;
          _currentColour.set_green (i);
        }
    }
  else
    {
      i -= _incr;
      if (i < 0) i = 0;
      _currentColour.set_red (i);
    }
  
  return _currentColour;
}

Colour:: Colour ()
{
}

Colour:: Colour (std::string colour)
: Gdk::Color (Glib::ustring (colour))
{
}

Colour:: Colour (const Colour& colour)
: Gdk::Color (colour)
{
}

const Colour&
Colour:: getStartColour ()
{
  _currentColour.set (Glib::ustring ("black"));
  return _currentColour;
}

void
Colour:: setToStartColour ()
{
  *this = getStartColour ();
}

void
Colour:: setToNextColour ()
{
  *this = getNextColour ();
}



} /* namespace GTKmm */
} /* namespace APB */

/* EOF */

