/***************************************************************************
                          cexportmanager.cpp  -  description
                             -------------------
    begin                : Mon Feb 25 2002
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

#include "cexportmanager.h"

#include "backend/cswordkey.h"
#include "backend/cswordmoduleinfo.h"
#include "backend/creferencemanager.h"
#include "backend/cswordversekey.h"

#include "frontend/ctoolclass.h"

#include "printing/cprintitem.h"
#include "printing/cprinter.h"

#include "util/scoped_resource.h"

//Qt includes
#include <qprogressdialog.h>
#include <qclipboard.h>
#include <qlist.h>

//KDE includes
#include <kfiledialog.h>
#include <klocale.h>
#include <kapplication.h>

//Sword includes
#include <swkey.h>
#include <listkey.h>

/** Saves the key to disk. */
const bool CExportManager::saveKey( CSwordKey* key, const bool withText, const CSwordBackend::FilterOptions filterOptions, const CSwordBackend::DisplayOptions displayOptions) {
  const QString file = KFileDialog::getSaveFileName(QString::null, i18n("*.txt | Text files\n *.* | All files (*.*)"), 0, i18n("Save search result ..."));
  QString text = QString::null;
  if (!key || file.isEmpty())
    return false;

  if (!withText) {
    text = key ? key->key() : QString::null;
  	return true;
  }
  else {
    CPointers::backend()->setFilterOptions(filterOptions);
    CPointers::backend()->setDisplayOptions(displayOptions);

    CSwordModuleInfo* module = key->module();
    Q_ASSERT(module);
  	if (CSwordVerseKey* vk = dynamic_cast<CSwordVerseKey*>(key) ) {
      CSwordVerseKey startKey(module);
      CSwordVerseKey stopKey(module);

  		startKey.key(vk->LowerBound());
  		stopKey.key(vk->UpperBound());
  	
  		while ( startKey < stopKey || startKey == stopKey ) {
  			text += ((bool)displayOptions.verseNumbers ? QString::fromLatin1("%1 ").arg(startKey.Verse()) : QString::null)
+ startKey.strippedText() + ((bool)displayOptions.lineBreaks ? QString::fromLatin1("\n") : QString::null);
  			
        startKey.next(CSwordVerseKey::UseVerse);
  		}
  	}		
    else {
      text = key->strippedText();
    }
    text += "\n" + QString::fromLatin1("(%1, %1)").arg(key->key()).arg(module->name());
  }	

	CToolClass::savePlainFile(file, text);		
	return true;
}

/** Saves the key to disk. */
const bool CExportManager::saveKeyList( ListKey* list, CSwordModuleInfo* module, const QString& label, const bool withText, const bool showProgress ){
	bool ret = false;
	const QString file = KFileDialog::getSaveFileName(QString::null, i18n("*.txt | Text files\n *.* | All files (*.*)"), 0, i18n("Save search result ..."));
	if (!file.isEmpty()) {
		QProgressDialog progress( label, i18n("Cancel"), list->Count(), 0,"progress", true );	
		progress.setProgress(0);	
		progress.setMinimumDuration(10);	
		progress.show();
	
		util::scoped_ptr<CSwordKey> key(CSwordKey::createInstance(module));
	
		QString text;	
		int index = 0;
		*list = TOP;
		while (!list->Error() && !progress.wasCancelled()) {
			key->key((const char*)(*list));
			if (!key)
				break;
			progress.setProgress(index++);
			KApplication::kApplication()->processEvents(10); //do not lock the GUI!																		
		
			if (withText)
				text += QString::fromLatin1("%1:\n\t%2\n").arg( key->key() ).arg( key->strippedText() );
			else
				text += key->key() + "\n";

			(*list)++;
		}	
		if (!progress.wasCancelled()) {
			progress.setProgress(index);	
			CToolClass::savePlainFile(file, text);
			ret = true;
		}
	}
	
	return ret;
}

const bool CExportManager::saveKeyList( QPtrList<CSwordKey>& list, CSwordModuleInfo* module, const QString& label, const bool withText, const bool showProgress ){
	bool ret = false;
	const QString file = KFileDialog::getSaveFileName(QString::null, i18n("*.txt | Text files\n *.* | All files (*.*)"), 0, i18n("Save search result ..."));	
	if (!file.isEmpty()){
	
		QProgressDialog progress( label, i18n("Cancel"), list.count(), 0,"progress", true );	
		progress.setProgress(0);	
		progress.setMinimumDuration(10);	
		progress.show();

		QString text;	
		CSwordKey* key = 0;
		for (list.first(); list.current(); list.next()) {
			key = list.current();
			if (!key)
				break;
			progress.setProgress(list.at());
			KApplication::kApplication()->processEvents(10); //do not lock the GUI!
			
			if (withText)
				text += QString::fromLatin1("%1:\n\t%2\n").arg( key->key() ).arg( key->strippedText() );
			else
				text += key->key() + "\n";
		}
		
		if (!progress.wasCancelled()) {
			progress.setProgress(list.count());	
			CToolClass::savePlainFile(file, text);
		}
	}
	
	return ret;
}


///////// copy functions
const bool CExportManager::copyKey( CSwordKey* key, const bool withText, const CSwordBackend::FilterOptions filterOptions, const CSwordBackend::DisplayOptions displayOptions) {
  /*
  * This function copies the text of key inton the clipboard. The keyname is appended in brackets.
  */
  if (!key)
    return false;

  if (!withText) {
  	KApplication::clipboard()->setText(key->key());
  	return true;
  }

  CPointers::backend()->setFilterOptions(filterOptions);
  CPointers::backend()->setDisplayOptions(displayOptions);

  QString text = QString::null;
  CSwordModuleInfo* module = key->module();
  Q_ASSERT(module);
	if (CSwordVerseKey* vk = dynamic_cast<CSwordVerseKey*>(key) ) {
    CSwordVerseKey startKey(module);
    CSwordVerseKey stopKey(module);

		startKey.key(vk->LowerBound());
		stopKey.key(vk->UpperBound());
	
		while ( startKey < stopKey || startKey == stopKey ) {
			text += ((bool)displayOptions.verseNumbers ? QString::fromLatin1("%1 ").arg(startKey.Verse()) : QString::null)
+ startKey.strippedText() + ((bool)displayOptions.lineBreaks ? QString::fromLatin1("\n") : QString::null);
			
      startKey.next(CSwordVerseKey::UseVerse);
		}
	}		
  else {
    text = key->strippedText();
  }

  text += "\n" + QString::fromLatin1("(%1, %1)").arg(key->key()).arg(module->name());
	KApplication::clipboard()->setText(text);
	return true;
}

const bool CExportManager::copyKeyList( ListKey* list, CSwordModuleInfo* module, const QString& label, const bool withText, const bool showProgress ){
	QProgressDialog progress( label, i18n("Cancel"), list->Count(), 0,"progress", true );	
	progress.setProgress(0);	
	progress.setMinimumDuration(10);	
	progress.show();
	
	QString text;	
	int index = 0;	
	util::scoped_ptr<CSwordKey> key( CSwordKey::createInstance(module) );
	*list = TOP;
	while (!list->Error() && !progress.wasCancelled()) {
		key->key((const char*)(*list));
		if (!key)
			break;
		progress.setProgress(index++);
		KApplication::kApplication()->processEvents(10); //do not lock the GUI!																		
		
		text += (withText) ? QString::fromLatin1("%1:\n\t%2\n").arg(key->key()).arg(key->strippedText()) : key->key()+"\n";
		(*list)++;
	}
	if (progress.wasCancelled())
		return false;
		
	progress.setProgress(index);	
	KApplication::clipboard()->setText(text);		
	return true;
}


//////// printing functions //////////

const bool CExportManager::printKey( CSwordModuleInfo* module, const QString& startKey, const QString& stopKey, const QString& description ){
	printer()->appendItem( new CPrintItem(module, startKey, stopKey, description) );	
	return true;
}

const bool CExportManager::printKeyList( ListKey* list, CSwordModuleInfo* module, const QString& label, const bool showProgress ){
	QProgressDialog progress( label, i18n("Cancel"), list->Count(), 0,"progress", true );	
	progress.setProgress(0);	
	progress.setMinimumDuration(10);	
	progress.show();
	
	int index = 0;	
	QPtrList<CPrintItem> itemList;
	QString startKey, stopKey;
	
	*list = TOP;
	while (!list->Error() && !progress.wasCancelled()) {
		VerseKey* vk = dynamic_cast<VerseKey*>(list);
		if (vk) {
			startKey = QString::fromLocal8Bit((const char*)(vk->LowerBound()) );
			stopKey = QString::fromLocal8Bit((const char*)(vk->UpperBound()) );
		}
		else {
			startKey = QString::fromLocal8Bit((const char*)*list);
			stopKey = QString::null;
		}
		itemList.append( new CPrintItem(module, startKey, stopKey) );
		
		progress.setProgress(index++);
		KApplication::kApplication()->processEvents(10); //do not lock the GUI!
				
		(*list)++;
	}
	
	//add all items to the queue
	if (progress.wasCancelled()) {
		itemList.setAutoDelete(true);	
		itemList.clear();//delete all items		
		return false;
	}
	
	progress.setProgress(list->Count());	
	printer()->appendItems(itemList);	
	
	return true;
}

/** Prints a key using the hyperlink created by CReferenceManager. */
void CExportManager::printKey( const QString& hyperlink ){
 	QString moduleName;
  QString keyName;
  CReferenceManager::Type type;

  CReferenceManager::decodeHyperlink(hyperlink, moduleName, keyName, type);
  if (moduleName.isEmpty()) {
  	moduleName = CReferenceManager::preferredModule(type);
	}

 	if (CSwordModuleInfo* module = backend()->findModuleByName(moduleName)) {
    qWarning(keyName.latin1());
    QString startKey = keyName;
    QString stopKey = keyName;

    //check if we have a range of entries or a single one
    if (module->type() == CSwordModuleInfo::Bible || module->type() == CSwordModuleInfo::Commentary) {
      ListKey verses = VerseKey().ParseVerseList((const char*)keyName.local8Bit(), "Genesis 1:1", true);
    	for (int i = 0; i < verses.Count(); ++i) {
    		VerseKey* element = dynamic_cast<VerseKey*>(verses.GetElement(i));
    		if (element)
        	CExportManager::printKey(module,QString::fromLocal8Bit((const char*)element->LowerBound()), QString::fromLocal8Bit((const char*)element->UpperBound()) );
    		else
					CExportManager::printKey(module,(const char*)*verses.GetElement(i),(const char*)*verses.GetElement(i));
    	}
		}
  	else {
			CExportManager::printKey(module,keyName,keyName);
    }
	}
}
