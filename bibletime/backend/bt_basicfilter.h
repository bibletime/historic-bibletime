/***************************************************************************
                          bt_basicfilter.h  -  description
                             -------------------
    begin                : Thu Nov 1 2001
    copyright            : (C) 2001 by The BibleTime team
    email                : info@bibletime.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BT_BASICFILTER_H
#define BT_BASICFILTER_H

#include <swbasicfilter.h>

/**
  * This is the base class for all filters in BT
  */

class BT_BASICFILTER : public SWBasicFilter  {
public: 
	BT_BASICFILTER();
  virtual char ProcessText (char *text, int maxlen = -1);

protected:
	/**
	* updates the filter specific settings like colors
	* the variables are used in the inherited filters
	* in HandleToken()
	*/
	void updateSettings(void);

	char footnote_color[8];
	char strongs_color[8];
	char morph_color[8];
	char jesuswords_color[8];
};

#endif
