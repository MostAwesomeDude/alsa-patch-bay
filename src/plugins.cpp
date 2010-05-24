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

#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

#include <iostream>
#include <list>
#include <string>

#include "plugin.h"
#include "plugins.h"

namespace APB {

static void * uiPluginHandle;
static void * driverPluginHandle;

PluginSet *
getPluginSet ()
{
  static PluginSet plugs;
  return &plugs;
}

UIPlugin *
openUIPlugin (std::string pluginName)
{
  std::string fqn;
  void * dlhandle;
  GetUIPluginFunc getUIPlugin;
  UIPlugin * plugin;
  const char * error;

  fqn = std::string (PKGLIBDIR) + "/ui/" + pluginName + ".so";

  dlhandle = dlopen (fqn.c_str(), RTLD_LAZY|RTLD_GLOBAL);
  
  if (!dlhandle)
    {
      std::cerr << __FUNCTION__
                << ": could not open ui plugin '"
                << fqn
                << "': "
                << dlerror ()
                << std::endl;
      exit (1);
    }

  dlerror();
  
  /* gcc 3.x */
  getUIPlugin = (GetUIPluginFunc) dlsym (dlhandle, "_Z11getUIPluginv");
  error = dlerror ();
  if (error)
    {
      /* gcc 2.95.x */
      getUIPlugin = (GetUIPluginFunc) dlsym (dlhandle, "getUIPlugin__Fv");
      error = dlerror ();
      if (error)
        {
          std::cerr << __FUNCTION__
                    << ": could not find getUIPlugin function in plugin '"
                    << fqn
                    << "': "
                    << error
                    << std::endl;
          dlclose (dlhandle);
          exit (1);
        }
    }
  
  plugin = getUIPlugin ();
  
  uiPluginHandle = dlhandle;
  
  return plugin;
}

DriverPlugin *
openDriverPlugin (std::string pluginName)
{
  std::string fqn;
  void * dlhandle;
  GetDriverPluginFunc getDriverPlugin;
  DriverPlugin * plugin;
  const char * error;

  fqn = std::string (PKGLIBDIR) + "/driver/" + pluginName + ".so";

  dlhandle = dlopen (fqn.c_str(), RTLD_LAZY|RTLD_GLOBAL);
  
  if (!dlhandle)
    {
      std::cerr << __FUNCTION__
                << ": could not open driver plugin '"
                << fqn
                << "': "
                << dlerror ()
                << std::endl;
      exit (1);
    }

  dlerror();
  
  getDriverPlugin = (GetDriverPluginFunc) dlsym (dlhandle, "_Z15getDriverPluginv");
  error = dlerror ();
  if (error)
    {
      getDriverPlugin = (GetDriverPluginFunc) dlsym (dlhandle, "getDriverPlugin__Fv");
      error = dlerror ();
      if (error)
        {
          std::cerr << __FUNCTION__
                    << ": could not find getDriverPlugin function in plugin '"
                    << fqn
                    << "': "
                    << error
                    << std::endl;
          dlclose (dlhandle);
          exit (1);
        }
    }
  
  plugin = getDriverPlugin ();
  
  driverPluginHandle = dlhandle;
  
  return plugin;
}

void
readDir (std::string dirname, std::list<std::string>& plugins)
{
  struct dirent * dir_entry;
  DIR * dir;
  char * plugin, * ptr;

  dir = opendir (dirname.c_str());
  
  if (!dir)
    {
      std::cerr << __FUNCTION__
                << ": could not open plugin directory '"
                << dirname
                << "': "
                << strerror (errno)
                << std::endl;
      exit (1);
    }
  
  while ( (dir_entry = readdir (dir)) )
    {
      plugin = strdup (dir_entry->d_name);
      dirname = plugin;
      if (dirname == "." || dirname == "..")
        continue;
      
      ptr = strrchr (plugin, '.');
      if (!ptr) {
        free (plugin);
        continue;
      }
      *ptr = '\0';
      plugins.push_back (plugin);
      free (plugin);
    }
}

void
initPlugins ()
{
  std::string dir;
  
  dir = std::string (PKGLIBDIR) + "/driver";
  readDir (dir, getPluginSet()->driverPlugins);

  dir = std::string (PKGLIBDIR) + "/ui";
  readDir (dir, getPluginSet()->uiPlugins);
  

  if (getPluginSet()->driverPlugins.size () == 0)
    {
      std::cerr << __FUNCTION__
                << ": no driver plugins"
                << std::endl;
      exit (1);
    }

  if (getPluginSet()->uiPlugins.size () == 0)
    {
      std::cerr << __FUNCTION__
                << ": no ui plugins"
                << std::endl;
      exit (1);
    }
}

void
closePlugins ()
{
}

} /* namespace APB */


/* EOF */


