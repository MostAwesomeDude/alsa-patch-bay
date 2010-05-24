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

#include <sstream>

#include "misc.h"

namespace APB {

Exception:: Exception (const std::string& d, int e)
: desc (d),
  err (e)
{
}

std::string int2string (int i) {
  static std::string str;

  std::ostringstream ss;

  ss << i;

  str = ss.str();

  return str;
}

int randomNumber (double lower, double upper) {
  return (int) ((lower-1)+(int) ((upper+1)*rand()/(RAND_MAX+1.0)));
}

} /* namespace APB */
