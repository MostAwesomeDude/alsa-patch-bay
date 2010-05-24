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

#ifndef __APB_GTKMM_COLOUR_H__
#define __APB_GTKMM_COLOUR_H__

#include <string>

namespace APB {
namespace GTKmm {

class Colour : public Gdk::Color
{
  private:
    static Colour      _currentColour;
    static const guint _incr = 10000;
  public:
                         Colour           ();
                         Colour           (std::string colour);
                         Colour           (const Colour& colour);
    static const Colour& getStartColour   ();
    static const Colour& getRandomColour  ();
    static const Colour& getNextColour    ();
    void                 setToStartColour ();
    void                 setToNextColour  ();
};

} /* namespace GTKmm */
} /* namespace APB */

#endif /* __APB_GTKMM_COLOUR_H__ */

