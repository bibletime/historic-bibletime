/***************************************************************************
                          ccommentaryreadwindow.h  -  description
                             -------------------
    begin                : Don Mai 9 2002
    copyright            : (C) 2002-2005 by The BibleTime team
    email                : info@bibletime.info
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CCOMMENTARYREADWINDOW_H
#define CCOMMENTARYREADWINDOW_H

//BibleTime includes
#include "cbiblereadwindow.h"

//Qt includes
#include <qwidget.h>

/**
  *@author The BibleTime team
  */

class CCommentaryReadWindow : public CLexiconReadWindow  {
   Q_OBJECT
public:
  /**
  * Reimplementation.
  */
	static void insertKeyboardActions( KActionCollection* const a );

  CCommentaryReadWindow(ListCSwordModuleInfo modules, CMDIArea* parent, const char *name=0);
  /**
  * Store the settings of this window in the given CProfileWindow object.
  */
  virtual void storeProfileSettings( CProfileWindow* profileWindow );
  /**
  * Store the settings of this window in the given profile window.
  */
  virtual void applyProfileSettings( CProfileWindow* profileWindow );
	virtual const bool syncAllowed() const;

public slots: // Public slots
  void nextBook();
  void previousBook();
  void nextChapter();
  void previousChapter();
  void nextVerse();
  void previousVerse();
  /**
  * Reimplementation to handle the keychooser refresh.
  */
  virtual void reload();

protected:
	virtual void initActions();
  virtual void initToolbars();

private:
  KToggleAction* m_syncButton;
  CSwordVerseKey* verseKey();
};

#endif
