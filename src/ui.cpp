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

#include "ui.h"

namespace APB {

Choice:: Choice (const std::string& choice,
                 bool takesInt,
                 int defaultValue)
: _choice (choice),
  _takesInt (takesInt),
  _value (defaultValue)
{
}

Choice:: Choice (const Choice& choice)
: _choice (choice._choice),
  _takesInt (choice._takesInt),
  _value (choice._value)
{
}


ChoiceSpec:: ChoiceSpec (std::string message, ChoiceType type)
: _message (message),
  _type (type)
{
}

void
ChoiceSpec:: addChoice (const Choice& choice)
{
  _choices.push_back (choice);
}

} /* namespace APB */

