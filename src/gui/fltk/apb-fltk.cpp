/*
 * ALSA Patch Bay
 *
 * Copyright (C) 2002,2003 Robert Ham (node@users.sourceforge.net)
 *
 * You have permission to use this file under the GNU General
 * Public License, version 2 or later.  See the file COPYING
 * for the full text.
 *
 */

#include <iostream>
#include <vector>

#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <FL/fl_draw.H>
#include <FL/Enumerations.H>
#include <FL/Fl_Round_Button.H>
#include <FL/x.H>
#include <X11/Xlib.h>

#include "apb-fltk.h"
#include "5-pin-din.xbm"
#include "plugin.h"
#include "driver.h"
#include "misc.h"

namespace APB {
namespace FLTK {

/**********************************
 ********** MainWindow ************
 **********************************/

void
MainWindow:: refreshPressed ()
{
  refreshButtons ();
  refreshSubscriptions ();
  redraw ();
}

void
MainWindow:: quitPressed ()
{
  delete this;
}

void
MainWindow:: jfdToggled (bool on)
{
  _driver->setJFD (on);
}

void
MainWindow:: setSelectedPortButton (PortButton * portButton)
{
  portButton->labelcolor (FL_RED);
  portButton->redraw();
  _selectedPortButton = portButton;
  _selectedClientButton = 0;
}

void
MainWindow:: setSelectedClientButton (PortButton * portButton)
{
  portButton->labelcolor (FL_BLUE);
  portButton->redraw();
  _selectedClientButton = portButton;
  _selectedPortButton = 0;
}

void
MainWindow:: unsetSelectedButtons ()
{
  if (_selectedPortButton)
    {
      _selectedPortButton->labelcolor (FL_BLACK);
      _selectedPortButton->redraw();
      _selectedPortButton = 0;
    }
  if (_selectedClientButton)
    {
      _selectedClientButton->labelcolor (FL_BLACK);
      _selectedClientButton->redraw();
      _selectedClientButton = 0;
    }
}

void
MainWindow:: readPressed (PortButton * portButton)
{
  PortButton * oldPortButton;
  if (_selectedPortButton)
    {
      oldPortButton = _selectedPortButton;
      unsetSelectedButtons ();
      if (oldPortButton == portButton)
        setSelectedClientButton (portButton);
    }
  else if (_selectedClientButton)
    unsetSelectedButtons ();
  else
    setSelectedPortButton (portButton);
}

void
MainWindow:: unsubscribePorts (PortButton * portButton)
{
  bool did_something = false;
  for (std::list<const Subscription *>::iterator sub = _subscriptions.begin();
       sub != _subscriptions.end();
       ++sub)
    {
      if ((*sub)->to()->equals (portButton->getAddr ()))
        {
          try
            {
              _driver->removeSubscription (*sub);
              did_something = true;
            }
          catch (Exception e)
            {
              log (e.desc);
              return;
            }
        }
    }
    
  if (did_something)
    {
      refreshSubscriptions ();
      redraw ();
    }
  
  return;
}

void
MainWindow:: writePressed (PortButton * portButton)
{
  if (!_selectedPortButton && !_selectedClientButton)
    {
      unsubscribePorts (portButton);
      return;
    }
  
  if (_selectedPortButton)
    {
      try
        {
          _driver->subscribePorts (_selectedPortButton->getAddr (),
                                   portButton->getAddr ());
        }
      catch (Exception e)
        {
          log (e.desc);
          return;
        }
    }
  else
    {
      try
        {
          _driver->subscribeClients (_selectedClientButton->getAddr (),
                                     portButton->getAddr ());
        }
      catch (Exception e)
        {
          log (e.desc);
          return;
        }
    }
  
  refreshSubscriptions ();
  redraw ();
  
  unsetSelectedButtons ();
}

PortButton *
MainWindow:: findPortButton (std::list<PortButton *>& portButtons, const Addr * addr)
{
  for (std::list<PortButton *>::iterator i = portButtons.begin();
       i != portButtons.end();
       ++i)
    {
      if ((*i)->getAddr()->equals (addr))
        return *i;
    }
  
  return 0;
}

PortButton *
MainWindow:: findReadButton (const Addr * addr)
{
  return findPortButton (_readButtons, addr);
}

PortButton *
MainWindow:: findWriteButton (const Addr * addr)
{
  return findPortButton (_writeButtons, addr);
}


void
MainWindow:: refreshButtons ()
{
  _driver->refreshPorts();

  Callback callback;

  callback = (void (*) (Fl_Widget *, void *)) &MainWindow::readPressed;
  
  refreshButtonPack (_readButtons,
                     _driver->getReadPorts (),
                     _readPack,
                     callback);

  callback = (void (*) (Fl_Widget *, void *)) &MainWindow::writePressed;

  refreshButtonPack (_writeButtons,
                     _driver->getWritePorts (),
                     _writePack,
                     callback);
}

void
MainWindow:: refreshButtonPack (std::list<PortButton *>& portButtons,
                                const std::list<Addr *>& addresses,
                                Fl_Pack * pack,
                                Callback callback)
{
  for (std::list<PortButton *>::iterator i = portButtons.begin();
       i != portButtons.end();
       ++i)
    {
      pack->remove (**i);
      delete *i;
    }
  portButtons.clear();

  PortButton * portButton;
  int index = 0;
  pack->begin ();
  for (std::list<Addr *>::const_iterator i = addresses.begin();
       i != addresses.end();
       ++i, ++index)
    {
      portButton = new PortButton (0, index * getButtonHeight(),
                                   getButtonWidth(), getButtonHeight(), *i, index);
      portButton->align (FL_ALIGN_WRAP|FL_ALIGN_CENTER);
      portButton->callback (callback, this);
      portButton->redraw ();
      portButtons.push_back (portButton);
    }
  pack->end ();
}

void
MainWindow:: refreshSubscriptions ()
{
  _driver->refreshSubscriptions ();
  _subscriptions.clear ();
  _subscriptions = _driver->getSubscriptions ();
}

void
MainWindow:: resize (int x, int y, int w, int h)
{
  Fl_Window::resize (x, y, w, h);
 
  int packWidth = (w/7) * 3;

  _log->resize (0, h - getLogHeight(), w, getLogHeight());

  _scrollPane->resize (0, getButtonHeight(), w, h - getLogHeight() - getButtonHeight());
  
  _readPack->resize (_scrollPane->x(), _scrollPane->y() + 2,
                     packWidth, getButtonHeight() * _readButtons.size());
  _writePack->resize (_scrollPane->x() + _scrollPane->w() - packWidth - 18,
                      _scrollPane->y() + 2, packWidth, getButtonHeight() * _writeButtons.size());
}

void
MainWindow:: draw ()
{
  Fl_Window::draw ();
  PortButton * readButton;
  PortButton * writeButton;
  int bX, bY, eX, eY;

  fl_push_clip(_scrollPane->x(), _scrollPane->y(),
               _scrollPane->w(), _scrollPane->h());

  for (std::list<const Subscription *>::iterator i = _subscriptions.begin();
       i != _subscriptions.end();
       ++i)
    {
      readButton = findReadButton ((*i)->from());
      writeButton = findWriteButton ((*i)->to());
      
      bX = readButton->x() + readButton->w();
      bY = readButton->y() + (readButton->h() / 2);
      eX = writeButton->x() - 1;
      eY = writeButton->y() + (writeButton->h() / 2);

      Fl_Color oldColor = fl_color ();
      fl_color (getColour (readButton->getIndex ()));
      fl_line(bX, bY, eX, eY);
      fl_color (oldColor);
      
    }

  fl_pop_clip ();
}

Fl_Color
MainWindow:: getColour (unsigned int index)
{
  static std::vector<Fl_Color> colours;
  
  if (index + 1 > colours.size())
    {
      for (unsigned int n = colours.size(); n <= index; ++n)
        {
          colours.push_back (fl_rgb_color(randomNumber (0, 255),
                                          randomNumber (0, 255),
                                          randomNumber (0, 255)));
        }
    }
  
  return colours[index];
}

MainWindow:: MainWindow (const std::string& title, int argc, char ** argv, APB::Driver * driver)
: Fl_Double_Window (0, 0, 620, 300, title.c_str()),
  _selectedPortButton (0),
  _selectedClientButton (0),
  _driver (driver)
{
  /* this is so that the window manager can set the window where it wants */
  free_position();

  /* set the minimum window size */
  size_range (300, 100);


  /* this is for double buffering stuff, apparently */
  Fl::visual(FL_DOUBLE|FL_INDEX);

  _refreshButton = new Fl_Button (0, 0, getButtonWidth(), getButtonHeight(), "Refresh");
  _refreshButton->callback (&MainWindow::refreshPressed, (void *) this);

  _quitButton = new Fl_Button (getButtonWidth() + 1, 0, getButtonWidth(), getButtonHeight(), "Quit");
  _quitButton->callback (&MainWindow::quitPressed, (void *) this);

  _jfdButton = new Fl_Check_Button (getButtonWidth()*2 + 1, 0, getButtonWidth(), getButtonHeight(), "JFD");
  /* set it on by default */
  _jfdButton->type (FL_TOGGLE_BUTTON);
  _jfdButton->value (1);
  _jfdButton->callback (&MainWindow::jfdToggled, (void *) this);

  _log = new Fl_Multiline_Output (0, h() - getLogHeight(), w(), getLogHeight());

  _scrollPane = new Fl_Scroll (0, getButtonHeight(), w(), h() - getButtonHeight() - getLogHeight());
  _scrollPane->box (FL_DOWN_BOX);
  _scrollPane->type (Fl_Scroll::VERTICAL_ALWAYS);
  
  int packWidth = (w()/7) * 3;
  _readPack = new Fl_Pack (0, 0, packWidth, getButtonHeight() * 5);
  _readPack->end ();
  _writePack = new Fl_Pack (_scrollPane->w() - packWidth - 18, 0, packWidth, getButtonHeight() * 5);
  _writePack->end ();

  _scrollPane->end ();

  end ();

  refreshButtons ();
  refreshSubscriptions ();

  /* make sure the pixmap creation doesn't segfault */
  fl_open_display();

  /* set the icon */
  Pixmap p = XCreateBitmapFromData(fl_display, DefaultRootWindow(fl_display),
                                   five_pin_din_bits, five_pin_din_width, five_pin_din_height);
  icon((char *)p);

  show (argc, argv);

  Fl::add_idle (&MainWindow::idleCallback, this);
}

void
MainWindow:: log (const std::string& message)
{
  std::cout << message << std::endl;
  std::string wholeLog (message + "\n" + _log->value());
  _log->value (wholeLog.c_str ());
}


void
MainWindow:: idleCallback ()
{
#ifdef HAVE_LADCCA
    cca_event_t * event;
    static int do_while = 1;

    if (do_while)
      while ( (event = cca_get_event (_driver->getCCAClient())) )
	{
	  switch (cca_event_get_type (event))
	    {
	    case CCA_Quit:
	      delete this;
	      break;
	    case CCA_Server_Lost:
	      log ("LADCCA server disconnected");
	      do_while = 0;
	      break;
	    default:
	      std::cerr << "Recieved unknown LADCCA event of type "
			<< cca_event_get_type (event)
			<< std::endl;
	      break;
	    }
          
	  cca_event_destroy (event);
	}
#endif /* HAVE_LADCCA */

  readRefresh ();
}

void
MainWindow:: readRefresh ()
{
  char refresh;
  ssize_t err;

  usleep (100);
  
  err = read (_driver->getRefreshReadFile(), &refresh, sizeof (refresh));
  if (err == -1)
    {
      if (errno == EAGAIN) /* no data */
        return;

      log (std::string ("Error reading from refresh pipe, disabling refresh callback: ") + strerror (errno));
      return;
    }
  
  if (err != sizeof (refresh))
    {
      log ("Couldn't read entire char from refresh pipe (err.. this should, like, never ever happen)");
      return;
    }
  
  if (!refresh)
    {
      log ("The refresh thread exited; disabling refresh callback");
      return;
    }
  
  /* finally do the refresh */
  refreshPressed ();
}

/**********************************
 ********** ChoiceDialog ********** 
 **********************************/

ChoiceDialog:: ChoiceDialog (const std::string& message, const ChoiceSpec& choiceSpec)
: Fl_Window (100, BUTTON_HEIGHT * (choiceSpec._choices.size() + 1)),
  _done (false),
  _onlyone (choiceSpec._type == ONLYONE)
{
  int y = 0;
  Fl_Button * button;
  
  set_modal ();
  
  for (std::list<Choice>::const_iterator i = choiceSpec._choices.begin();
       i != choiceSpec._choices.end();
       ++i)
    {
      if (i->_takesInt)
        {
        }
      else
        {
          if (_onlyone)
            {
              button = new Fl_Round_Button (0, y, w(), BUTTON_HEIGHT, i->_choice.c_str());
              button->type (FL_RADIO_BUTTON);
              if (i->_value)
                button->setonly();
            }
          else
            {
              button = new Fl_Check_Button (0, y, w(), BUTTON_HEIGHT, i->_choice.c_str());
              button->type (FL_TOGGLE_BUTTON);
              if (i->_value)
                button->set();
            }
        }
      
      _buttons.push_back (button);
      
      y += BUTTON_HEIGHT;
    }
    
  button = new Fl_Button (0, y, w(), BUTTON_HEIGHT, "OK");
  button->callback (&ChoiceDialog::okPressed, (void *) this);
  
  end ();
  
  show ();
}

std::vector<ChoiceResult>
ChoiceDialog:: run ()
{
  while (!_done)
    Fl::wait();
  
  std::vector<ChoiceResult> results;
  
  for (std::list<Fl_Button *>::iterator i = _buttons.begin();
       i != _buttons.end();
       ++i)
    {
      results.push_back (std::pair<bool, int> ((*i)->value(), (*i)->value()));
    }
    
  return results;
}

/****************************
 ********** UI **************
 ****************************/

UI:: UI (const std::string& title, int argc, char ** argv, APB::Driver * driver)
{
  _mainWindow = new MainWindow (title, argc, argv, driver);
}

void
UI:: run ()
{
  Fl::run ();
}

void
UI:: log (const std::string& message)
{
  _mainWindow->log (message);
}

std::vector<ChoiceResult>
UI:: getChoice (std::string message, const ChoiceSpec& choiceSpec)
{
  ChoiceDialog choiceDialog (message, choiceSpec);
  return choiceDialog.run ();
}

} /* namespace FLTK */

class FLTKPlugin : public UIPlugin
{
    virtual const char * getUIName     ();
    virtual UI *       getUI           (const std::string& title, int * argc, char *** argv, Driver * driver);
};

const char *
FLTKPlugin:: getUIName ()
{
  return "FLTK";
}

UI *
FLTKPlugin:: getUI (const std::string& title, int * argc, char *** argv, Driver * driver)
{
  return new FLTK::UI (title, *argc, *argv, driver);
}

} /* namespace APB */

APB::UIPlugin *
getUIPlugin ()
{
  return new APB::FLTKPlugin ();
}

/* EOF */

