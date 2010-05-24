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

#ifndef __APB_PLUGINS_H__
#define __APB_PLUGINS_H__

#include <map>
#include <string>

#include "config.h"

#include "plugin.h"

namespace APB {

class PluginSet
{
  public:
    std::list<std::string> driverPlugins;
    std::list<std::string> uiPlugins;
};

PluginSet * getPluginSet ();

void initPlugins ();
UIPlugin * openUIPlugin (std::string);
DriverPlugin * openDriverPlugin (std::string);
void closePlugins ();

} /* namespace APB */


#endif /* __APB_PLUGINS_H__ */
