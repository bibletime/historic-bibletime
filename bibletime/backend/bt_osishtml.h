/***************************************************************************
                          thmlhtml.h  -  description
                             -------------------
    begin                : 1999-10-28

    copyright            : 1999 by Chris Little
    email                : chrislit@chiasma.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef OSISHTML_H
#define OSISHTML_H

#include "bt_basicfilter.h"

/**
* This filter converts ThML text to HTML text
*/
class BT_OSISHTML : public BT_BASICFILTER {

public:
  BT_OSISHTML ();
  virtual bool handleToken(sword::SWBuf& buf, const char *token, DualStringMap &userData);
//  virtual char ProcessText (char *text, int maxlen = -1);
};

#endif