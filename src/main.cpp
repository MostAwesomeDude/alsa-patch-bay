/*
 * ALSA Patch Bay
 *
 * Copyright (C) 2002, 2003 Robert Ham (node@users.sourceforge.net)
 *
 * You have permission to use this file under the GNU General
 * Public License, version 2 or later.  See the file COPYING
 * for the full text.
 *
 */

#include "config.h"

#include <map>
#include <string>
#include <iostream>

#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdio.h>

#include "apb.h"
#include "plugins.h"
#include "misc.h"

using namespace APB;

void print_help () {
  printf("%s version %s\n", PACKAGE_NAME, PACKAGE_VERSION);
  printf("Copyright (C) 2002, 2003 Robert Ham <rah@bash.sh>\n");
  printf("\n");
  printf("This program comes with ABSOLUTELY NO WARRANTY.  You are licensed to use it\n");
  printf("under the terms of the GNU General Public License, version 2 or later.  See\n");
  printf("the COPYING file that came with this software for details.\n");
  printf("\n");
  printf(" -h, --help                  Display this help info\n");
  
  printf(" -u, --ui <UI>               Specify which user interface to use.\n");
  printf("                             <UI> can be one of: ");
  for (std::list<std::string>::iterator i = getPluginSet()->uiPlugins.begin();
       i != getPluginSet()->uiPlugins.end ();
       ++i)
    {
      printf ("%s ", i->c_str());
    }
  printf("\n");

  printf(" -d, --driver <DRIVER>       Specify which driver to use.\n");
  printf("                             <DRIVER> can be one of: ");
  for (std::list<std::string>::iterator i = getPluginSet()->driverPlugins.begin();
       i != getPluginSet()->driverPlugins.end ();
       ++i)
    {
      printf ("%s ", i->c_str());
    }
  printf("\n");

#ifdef HAVE_LADCCA
  printf("     --ladcca-port=<port>    Connect to ladcca server on port <port>\n");
#endif

  printf ("\n");
}

int main(int argc, char ** argv) {
  srand (time(0));
  using namespace APB;
  AlsaPatchBay * apb;
  const char * ui = 0;
  const char * driver = 0;
  UIPlugin * uiPlugin;
  DriverPlugin * driverPlugin;
  char * driverExecName;
  char * ptr;

  const char * options = "hu:d:";
  struct option long_options[] = {
    { "help", 0, NULL, 'h' },
    { "ui", 1, NULL, 'u' },
    { "driver", 1, NULL, 'd' },
    { 0, 0, 0, 0 }
  };
  int opt;
#ifdef HAVE_LADCCA
  cca_client_t * cca_client;
  cca_args_t * cca_args;

  cca_args = cca_extract_args (&argc, &argv);
#endif
  


  initPlugins ();

  /* extract the driver name from argv[0] */
  driverExecName = strdup (argv[0]);
  
  driver = strrchr (driverExecName, '/');
  if (!driver)
    driver = driverExecName;
  else
    driver++;
  
  ptr = strchr ((char*)driver, '-');
  if (ptr)
    *ptr = '\0';
  

  while ((opt = getopt_long (argc, argv, options, long_options, NULL)) != -1)
    {
      switch (opt)
        {
          case 'h':
            print_help ();
            exit (0);
            break;
          case 'u':
            ui = optarg;
            break;
          case 'd':
            driver = optarg;
            break;
          case ':':
          case '?':
            print_help ();
            exit (1);
            break;
        }
    }
  
  
  if (!ui)
    {
      ui = getPluginSet()->uiPlugins.begin()->c_str();
    }
  
  if (!driver)
    {
      driver = getPluginSet()->driverPlugins.begin()->c_str();
    }

  uiPlugin = openUIPlugin (ui);
  driverPlugin = openDriverPlugin (driver);

#ifdef HAVE_LADCCA
  cca_client = cca_init (cca_args, PACKAGE_STRING, 0, CCA_PROTOCOL (2,0));
#endif

  char title[32];
  sprintf (title, "%s Patch Bay", driver);
  ptr = title;
  while (*ptr != ' ')
    {
      *ptr = toupper (*ptr);
      ptr++;
    }
  
  try {
#ifdef HAVE_LADCCA
    apb = new AlsaPatchBay (title, &argc, &argv, uiPlugin, driverPlugin, cca_client);
#else
    apb = new AlsaPatchBay (title, &argc, &argv, uiPlugin, driverPlugin);
#endif
  } catch (Exception e) {
    std::cerr << std::endl << "Caught exception while creating AlsaPatchBay: " << e.desc << std::endl;
    return 1;
  }
  apb->run ();

  closePlugins ();
  
  return 0;
}

