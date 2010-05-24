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

#ifndef __APB_UI_H__
#define __APB_UI_H__

#include <list>
#include <string>
#include <vector>
#include <map>

namespace APB {

typedef enum { ONLYONE, ANY } ChoiceType;

struct Choice
{
  std::string _choice;
  bool        _takesInt;
  int         _value;
  
  Choice (const std::string& choice, bool takesInt = false, int defaultValue = 0);
  Choice (const Choice& choice);
};

struct ChoiceSpec {
  std::string       _message;
  ChoiceType        _type;
  std::list<Choice> _choices;
  
       ChoiceSpec (std::string message, ChoiceType type);
  void addChoice  (const Choice& choice);
};

typedef std::pair< bool, int > ChoiceResult; // (selected, value)

class UI
{
  public:
    virtual void log (const std::string&) = 0;
    virtual void run () = 0;
    virtual std::vector<ChoiceResult> getChoice (std::string, const ChoiceSpec&) = 0;
};

} /* namespace APB */

#endif /* __APB_UI_H__ */
