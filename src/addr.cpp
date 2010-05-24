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

#include "addr.h"

namespace APB {

/* this is here because g++ moans if classes with virtual functions don't
 * have a virtual destructor, and subclasses still seem to need some code
 * even tho it's declared pure virtual.  believe me, this is about the best
 * it can get.
 */
Addr:: ~Addr ()
{
}

} /* namespace APB */

/* EOF */

