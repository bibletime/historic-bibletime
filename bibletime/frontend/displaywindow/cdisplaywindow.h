/***************************************************************************
                          cdisplaywindow.h  -  description
                             -------------------
    begin                : Don Mai 9 2002
    copyright            : (C) 2002 by The BibleTime team
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

#ifndef CDISPLAYWINDOW_H
#define CDISPLAYWINDOW_H

//BibleTime includes
#include "frontend/cpointers.h"
#include "backend/cswordmoduleinfo.h"
#include "backend/cswordbackend.h"
#include "backend/cswordkey.h"
#include "backend/cswordversekey.h"
#include "backend/cswordtreekey.h"
#include "backend/cswordldkey.h"

//Qt includes
#include <qwidget.h>

//KDE includes
#include <kmainwindow.h>


class CMDIArea;
class CReadWindow;
class CWriteWindow;
class CMDIArea;
class CProfileWindow;
class KAccel;
class CDisplaySettingsButton;
class CKeyChooser;
class CModuleChooserBar;
class CDisplaySettingsButton;
class KToolBar;

/** The base class for all display windows of BibleTime.
  * @author The BibleTime team
  */

class CDisplayWindow : public KMainWindow, public CPointers  {
   Q_OBJECT
public:
	static CReadWindow* createReadInstance(ListCSwordModuleInfo modules, CMDIArea* parent, const char* name = 0);
	static CWriteWindow* createWriteInstance(ListCSwordModuleInfo modules, CMDIArea* parent, const char* name = 0);
  /**
  * Insert the keyboard accelerators of this window into the given KAccel object.
  */
  static void insertKeyboardActions( KAccel* const accel );

 	CMDIArea* const mdi();
  /**
  * Returns the right window caption.
  */
  const QString windowCaption();
  /**
  * Returns the used modules as a QPtrList
  */
  ListCSwordModuleInfo& modules();
  /**
  * Store the settings of this window in the given CProfileWindow object.
  */
  virtual void storeProfileSettings( CProfileWindow* profileWindow ) = 0;
  /**
  * Store the settings of this window in the given profile window.
  */
  virtual void applyProfileSettings( CProfileWindow* profileWindow ) = 0;
  /**
  * Set the window caption.
  */
  virtual void setCaption( const QString& );
  /**
  * Refresh the settings of this window.
  */
  virtual void refresh();
  /**
  * Sets the new filter options of this window.
  */
  void setFilterOptions( CSwordBackend::FilterOptionsBool filterOptions );
  /**
  * Sets the new display options for this window.
  */
  void setDisplayOptions( const CSwordBackend::DisplayOptionsBool displayOptions );
  /**
  * Returns the display options used by this display window.
  */
  CSwordBackend::DisplayOptionsBool& displayOptions();
  /**
  * Returns the filter options used by this window.
  */
  CSwordBackend::FilterOptionsBool& filterOptions();
  /**
  * Set the ready status
  */
  void setReady( const bool ready );
  /**
  * Returns true if the widget is ready for use.
  */
  const bool isReady() const;
  /**
  * Returns true if the window may be closed.
  */
  virtual bool queryClose();
  /**
  * Returns the keychooser widget of this display window.
  */
  CKeyChooser* const keyChooser();
  /**
  * Sets the new sword key.
  */
  void setKey( CSwordKey* key );
  /**
  * Returns the key of this display window.
  */
  CSwordKey* key();
  /**
  * Initialize the window. Call this method from the outside, because calling this in the constructor is not possible!
  */
  virtual const bool init( const QString& key );
  /**
  * Sets the main toolbar.
  */
  void setMainToolBar( KToolBar* bar );
  /**
  * Returns the main toolbar.
  */
  KToolBar* const mainToolBar() const;
  /**
  * Returns the display settings button
  */
  CDisplaySettingsButton* const displaySettingsButton() const;
  /**
  * Sets the display settings button.
  */
  void setDisplaySettingsButton( CDisplaySettingsButton* button );

public slots:
  /**
  * Lookup the specified key in the given module. If the module is not chosen withing
  * this display window create a new displaywindow with the right module in it.
  */
  virtual void lookup( const QString& module, const QString& key );
  /**
  * Lookup the key in the chosen modules.
  */
  virtual void lookup( const QString& key );

protected:
	friend class CMDIArea;
	CDisplayWindow(ListCSwordModuleInfo modules, CMDIArea* parent, const char *name=0);
	virtual ~CDisplayWindow();
  /**
  * Initializes the intern keyboard actions.
  */
  virtual void initKeyboardActions() = 0;
  /**
   Is called when this window gets the focus or looses the focus.
  */
  virtual void windowActivated( const bool hasFocus );
  /**
  * Sets the keychooser widget for this display window.
  */
  void setKeyChooser( CKeyChooser* ck );
  /**
  * Returns the module chooser bar.
  */
  CModuleChooserBar* const moduleChooserBar() const;
  /**
  * Lookup the given key.
  */
  virtual void lookup( CSwordKey* );
  /**
  * Sets the module chooser bar.
  */
  void setModuleChooserBar( CModuleChooserBar* bar );
  /**
  * Sets the modules.
  */
  void setModules( const ListCSwordModuleInfo modules );
  /**
  * Initializes the signal / slot connections of this display window.
  */
  virtual void initConnections() = 0;
  /**
  * Initialize the view of this display window.
  */
  virtual void initView() = 0;

protected slots:
	void modulesChanged();
  /**
  * Lookup the current key. Used to refresh the display.
  */
  void lookup();
  /**
  * Reimplementation from QWidget. Used to initialize things before the widget is shown.
  */
  virtual void polish();

private:
	CMDIArea* m_mdi;
	ListCSwordModuleInfo m_modules;
  CSwordBackend::FilterOptionsBool m_filterOptions;
	CSwordBackend::DisplayOptionsBool m_displayOptions;
  CDisplaySettingsButton* m_displaySettingsButton;
  CKeyChooser* m_keyChooser;
  CSwordKey* m_swordKey;
  bool m_isReady;
  CModuleChooserBar* m_moduleChooserBar;
  KToolBar* m_mainToolBar;
};

#endif