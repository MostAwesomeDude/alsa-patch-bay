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

#ifndef __APB_FLTK_H__
#define __APB_FLTK_H__

#include <string>
#include <list>

#include <FL/Fl.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Multiline_Output.H>

#include "addr.h"
#include "subscription.h"
#include "ui.h"

#define BUTTON_HEIGHT 24

namespace APB {
namespace FLTK {

typedef void (*Callback) (Fl_Widget *, void *);

class PortButton : public Fl_Button
{
  private:
    Addr * _addr;
    std::string _label;
    int _index;
  public:
    PortButton (int x, int y, int w, int h, Addr * addr, int index = 0)
    : Fl_Button (x, y, w, h, addr->getName().c_str()),
      _addr (addr),
      _label (addr->getName().c_str()),
      _index (index)
    {
      label (_label.c_str());
    }
    Addr * getAddr () { return _addr; }
    const Addr * getAddr () const { return _addr; }
    int getIndex () const { return _index; }
};

class MainWindow : public Fl_Double_Window
{
  private:
    Fl_Button * _quitButton;
    Fl_Button * _refreshButton;
    Fl_Check_Button * _jfdButton;
    Fl_Scroll * _scrollPane;
    Fl_Multiline_Output * _log;
    Fl_Pack * _readPack;
    Fl_Pack * _writePack;
    std::list<PortButton *> _readButtons;
    std::list<PortButton *> _writeButtons;
    std::list<const Subscription *> _subscriptions;
    PortButton * _selectedPortButton;
    PortButton * _selectedClientButton;
    
    APB::Driver * _driver;
    
    static void quitPressed (Fl_Widget * widget, void * data)
    {
      ((MainWindow *) data)->quitPressed ();
    }
    void quitPressed ();
    
    static void refreshPressed (Fl_Widget * widget, void * data)
    {
      ((MainWindow *) data)->refreshPressed ();
    }
    void refreshPressed ();
    
    static void jfdToggled (Fl_Widget * widget, void * data)
    {
      ((MainWindow *) data)->jfdToggled (((Fl_Button *)widget)->value ());
    }
    void jfdToggled (bool on);
    
    static void readPressed (Fl_Widget * widget, void * data)
    {
      ((MainWindow *) data)->readPressed ((PortButton *) widget);
    }
    void readPressed (PortButton *);

    static void writePressed (Fl_Widget * widget, void * data)
    {
      ((MainWindow *) data)->writePressed ((PortButton *) widget);
    }
    void writePressed (PortButton *);
    
    static void idleCallback (void * data)
    {
      ((MainWindow *) data)->idleCallback ();
    }
    void idleCallback ();
    void readRefresh ();
    
    PortButton * findPortButton (std::list<PortButton *>&, const Addr *);
    PortButton * findReadButton (const Addr * addr);
    PortButton * findWriteButton (const Addr * addr);
    
    void setSelectedPortButton (PortButton * portButton);
    void setSelectedClientButton (PortButton * portButton);
    void unsetSelectedButtons ();
    void unsubscribePorts (PortButton * portButton);
    void refreshButtonPack (std::list<PortButton *>& portButtons,
                            const std::list<Addr *>& addresses,
                            Fl_Pack * pack,
                            Callback callback);
    virtual void resize (int x, int y, int w, int h);
    Fl_Color getColour (unsigned int);
  
    int getButtonHeight () { return BUTTON_HEIGHT; }
    int getButtonWidth () { return 70; }
    int getLogHeight () { return h() / 8; }
  protected:
    virtual void draw();
  public:
    MainWindow (const std::string& title, int, char **, APB::Driver * driver);
    void log (const std::string&);
    
    void refreshButtons ();
    void refreshSubscriptions ();
};

class ChoiceDialog : public Fl_Window
{
  private:
    bool _done;
    bool _onlyone;
    std::list<Fl_Button *> _buttons;
    
    static void okPressed (Fl_Widget * widget, void * data)
    {
      ((ChoiceDialog *) data)->okPressed ();
    }
    void okPressed () { _done = true; }
    
  public:
    ChoiceDialog (const std::string&, const ChoiceSpec&);
    std::vector<ChoiceResult> run ();
};

class UI : public APB::UI
{
  private:
    MainWindow * _mainWindow;
  public:
    UI (const std::string& title, int, char **, Driver * driver);
    virtual void run ();
    virtual void log (const std::string&);
    virtual std::vector<ChoiceResult> getChoice (std::string, const ChoiceSpec&);
};

} /* namespace FLTK */
} /* namespace APB */

#endif /* __APB_FLTK_H__ */

