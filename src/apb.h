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

#ifndef __APB_APB_H__
#define __APB_APB_H__

#include <list>
#include <string>
#include <vector>
#include <map>

#include "config.h"

#ifdef HAVE_LADCCA
# include <ladcca/ladcca.h>
#endif

#define DEBUG_STRING std::string(__FILE__) + ":" + APB::int2string(__LINE__) + ":" + \
      __FUNCTION__ + ": "
      
      
namespace APB {

class UIPlugin;
class DriverPlugin;
class UI;
class Driver;

class AlsaPatchBay {
  private:
    UI * _ui;
    Driver * _driver;
  public:
    AlsaPatchBay (std::string title, int * argc, char *** argv,
                  UIPlugin * uiPlugin, DriverPlugin * driverPlugin
#ifdef HAVE_LADCCA
                  , cca_client_t * client
#endif
                  );

    void run ();
};

} /* namespace APB */

#endif /* __APB_APB_H__ */
