/***************************************************************************
                          chtmlwritewindow.cpp  -  description
                             -------------------
    begin                : Fre Nov 29 2002
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

#include "chtmlwritewindow.h"

//frontend includes
#include "frontend/keychooser/ckeychooser.h"
#include "frontend/cresmgr.h"

//Qt includes

//KDE includes
#include <kaction.h>
#include <klocale.h>


CHTMLWriteWindow::CHTMLWriteWindow(ListCSwordModuleInfo modules, CMDIArea* parent, const char *name)
  : CPlainWriteWindow(modules, parent, name)
{
  qWarning("CHTMLWriteDisdplayWindow");
//  setKey( CSwordKey::createInstance(moduleList.first()) );    
}

CHTMLWriteWindow::~CHTMLWriteWindow(){
  
}

//void CHTMLWriteWindow::setupPopupMenu() {
//
//};

void CHTMLWriteWindow::initView() {
  qWarning("CHTMLWriteWindow::initView()");
  CWriteDisplay* writeDisplay = CDisplay::createWriteInstance(this, CDisplay::HTMLDisplay);
  Q_ASSERT(writeDisplay);
 	setDisplayWidget( writeDisplay );
 	setCentralWidget( displayWidget()->view() );

  setMainToolBar( new KToolBar(this) );
  mainToolBar()->setFullSize(true);
	addDockWindow(mainToolBar());

	setKeyChooser( CKeyChooser::createInstance(modules(), key(), mainToolBar()) );
	mainToolBar()->insertWidget(0,keyChooser()->sizeHint().width(),keyChooser());
 	mainToolBar()->setFullSize(false);


  //setip the toolbar
 	m_actions.saveText = new KAction( i18n("Save the text"),
    CResMgr::displaywindows::writewindow::saveText::icon,
    CResMgr::displaywindows::writewindow::saveText::accel,
    this, SLOT( saveCurrentText()  ),
    actionCollection()
  );
  m_actions.saveText->setToolTip( CResMgr::displaywindows::writewindow::saveText::tooltip );
  m_actions.saveText->setWhatsThis( CResMgr::displaywindows::writewindow::saveText::whatsthis );
  m_actions.saveText->plug(mainToolBar());  

 	m_actions.deleteEntry = new KAction(i18n("Delete the current entry"),
    CResMgr::displaywindows::writewindow::deleteEntry::icon,
    CResMgr::displaywindows::writewindow::deleteEntry::accel,
    this, SLOT(deleteEntry()),
    actionCollection()
  );
  m_actions.deleteEntry->setToolTip( CResMgr::displaywindows::writewindow::deleteEntry::tooltip );
  m_actions.deleteEntry->setWhatsThis( CResMgr::displaywindows::writewindow::deleteEntry::whatsthis );
  m_actions.deleteEntry->plug(mainToolBar());


 	m_actions.restoreText = new KAction(i18n("Restore original text"),
    CResMgr::displaywindows::writewindow::restoreText::icon,
    CResMgr::displaywindows::writewindow::restoreText::accel,
    this, SLOT(restoreText()), actionCollection()
  );
  m_actions.restoreText->setToolTip( CResMgr::displaywindows::writewindow::restoreText::tooltip );
  m_actions.restoreText->setWhatsThis( CResMgr::displaywindows::writewindow::restoreText::whatsthis );
  m_actions.restoreText->plug(mainToolBar());
  

  KToolBar* bar = new KToolBar(this);
  bar->setFullSize(true);
	addDockWindow(bar);

  writeDisplay->setupToolbar( bar, actionCollection() );
};

void CHTMLWriteWindow::initConnections() {
 	connect(keyChooser(), SIGNAL(keyChanged(CSwordKey*)),
		this, SLOT(lookup(CSwordKey*)));
  connect(displayWidget()->connectionsProxy(), SIGNAL(textChanged()),
    this, SLOT(textChanged()) );
};

/** Is called when the current text was changed. */
void CHTMLWriteWindow::textChanged() {
  m_actions.saveText->setEnabled( displayWidget()->isModified() );
  m_actions.restoreText->setEnabled( displayWidget()->isModified() );
}

/** Loads the original text from the module. */
void CHTMLWriteWindow::restoreText(){
  lookup(key());
  displayWidget()->setModified(false);
  textChanged();
}
