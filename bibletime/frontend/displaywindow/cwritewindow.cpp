/***************************************************************************
                          cwritewindow.cpp  -  description
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

#include "cwritewindow.h"

#include "backend/cswordmoduleinfo.h"

#include "frontend/keychooser/ckeychooser.h"


CWriteWindow::CWriteWindow(ListCSwordModuleInfo modules, CMDIArea* parent, const char *name )
  : CDisplayWindow(modules, parent,name), m_writeDisplay(0) {
}

CWriteWindow::~CWriteWindow(){
}


void CWriteWindow::insertKeyboardActions( KAccel* const a ) {

};

const bool CWriteWindow::init( const QString& keyName ) {
  qWarning("CWriteWindow::init( const QString& keyName )");
  CDisplayWindow::init(keyName);
// 	setupPopupMenu();
  keyChooser()->setKey(key());
	setReady(true);
  return true;
};

void CWriteWindow::storeProfileSettings(CProfileWindow * const settings) {

};

void CWriteWindow::applyProfileSettings(CProfileWindow * const settings) {

};

/** Sets the write display-widget for this write display window. */
void CWriteWindow::setDisplayWidget( CWriteDisplay* display ){
  m_writeDisplay = display;
}

/** Look up the given key and display the text. In our case we offer to edit the text. */
void CWriteWindow::lookup( CSwordKey* newKey ){
  //set the raw text to the display widget
	if (!newKey)
		return;

//	if (CEntryDisplay* display = modules().first()->getDisplay()) {	//do we have a display object?
// 		displayWidget()->setText( display->text( modules(), newKey->key(), displayOptions(), filterOptions() ) );
//	}
  if (CSwordModuleInfo* module = modules().first()) {
    displayWidget()->setText( newKey->rawText() );
  }
  
	if (key() != newKey)
		key()->key(newKey->key());

	setCaption( windowCaption() );
  
}

/** Saves the given text as text of the given key. Use this function as backend in each write window implementation. */
const bool CWriteWindow::saveText( CSwordKey* newKey, const QString newText ){
//  qWarning("CWriteWindow::saveText: %s", newText.latin1());
//  newKey->key( newKey->key() ); //set the key to the module
//  sword::SWModule* m = modules().first()->module();
//  m->KeyText( newKey->key().latin1() );
//
//  qWarning("used key: %s",newKey->key().latin1());
//
//  if (m->isWritable())  {
//    (*m) << newText.latin1();
//  };
  modules().first()->write(newKey, newText);
}

/** Returns the write display widget used by this window. */
CWriteDisplay* const CWriteWindow::displayWidget(){
  return m_writeDisplay;
}

void CWriteWindow::initConnections() {

};

void CWriteWindow::initKeyboardActions() {

};
