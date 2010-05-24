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
#include <string>

#include "apb.h"
#include "plugin.h"
#include "driver.h"
#include "ui.h"
#include "misc.h"

namespace APB {

AlsaPatchBay:: AlsaPatchBay (std::string title, int * argc, char *** argv,
                             UIPlugin * uiPlugin, DriverPlugin * driverPlugin
#ifdef HAVE_LADCCA
                             , cca_client_t * client
#endif
                             )
{
  _driver = driverPlugin->getDriver (title, argc, argv);
#ifdef HAVE_LADCCA
  _driver->setCCAClient (client);
#endif
  _ui = uiPlugin->getUI (title, argc, argv, _driver);
  _driver->setUI (_ui);

#ifdef HAVE_LADCCA
  if (cca_enabled (client))
    _ui->log (std::string ("Connected to LADCCA server '") +
              cca_get_server_name (client) + "'");
  else
    _ui->log (std::string ("Could not connect to LADCCA server; LADCCA disabled"));
#endif
}

void
AlsaPatchBay:: run ()
{
  _ui->run ();
}

} /* namespace APB */
