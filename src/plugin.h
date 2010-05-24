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

#ifndef __APB_PLUGIN_H__
#define __APB_PLUGIN_H__

#include <string>

#include "apb.h"

namespace APB {

/**************************************/
/*************** Plugin ***************/
/**************************************/

class DriverPlugin
{
  public:
    virtual Driver *     getDriver     (const std::string& title, int * argc, char *** argv) = 0;
};

class UIPlugin
{
  public:
    virtual UI *         getUI         (const std::string& title, int * argc, char *** argv, Driver * driver) = 0;
};

typedef DriverPlugin * (*GetDriverPluginFunc) ();
typedef UIPlugin *     (*GetUIPluginFunc) ();

} /* namespace APB */

#endif /* __APB_PLUGIN_H__ */
