/***************************************************************************
                          ccommentarypresenter.cpp  -  description
                             -------------------
    begin                : Sun Mar 18 2001
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

#include "ccommentarypresenter.h"
#include "cmodulechooserbar.h"
#include "../chtmlwidget.h"
#include "../keychooser/ckeychooser.h"
#include "../../ressource.h"
#include "../../whatsthisdef.h"
#include "../../backend/sword_backend/cswordversekey.h"
#include "../../backend/sword_backend/chtmlchapterdisplay.h"

//KDE includes
#include <ktoolbar.h>
#include <kaction.h>
#include <klocale.h>

CCommentaryPresenter::CCommentaryPresenter(ListCSwordModuleInfo useModules, CImportantClasses* importantClasses,QWidget *parent, const char *name )
	: CSwordPresenter(useModules, importantClasses, parent,name) {
	m_editToolBar = 0;	
	m_key = new CSwordVerseKey(m_moduleList.first());
	m_key->setKey("Genesis 1:1");
	
	initView();
	show();
		
	initConnections();
	
	lookup(m_key);
}

CCommentaryPresenter::~CCommentaryPresenter(){
}

/** Initializes our view. */
void CCommentaryPresenter::initView(){
	ASSERT(m_moduleList.first());
	
	m_mainToolBar = new KToolBar(this);
	m_keyChooser = CKeyChooser::createInstance(m_moduleList.first(), m_key, m_mainToolBar);
	m_mainToolBar->insertWidget(0,m_keyChooser->sizeHint().width(),m_keyChooser);	
	addToolBar(m_mainToolBar);			
	
	m_moduleChooserBar = new CModuleChooserBar(m_important, m_moduleList, CSwordModuleInfo::Commentary, this );
	addToolBar(m_moduleChooserBar);
	
	presenterEdit_action =  new KToggleAction(i18n("Edit entry..."), "pencil",
															IDK_PRESENTER_EDIT, this,	SLOT(editComment()), actionCollection(), "editComment_action");
	presenterEdit_action->setEnabled( m_moduleList.first()->module()->isWritable() );
	presenterEdit_action->setWhatsThis( WT_PRESENTER_EDIT );
	presenterEdit_action->plug(m_mainToolBar);
	
	m_htmlWidget = new CHTMLWidget(this);
		
	//setup popup menu
	m_popup = new KPopupMenu(this);
	m_popup->insertTitle(i18n("Bible presenter"));
	m_popup->insertItem(i18n("Save chapter as HTML..."), m_htmlWidget, SLOT(slotSaveAsHTML()), 0,ID_PRESENTER_SAVE_AS_HTML);	
	m_popup->insertItem(i18n("Save chapter as plain text..."), m_htmlWidget, SLOT(slotSaveAsText()),0,ID_PRESENTER_SAVE_AS_TEXT);
	m_popup->insertSeparator();
	m_popup->insertItem(i18n("Select all"), m_htmlWidget, SLOT(slotSelectAll()),0, ID_PRESENTER_SELECT_ALL);
	m_popup->insertItem(i18n("Copy selected text"), m_htmlWidget, SLOT(copy()),0,ID_PRESENTER_COPY_SELECTED);	
	m_popup->insertItem(i18n("Copy chapter into clipboard"), m_htmlWidget, SLOT(copyDocument()),0,ID_PRESENTER_COPY_ALL);
	m_popup->insertSeparator();		
  m_popup->insertItem(i18n("Lookup word in lexicon"), m_lexiconPopup, ID_PRESENTER_LOOKUP );	
//	m_popup->insertSeparator();		
//	m_popup->insertItem(i18n("Add verse to print queue"), this, SLOT(printHighlightedVerse()));	
	m_htmlWidget->installPopup(m_popup);		
	m_htmlWidget->installAnchorMenu( m_popup );
		
	setCentralWidget(m_htmlWidget);	
	setIcon( COMMENTARY_ICON_SMALL );
}

/** Initializes the connections */
void CCommentaryPresenter::initConnections(){
	connect(m_htmlWidget, SIGNAL(referenceClicked(const QString&)),
		this, SLOT(referenceClicked(const QString&))); 	 	
	connect(m_htmlWidget, SIGNAL(sigDeleteDocument()),
		this, SLOT(deleteText())); 	 	
	connect(m_htmlWidget, SIGNAL(sigSaveDocument(const QString)),
		this, SLOT(saveText(const QString))); 	 	 	
 	connect( m_keyChooser, SIGNAL(keyChanged(CKey*)),
 		this, SLOT(lookup(CKey*)));		
	connect(m_popup,SIGNAL(aboutToShow()),
		SLOT(popupAboutToShow()));
	connect(m_moduleChooserBar, SIGNAL( sigChanged() ),
		SLOT( modulesChanged() ));		
}

/** Is called when the selected modules changed. */
void CCommentaryPresenter::modulesChanged(){
  qDebug("CBiblePresenter::modulesChanged()");
  m_moduleList = m_moduleChooserBar->getModuleList();

  if (!m_moduleList.count())
  	close();
  else {
		presenterEdit_action->setEnabled( m_moduleList.first()->module()->isWritable() );
	  refreshFeatures();	
	  m_key->setModule(m_moduleList.first());
	  m_keyChooser->setModule(m_moduleList.first());	
	  lookup(m_key);
	}
}

/** renders text and set it to the HTML widget */
void CCommentaryPresenter::lookup(CKey* key){
	setUpdatesEnabled(false);	
	
	CSwordVerseKey* vKey = dynamic_cast<CSwordVerseKey*>(key);	
	if (!vKey)
		return;
  m_moduleList.first()->module()->SetKey(*vKey);
		
	if (m_moduleList.first()->getDisplay()) {	//do we have a display object?
		if (m_htmlWidget->isReadOnly())	 {
			if (m_moduleChooserBar->getModuleList().count()>1)  //we want to display more than one module
				m_moduleList.first()->getDisplay()->Display( &m_moduleList );
			else
				m_moduleList.first()->getDisplay()->Display( m_moduleList.first() );
	 		 m_htmlWidget->setText( m_moduleList.first()->getDisplay()->getHTML() );		
		}
		else {
	 		 m_htmlWidget->setText( QString::fromLocal8Bit(m_moduleList.first()->module()->getRawEntry()) );					
		}
	}
	
	if (m_key != vKey) {
		ASSERT(m_key);
		ASSERT(vKey);
		m_key->setKey(QString::fromLocal8Bit((const char*)*vKey));
	}
	m_htmlWidget->scrollToAnchor( QString::number(vKey->Verse()) );
	setCaption( QString::fromLocal8Bit((const char*)*m_key) );
	
	setUpdatesEnabled(true);		
}

/** No descriptions */
void CCommentaryPresenter::popupAboutToShow(){
	m_popup->setItemEnabled(ID_PRESENTER_COPY_SELECTED, m_htmlWidget->hasSelectedText());	
	m_popup->setItemEnabled(ID_PRESENTER_LOOKUP, m_htmlWidget->hasSelectedText());
}

/** No descriptions */
void CCommentaryPresenter::referenceClicked(const QString&){
}

/** Saves the given text in the module. */
void CCommentaryPresenter::saveText(const QString text){
	m_moduleList.first()->module()->SetKey(*m_key);
	(*(m_moduleList.first()->module())) << (const char*)text.local8Bit();
	
	lookup(m_key);	//update current key so the saved text will be displayed
}

/** Deletes the displayed and edited text. */
void CCommentaryPresenter::deleteText(){
	m_moduleList.first()->module()->deleteEntry();
	
	lookup(m_key);
}

void CCommentaryPresenter::editComment(){
	qDebug("CCommentaryPresenter::editComment()");	
	m_htmlWidget->setReadOnly( !m_htmlWidget->isReadOnly() );	
	if (!m_htmlWidget->isReadOnly() && !m_editToolBar) {
		m_editToolBar = new KToolBar(this);
		addToolBar(m_editToolBar);
		
		m_htmlWidget->createEditToolbar( m_editToolBar );
	}	
	ASSERT(m_editToolBar);
	if (!m_htmlWidget->isReadOnly() && !m_editToolBar)
		return;
		
	if (!m_htmlWidget->isReadOnly())
		m_editToolBar->show();
	else
		m_editToolBar->hide();
			
	lookup( m_key );	
	m_htmlWidget->setFocus();
}
