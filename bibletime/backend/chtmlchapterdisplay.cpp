/***************************************************************************
                          chtmlchapterdisplay.cpp  -  description
                             -------------------
    begin                : Thu Oct 12 2000
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

//BibleTiem includes
#include "chtmlchapterdisplay.h"
#include "cswordmoduleinfo.h"
#include "cswordversekey.h"
#include "../frontend/ctoolclass.h"
#include "creferencemanager.h"
#include "../frontend/optionsdialog/coptionsdialog.h"

//Qt includes
#include <qfont.h>

//Sword includes
#include <versekey.h>

/** Renders the text and puts the result into the member variable m_htmlText */
char CHTMLChapterDisplay::Display( CSwordModuleInfo* module ){
	if (!module) {
		m_htmlText = QString::null;
		return -1; //error
	}
	CSwordVerseKey key(module);
	key.key( module->module()->KeyText() );
	
	const int currentBook = key.Book();
	const int currentChapter = key.Chapter();
	const int currentVerse = key.Verse();	
	int verse = 0;
	
	if (module->encoding() == QFont::Unicode) {
		m_htmlHeader = "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"></head>";
	}
	m_htmlText = m_htmlHeader + QString("<body>");//dir=\"%1\">").arg((module->getTextDirection() == CSwordModuleInfo::RTL) ? "rtl" : "ltr");
	
	//reload font settings
	updateSettings();

	m_htmlText.append(QString("<font face=\"%1\" size=\"%2\" color=\"%3\">")
		.arg((module->encoding() == QFont::Unicode)? m_unicodeFontName : m_standardFontName)
		.arg((module->encoding() == QFont::Unicode)? m_unicodeFontSize : m_standardFontSize)
		.arg(m_standardFontColorName)
	);

	for (key.Verse(1); key.Book() == currentBook && key.Chapter() == currentChapter && !module->module()->Error(); key.NextVerse()) {
		verse = key.Verse();
		if (m_displayOptionsBool.verseNumbers)
#warning make color configurable
			m_htmlText.append( QString::fromLatin1("<a name=\"%1\" href=\"%2\"><b>%3</b></a>")
				.arg(verse)
				.arg(CReferenceManager::encodeHyperlink( module->name(), key.key() ))
				.arg(verse)
			);
		if (verse == currentVerse)
		  m_htmlText.append( QString("<font color=\"")+m_highlightedVerseColorName+QString("\">") );
		m_htmlText.append(key.renderedText());
		if (verse == currentVerse)
		  m_htmlText.append( QString("</font>") );

		if (m_displayOptionsBool.lineBreaks)
			m_htmlText.append("<br>\n");
	}
	m_htmlText.append("</font>");
	m_htmlText.append(m_htmlBody);	

	return 1;	//no error	
}

/** Generates code to display the given modules side by side. */
char CHTMLChapterDisplay::Display( QList<CSwordModuleInfo>* moduleList){	
#warning make table colors configurable
	qDebug("CHTMLChapterDisplay::Display( QList<CSwordModuleInfo>* moduleList)");
	if (!moduleList || (moduleList && !moduleList->count()) ) {
		m_htmlText = QString::null;
		return 0;
	}

	//reload font settings
	updateSettings();

//	QMap<CSwordModuleInfo*, QFont> fontMap;	
	SWModule* module = moduleList->first()->module();		
		
	VerseKey* vk = (VerseKey*)(SWKey*)*module;
	CSwordVerseKey key(moduleList->first());
	key.key((const char*)*vk);

	const int currentBook = key.Book();
	const int currentChapter = key.Chapter();
	const int chosenVerse = key.Verse();
	const int width=(int)((double)97/(double)moduleList->count());
	CSwordModuleInfo *d = 0;
			
	m_htmlText=m_htmlHeader+QString("<body>");

	m_htmlText.append(
		QString("<table cellpadding=\"2\" cellspacing=\"0\"><td bgcolor=\"#f1f1f1\"></td>"));

	m_htmlText.append(QString("<font face=\"%1\" size=\"%2\" color=\"%3\">")
		.arg(m_standardFontName).arg(m_standardFontSize).arg(m_standardFontColorName));

	
	SWModule *m = (d = moduleList->first()) ? d->module() : 0;

	while (m) {
    if (m)
			m_htmlText +=
				QString::fromLatin1("<td width=\"%1\" bgcolor=\"#f1f1f1\"><b>%1</b></td>").arg((int)((double)100/(double)moduleList->count())).arg(d->name());
		m = (d=moduleList->next()) ? d->module() : 0;			
	}
	m_htmlText.append("</tr>");
		
	QString rowText   = QString::null;
	int currentVerse = 0;
	for (key.Verse(1); key.Book() == currentBook && key.Chapter() == currentChapter && !module->Error(); key.NextVerse()) {
		const QString currentKey = key.key();
		currentVerse = key.Verse();
		rowText = QString("<tr><td bgcolor=\"#f1f1f1\"><b><a name=\"%1\" href=\"sword://%2\">%3</a></b></td>\n")
			.arg(currentVerse)
			.arg(currentKey)
			.arg(currentVerse);
		m = (d = moduleList->first()) ? d->module() : 0;
		while (m) {
			CSwordVerseKey current(d);
			current.key(currentKey);
			rowText += QString("<td width=\"%1\" bgcolor=\"%2\">")
				.arg(width).arg(currentVerse % 2 ? "white" : "#f1f1f1");
			if (d->encoding()==QFont::Unicode)
				rowText += QString("<font face=\"%1\" size=\"%2\">")
					.arg(m_unicodeFontName).arg(m_unicodeFontSize);
			if (currentVerse == chosenVerse)
				rowText += QString("<font color=\"%1\">")
					.arg(m_highlightedVerseColorName);

			rowText += current.renderedText();

			if (currentVerse == chosenVerse)
				rowText += QString("</font>");
			if (d->encoding()==QFont::Unicode)
				rowText += QString("</font");

			m = (d = moduleList->next()) ? d->module() : 0;
		}
		m_htmlText.append(rowText + QString::fromLatin1("</tr>\n"));
	}
	m_htmlText.append(QString("</font>"));
	m_htmlText.append(QString("</table>"));
	m_htmlText.append(m_htmlBody);

//	qWarning(m_htmlText.utf8());
	
	//clean up
	return 1;		
}
