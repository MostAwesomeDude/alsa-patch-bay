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

#include "choice.h"

namespace APB {
namespace GTKmm {


////////////////////////////////
//////// ChoiceWidget //////////
////////////////////////////////

Gtk::RadioButton::Group ChoiceWidget::_group;

void
ChoiceWidget:: setInput ()
{
  if (_spinButton)
    _spinButton->set_sensitive (_toggleButton->get_active());
}

ChoiceWidget:: ChoiceWidget (ChoiceType type, const Choice& choice)
  : _spinButtonAdj (choice._value, -10000.0, 10000.0)
{
  if (type == ANY)
    {
      _toggleButton = manage (new Gtk::CheckButton (choice._choice));
    }
  else
    {
      _toggleButton = manage (new Gtk::RadioButton (_group, choice._choice));
    }
  pack_start (*_toggleButton);
  
  if (choice._takesInt)
    {
      _spinButton = manage (new Gtk::SpinButton (_spinButtonAdj));
      _spinButton->set_numeric ();
      this->setInput ();
      _toggleButton->signal_toggled().connect (SigC::slot (*this, &ChoiceWidget::setInput));
      pack_end (*_spinButton);
    }
  else
    _spinButton = 0;

  show_all ();
}


const ChoiceResult&
ChoiceWidget:: getResult () const
{
  static ChoiceResult result;
  
  result.first = _toggleButton->get_active ();
  
  if (_spinButton)
    result.second = _spinButton->get_value_as_int ();
  else
    result.second = 0;
  
  return result;
}

ChoiceWindow::  ChoiceWindow (const std::string& title, const ChoiceSpec& choiceSpec)
: Gtk::Dialog (title, true)
{
  Gtk::VBox * vbox = get_vbox ();
  
  Gtk::Label * message = manage (new Gtk::Label (choiceSpec._message));
  vbox->pack_start (*message);
  
  for (std::list<Choice>::const_iterator it = choiceSpec._choices.begin();
       it != choiceSpec._choices.end ();
       ++it)
    {
      ChoiceWidget * choiceWidget = manage (new ChoiceWidget (choiceSpec._type, *it));
      _choiceWidgets.push_back (choiceWidget);
      vbox->pack_start (*choiceWidget);
    }
  
  Gtk::Button * okButton = manage (new Gtk::Button(Gtk::StockID(GTK_STOCK_OK)));
  add_action_widget (*okButton, 0);
  
  show_all ();
}

std::vector<ChoiceResult>
ChoiceWindow:: getChoice ()
{
  run ();

  std::vector<ChoiceResult> results;
  for (std::list<ChoiceWidget *>::iterator it = _choiceWidgets.begin ();
       it != _choiceWidgets.end ();
       ++it)
    results.push_back ((*it)->getResult ());

  return results;
}

} /* namespace GTKmm */
} /* namespace APB */

/* EOF */

