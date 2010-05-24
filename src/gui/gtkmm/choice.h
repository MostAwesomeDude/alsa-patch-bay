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

#ifndef __APB_GTKMM_CHOICE_H__
#define __APB_GTKMM_CHOICE_H__

#include <gtkmm-2.0/gtkmm.h>

#include "ui.h"

namespace APB {
namespace GTKmm {

class ChoiceWidget : public Gtk::HBox
{
  private:
    static Gtk::RadioButton::Group _group;
    Gtk::ToggleButton *            _toggleButton;
    Gtk::SpinButton *              _spinButton;
    Gtk::Adjustment                _spinButtonAdj;
    
    void setInput ();
  
  public:
                        ChoiceWidget (ChoiceType, const Choice&);
    const ChoiceResult& getResult () const;
};

class ChoiceWindow : public Gtk::Dialog
{
  private:
    std::list<ChoiceWidget *> _choiceWidgets;
    
  public:
                              ChoiceWindow (const std::string&, const ChoiceSpec&);
    std::vector<ChoiceResult> getChoice ();
};
 
} /* namespace GTKmm */
} /* namespace APB */

#endif /* __APB_GTKMM_CHOICE_H__ */

