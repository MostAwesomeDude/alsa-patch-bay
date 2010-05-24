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

#ifndef __APB_MISC_H__
#define __APB_MISC_H__

#include <string>

namespace APB {

struct Exception
{
  std::string desc;
  int         err;
  
  Exception (const std::string&, int = 0);
};

std::string int2string   (int);
int         randomNumber (double, double);

} /* namespace APB */

#endif /* __APB_MISC_H__ */
