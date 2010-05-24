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

#include "subscription.h"
#include "apb.h"
#include "addr.h"

namespace APB {

Subscription:: Subscription (Addr * from, Addr * to)
: _from (from),
  _to (to)
{
}

Subscription:: Subscription (const Subscription& sub)
: _from (sub._from),
  _to   (sub._to)
{
}

Subscription:: ~Subscription ()
{
}

const Addr *
Subscription:: from () const
{
  return _from;
}

const Addr *
Subscription:: to () const
{
  return _to;
}

std::string
Subscription:: getName () const
{
  return std::string("'") + _from->getName() + "' -> '" + _to->getName()+ "'";
}

bool
Subscription:: operator == (const Subscription& sub) const
{
  if (!(sub.to()->equals (_to) && sub.from()->equals (_from)))
      return false;

  if (_attributes.size () != sub._attributes.size ())
    return false;

  std::map<std::string, int>::const_iterator this_it;
  std::map<std::string, int>::const_iterator that_it;
  for (this_it = _attributes.begin(),
       that_it = sub._attributes.begin();
       this_it != _attributes.end();
       ++this_it, ++that_it)
    {
      if (this_it->first != that_it->first ||
          this_it->second != this_it->second)
        {
          return false;
        }
    }
  
  return true;
}

const std::map<std::string, int>&
Subscription:: getAttributes () const
{
  return _attributes;
}

int
Subscription:: getAttribute (const std::string& attr) const
{
  return _attributes.find(attr)->second;
}

void
Subscription:: setAttribute (const std::string& attr, int num)
{
  _attributes[attr] = num;
}


} /* namespace APB */
