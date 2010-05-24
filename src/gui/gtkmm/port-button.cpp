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

#include "port-button.h"
#include "addr.h"
#include "button-array.h"

namespace APB {
namespace GTKmm {


void
PortButton:: on_clicked ()
{
  if (_read)
    _buttonArray->readButtonClicked (this);
  else
    _buttonArray->writeButtonClicked (this);
}

PortButton:: PortButton (const Addr * addr, ButtonArray * buttonArray,
                         bool read, guint index)
: Gtk::Button (addr->getName()),
  _addr (addr),
  _buttonArray (buttonArray),
  _read (read),
  _index (index)
{
}

void
PortButton:: addr (const Addr * addr)
{
  _addr = addr;
}

const Addr *
PortButton:: addr () const
{
  return _addr;
}

void
PortButton:: index (guint index)
{
  _index = index;
}

guint
PortButton:: index () const
{
  return _index;
}


} /* namespace GTKmm */
} /* namespace APB */

/* EOF */

