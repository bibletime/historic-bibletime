/***************************************************************************
                          cmodulechooserbutton.h  -  description
                             -------------------
    begin                : Sat Mar 17 2001
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

#ifndef CMODULECHOOSERBUTTON_H
#define CMODULECHOOSERBUTTON_H

#include "backend/cswordmoduleinfo.h"
#include "frontend/cpointers.h"

#include <ktoolbarbutton.h>

class KPopupMenu;

/**
  *@author The BibleTime team
  */

class CModuleChooserButton : public KToolBarButton, public CPointers  {
   Q_OBJECT
public: 
	CModuleChooserButton(CSwordModuleInfo* useModule,CSwordModuleInfo::ModuleType type, const int id, QWidget *parent = 0, const char *name = 0 );
  CSwordModuleInfo* module();
  /**
  * Returns the id used for this button.
  */
  int getId() const;
	
private:
  /**
  * Returns the icon used for the current status.
  */
  const QString iconName();		
	
	bool m_hasModule;
	CSwordModuleInfo::ModuleType m_moduleType;
	CSwordModuleInfo* m_module;
	int m_id;
	KPopupMenu* m_popup;

private slots:
	void moduleChosen(int ID );

signals:
	void sigRemoveButton(const int ID);
	void sigAddButton();
  void sigChanged();
};
#endif