/***************************************************************************
                          chtmlentrydisplay.cpp  -  description
                             -------------------
    begin                : Wed Oct 11 2000
    copyright            : (C) 2000 by The BibleTime team
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

//BibleTime includes
#include "chtmlentrydisplay.h"
#include "cswordmoduleinfo.h"
#include "cswordkey.h"
#include "cswordldkey.h"
#include "cswordversekey.h"
#include "../frontend/ctoolclass.h"

//Qt includes
#include <qfont.h>

//Sword includes
#include <versekey.h>

CHTMLEntryDisplay::CHTMLEntryDisplay(){
	m_highlightedVerseColor = "red";
	m_htmlHeader = "<HTML><HEAD></HEAD>";
	m_htmlBody = "</BODY></HTML>";
	m_standardFontName = QFont::defaultFont().family();
	m_standardFontSize = 4; //we use logical font sizes between 1 and 7
	m_includeHeader = true;
}

CHTMLEntryDisplay::~CHTMLEntryDisplay(){
}

/** Displays the current entry of the module as HTML */
char CHTMLEntryDisplay::Display(CSwordModuleInfo* module) {
	qDebug("CHTMLEntryDisplay::Display(CSwordModuleInfo* module)");
	if (!module) {
		m_htmlText = QString::null;
		return -1;
	}
	QString FontName = m_standardFontName;
  int FontSize = m_standardFontSize;
	
  CSwordKey* key = 0;
  if (module->getType() == CSwordModuleInfo::Commentary || module->getType() == CSwordModuleInfo::Bible)
		key = new CSwordVerseKey(module);
  else if (module->getType() == CSwordModuleInfo::Lexicon)
		key = new CSwordLDKey(module);
	key->key(module->module()->KeyText());
	
  if (module->hasFont()){ //use custom font
    QFont font = module->getFont();
    FontName = font.family();
    FontSize = CToolClass::makeLogicFontSize(font.pointSize());
  }
	if (m_includeHeader) {
		m_htmlText =
			m_htmlHeader + QString::fromLatin1("<BODY><FONT color=\"%1\">\
<A HREF=\"sword://%2\">%3: <B>%4</B></A></FONT>\
<HR><FONT face=\"%5\" size=\"%6\">%7</FONT>")
				.arg(m_highlightedVerseColor)
				.arg(key->key())
				.arg(module->getDescription())
				.arg(key->key())
				.arg(FontName)
				.arg(FontSize)
				.arg(key->renderedText())
			+ m_htmlBody;
	}
	else
		m_htmlText = key->renderedText();

//	delete key;
	return 1;
}

/** Generates code to display the given modules side by side. */
char CHTMLEntryDisplay::Display( QList<CSwordModuleInfo>* moduleList) {
	qDebug("CHTMLEntryDisplay::Display( QList<CSwordModuleInfo>* moduleList)");
	if (!moduleList || (moduleList && !moduleList->count()) ) {
		m_htmlText = QString::null;
		return 0;
	}
 	QString FontName = m_standardFontName;
 	int FontSize = m_standardFontSize;

 	CSwordKey* key = 0;
 	if (moduleList->first()->getType() == CSwordModuleInfo::Commentary || moduleList->first()->getType() == CSwordModuleInfo::Bible)
		key = new CSwordVerseKey(moduleList->first());
	else if (moduleList->first()->getType() == CSwordModuleInfo::Lexicon)
		key = new CSwordLDKey(moduleList->first());
	  	
	SWModule* module = moduleList->first()->module();
	QString usedKey = QString::null;
	CSwordModuleInfo *d = 0;	
	
	SWModule *m= (d = moduleList->first()) ? d->module() : 0;		
	if (moduleList->first()->getType() == CSwordModuleInfo::Commentary || moduleList->first()->getType() == CSwordModuleInfo::Bible) {
		VerseKey* vk = (VerseKey*)(SWKey*)(*module);
//		vk->Persist(1);
		
		key->key((const char*)*vk);
		usedKey = key->key();
		VerseKey k(*vk);
		
		m = (d = moduleList->first()) ? d->module() : 0;	
		while (m) {
	   	m = (d=moduleList->next()) ? d->module() : 0;
	   	if (m)
				m->SetKey( (const char*)k );
		}	
	}
	else { //lexicon
		SWKey* lk = (SWKey*)(*module);
//		lk->Persist(1);
		key->key((const char*)*lk);
		usedKey = key->key();
				
		m = (d = moduleList->first()) ? d->module() : 0;	
		while (m) {
	   	m = (d=moduleList->next()) ? d->module() : 0;
	   	if (m){
				m->SetKey( (const char*)lk );
				(const char*)*m; //snap to entry
			}
		}		
	}
	
	const int width=(int)((double)100/(double)moduleList->count());
	m_htmlText = m_htmlHeader + QString::fromLatin1("<TABLE cellpadding=\"2\" cellspacing=\"0\"><TR>"); 	
	
	m = (d = moduleList->first()) ? d->module() : 0;		
	while (m) {
		key->module(d);
    if (m)
    	m_htmlText.append(
				QString::fromLatin1("<TD width=\"%1\" bgcolor=\"#F1F1F1\"><B>%2 (<FONT COLOR=\"%3\">%4</FONT>)</B></TD>")
					.arg(width)
					.arg(QString::fromLocal8Bit(m->Name()))
					.arg(m_highlightedVerseColor)
					.arg(key->key())
			);
		m = (d=moduleList->next()) ? d->module() : 0;			
	}
	m_htmlText.append(QString::fromLatin1("</TR>"));
			
	m = (d = moduleList->first()) ? d->module() : 0;
	m_htmlText.append(QString::fromLatin1("<TR>"));
	while (m) {
		if (d && d->hasFont()) { //use custom font
			QFont font = d->getFont();
			FontName = font.family();
			FontSize = CToolClass::makeLogicFontSize(font.pointSize());
		}
		else {
			FontName = m_standardFontName;
			FontSize = m_standardFontSize;
		}
		key->module(d);
		key->key(usedKey);
		
		m_htmlText +=
			QString::fromLatin1("<TD width=\"%1%\"><FONT SIZE=\"%2\" FACE=\"%3\">%4</FONT></TD>")
				.arg(width)
				.arg(FontSize)
				.arg(FontName)
				.arg(key->renderedText());
		m = (d = moduleList->next()) ? d->module() : 0;		
	}
	m_htmlText += QString::fromLatin1("</TR></TABLE>") + m_htmlBody;

//	delete key;	
	return 1;
}