/***************************************************************************
                   csearchdialogresultview.cpp  -  description
                             -------------------
    begin                : Wed Jan 19 2000
    copyright            : (C) 2000 by The BibleTime Team
    email                : Info@bibletime.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//own includes
#include "csearchdialogresultview.h"
#include "csearchdialog.h"
#include "../../structdef.h"
#include "../../ressource.h"
#include "../../whatsthisdef.h"
#include "../../tooltipdef.h"
#include "../../backend/cmoduleinfo.h"
#include "../../backend/sword_backend/cswordbiblemoduleinfo.h"
#include "../../backend/sword_backend/cswordlexiconmoduleinfo.h"
#include "../../backend/sword_backend/cswordversekey.h"
#include "../../backend/sword_backend/cswordldkey.h"
#include "../ctoolclass.h"
#include "../../printing/cprintitem.h"
#include "../../printing/cprinter.h"

//QT includes
#include <qlist.h>
#include <qheader.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qdragobject.h>
#include <qwidget.h>
#include <qclipboard.h>
#include <qprogressdialog.h>
#include <qwhatsthis.h>
#include <qtooltip.h>

//KDE includes
#include <kfiledialog.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kiconloader.h>
#include <kconfig.h>
#include <kapp.h>
#include <kpopupmenu.h>
#include <kglobalsettings.h>
#include <kaction.h>

//Sword includes
#include <listkey.h>
#include <swmodule.h>

CSearchDialogResultModuleView::CSearchDialogResultModuleView(CImportantClasses* importantClasses,QWidget *parent, const char *name)
	: QListView ( parent, name ) {
	m_important = importantClasses;
	
	initView();
	adjustSize();
}

CSearchDialogResultModuleView::~CSearchDialogResultModuleView() {

}

/** Initializes the tree of this ResultView */
void CSearchDialogResultModuleView::setupTree() {
	ListKey moduleSearchResult;
	QListViewItem	*module = 0;

	for (moduleList->first(); moduleList->current(); moduleList->next()) {
		moduleSearchResult = moduleList->current()->getSearchResult();
		module = new QListViewItem(this, QString("%1 [%2]").arg(QString::fromLocal8Bit(moduleList->current()->module()->Name())).arg( moduleSearchResult.Count() ));
		module->setPixmap(0,CToolClass::getIconForModule(moduleList->current()) );
	}	
	setFixedWidth( sizeHint().width() );
//	resize(sizeHint());
}

void CSearchDialogResultModuleView::viewportMousePressEvent(QMouseEvent *e) {
	qDebug("CSearchDialogResultModuleView::viewportMousePressEvent");
	QListView::viewportMousePressEvent(e);

	m_currentItem = (QListViewItem*)itemAt(e->pos());
	if (!m_currentItem)
	{
		qDebug("item is NULL");
		return;
	}
	if ((e->state() & ControlButton) || (e->state() & ShiftButton))
			return;
	for (moduleList->first();moduleList->current();moduleList->next()){
		QString modName = m_currentItem->text(0);
		modName = modName.left( modName.find(" [") );
		m_currentModule = m_important->swordBackend->findModuleByName(modName);
		if (m_currentModule)
			emit moduleSelected(m_currentModule);
	}	
	if (e->button() == RightButton)
		m_popup->popup( mapToGlobal(e->pos()) );
}

/** Initializes this widget */
void CSearchDialogResultModuleView::initView(){
	QWhatsThis::add(this, WT_SD_RESULT_MODULE_VIEW);
	
	setFocusPolicy(QWidget::WheelFocus);
	setAllColumnsShowFocus( true );
	setRootIsDecorated( false );
	addColumn( i18n( "Modules" ), -1 );
	header()->hide();
	setSorting( -1 );
	
	m_popup = new KPopupMenu(this);
	
	m_copyPopup = new KPopupMenu(m_popup);	
	m_copyPopup->insertItem(i18n("Search result"), this, SLOT(slotCopySearchResult()));	
	m_copyPopup->insertItem(i18n("Search result with text"), this, SLOT(slotCopySearchResultWithKeytext()));
	
	m_printPopup = new KPopupMenu(m_popup);		
	m_printPopup->insertItem(i18n("Search result"), this, SLOT(printSearchResult()));
	
	m_savePopup = new KPopupMenu(m_popup);		
	m_savePopup->insertItem(i18n("Search result"), this, SLOT(slotSaveSearchResult()));
	m_savePopup->insertItem(i18n("Result with text"), this, SLOT(slotSaveSearchResultWithKeytext()));
	
	m_popup->insertItem(SmallIcon(ICON_EDIT_COPY), i18n("Copy..."), 	m_copyPopup);
	m_popup->insertItem(SmallIcon(ICON_FILE_PRINT), i18n("Add to printing queue..."), m_printPopup);
	m_popup->insertItem(SmallIcon(ICON_FILE_SAVE), i18n("Save..."), 	m_savePopup);
}

/** Returns the module belonging to the selected item. */
CSwordModuleInfo* CSearchDialogResultModuleView::getCurrentModule() {
	return m_currentModule;
}

/** Reimplementation. Sets the cortrect QHeader width. */
void CSearchDialogResultModuleView::resizeEvent( QResizeEvent* e){
	QListView::resizeEvent(e);	
	header()->resize( visibleWidth(), header()->height() );
}

/** Adds all items  of the search result of this module to the printing queue of BibleTime. */
void CSearchDialogResultModuleView::printSearchResult(){
	ListKey& searchResult = m_currentModule->getSearchResult();
		
	QProgressDialog progress( "Printing search result...", i18n("Cancel"), searchResult.Count(), this, "progress", true );	
	progress.setProgress(0);
	
	const int count = searchResult.Count();
	QString text;
	const CSwordModuleInfo::type type = m_currentModule->getType();
	for (int index = 0; index < count; ++index) {
		progress.setProgress(index);
		KApplication::kApplication()->processEvents(10); //do not lock the GUI!				
		if ( progress.wasCancelled() )
			return;
		SWKey* key = searchResult.GetElement(index);
		if (!key)
			break;
		text = QString::fromLocal8Bit( (const char*)*key );
		
		CPrintItem*	printItem = new CPrintItem();
		printItem->setModule(m_currentModule);		
		if (type == CSwordModuleInfo::Bible || type == CSwordModuleInfo::Commentary ) {	//a bible or a commentary
			CSwordVerseKey* verseKey = new CSwordVerseKey(m_currentModule); 	//the key is deleted by the CPrintItem
			verseKey->setKey(text);
			printItem->setStartKey(verseKey);
//			printItem->setStopKey(verseKey);
		}
		else if (type == CSwordModuleInfo::Lexicon) {	//a lexicon
			CSwordLDKey* ldKey = new CSwordLDKey(m_currentModule);	//the key is deleted by the CPrintItem
			ldKey->setKey(text);
			printItem->setStartKey(ldKey);
//			printItem->setStopKey(ldKey);	
		}
		m_important->printer->addItemToQueue( printItem );
	}
	progress.setProgress(searchResult.Count());		
}

/** This function copies the search result into the clipboard */
void CSearchDialogResultModuleView::slotCopySearchResult(){
	//get the searched text
	QString searchedText;
	for (QObject* w = parent(); w; w = w->parent()) {
		if (w->isA("CSearchDialog")) {
			CSearchDialog*	dlg = (CSearchDialog*)w;
			searchedText = dlg->getSearchedText().stripWhiteSpace();
			break;
		}		
	}

	ListKey& searchResult = m_currentModule->getSearchResult();
	QProgressDialog progress( "Copying search result to clipboard...", i18n("Cancel"), searchResult.Count(), this, "progress", true );	
	progress.setProgress(0);		
		
	QString text = i18n("Search result for \"%1\" in module \"%2\"\n").arg(searchedText).arg( QString::fromLocal8Bit(m_currentModule->module()->Name()) );
	text.append( QString("Entries found: %1\n\n").arg(searchResult.Count()) );	
	
	const int count = searchResult.Count();
	for (int index = 0; index < count; index++) {
		progress.setProgress(index);
		KApplication::kApplication()->processEvents(10); //do not lock the GUI!		
		SWKey* key = searchResult.GetElement(index);		
		if ( progress.wasCancelled())
			return;
		if (!key )
			break;
		text += QString("%1\n").arg(QString::fromLocal8Bit( (const char*)*key ));
	}
	progress.setProgress(searchResult.Count());
	KApplication::clipboard()->setText(text);	
}

/** This slot saves the search result to disk. */
void CSearchDialogResultModuleView::slotSaveSearchResult(){
	//get the searched text
	QString searchedText;
	for (QObject* w = parent(); w; w = w->parent()) {
		if (w->isA("CSearchDialog")) {
			CSearchDialog*	dlg = (CSearchDialog*)w;
			searchedText = dlg->getSearchedText().stripWhiteSpace();
			break;
		}		
	}
	const QString file = KFileDialog::getSaveFileName (QString::null, i18n("*.txt | Text files\n *.* | All files (*.*)"), 0, i18n("Save search result ..."));	
	if (file.isEmpty())
		return;
	
	ListKey& searchResult = m_currentModule->getSearchResult();
	QProgressDialog progress( "Saving search result...", i18n("Cancel"), searchResult.Count(), this, "progress", true );	
	progress.show();
	progress.setProgress(0);	
	QString text = i18n("Search result for \"%1\" in module \"%2\"\n").arg(searchedText).arg( QString::fromLocal8Bit(m_currentModule->module()->Name()) );
	text.append( QString("Entries found: %1\n\n").arg(searchResult.Count()) );	
	
	const int count = searchResult.Count();
	for (int index = 0; index < count; index++) {
		progress.setProgress(index);
		KApplication::kApplication()->processEvents(10); //do not lock the GUI!														
		if ( progress.wasCancelled() )
			return;		
		SWKey* key = searchResult.GetElement(index);
		if (!key)
			break;
		text += QString("%1\n").arg(QString::fromLocal8Bit( (const char*)*key ));
	}
	progress.setProgress(searchResult.Count());	
	CToolClass::savePlainFile( file, text);
}


void CSearchDialogResultModuleView::slotCopySearchResultWithKeytext(){
	//get the searched text
	QString searchedText;
	for (QObject* w = parent(); w; w = w->parent()) {
		if (w->isA("CSearchDialog")) {
			CSearchDialog*	dlg = (CSearchDialog*)w;
			searchedText = dlg->getSearchedText().stripWhiteSpace();
			break;
		}		
	}

	ListKey& searchResult = m_currentModule->getSearchResult();
	QProgressDialog progress( "Copying search result to clipboard...", i18n("Cancel"), searchResult.Count(), this, "progress", true );	
	progress.setProgress(0);		
	QString text = i18n("Search result for \"%1\" in module \"%2\"\n").arg(searchedText).arg( QString::fromLocal8Bit(m_currentModule->module()->Name()) );
	text.append( QString("Entries found: %1\n\n").arg(searchResult.Count()) );	
	const int count = searchResult.Count();
	const CSwordModuleInfo::type type = m_currentModule->getType();
	for (int index = 0; index < count; index++) {		
		progress.setProgress(index);
		KApplication::kApplication()->processEvents(10); //do not lock the GUI!									
		if ( progress.wasCancelled() )
			return;
		SWKey* key = searchResult.GetElement(index);
		if (!key)
			break;		
			
		if (type == CSwordModuleInfo::Bible || type == CSwordModuleInfo::Commentary) {
			CSwordVerseKey vKey(m_currentModule);
			vKey.setKey(QString::fromLocal8Bit((const char*)*key));
			text += QString("%1:\n\t%2\n").arg( vKey.getKey() ).arg( vKey.getRenderedText() );
		}
		else if (type == CSwordModuleInfo::Lexicon) {
			CSwordLDKey ldKey(m_currentModule);
			ldKey.setKey(QString::fromLocal8Bit((const char*)*key));
			text += QString("%1:\n\t%2\n").arg( ldKey.getKey() ).arg( ldKey.getRenderedText() );			
		}		
	}
	KApplication::clipboard()->setText(text);	
}

/** This slot saves the search result to disk. */
void CSearchDialogResultModuleView::slotSaveSearchResultWithKeytext(){
	//get the searched text
	QString searchedText;
	for (QObject* w = parent(); w; w = w->parent()) {
		if (w->isA("CSearchDialog")) {
			CSearchDialog*	dlg = (CSearchDialog*)w;
			searchedText = dlg->getSearchedText().stripWhiteSpace();
			break;
		}		
	}

	const QString file = KFileDialog::getSaveFileName (QString::null, i18n("*.txt | Text files\n *.* | All files (*.*)"), 0, i18n("Save search result ..."));		
	if (file.isEmpty())
		return;
	ListKey& searchResult = m_currentModule->getSearchResult();
	QProgressDialog progress( "Saving...", i18n("Cancel"), searchResult.Count(), this, "progress", true );	
	progress.setProgress(0);		
	QString text = i18n("Search result for \"%1\" in module \"%2\"\n").arg(searchedText).arg( QString::fromLocal8Bit(m_currentModule->module()->Name()) );
	text.append( QString("Entries found: %1\n\n").arg(searchResult.Count()) );	

	const int count = searchResult.Count();
	const CSwordModuleInfo::type type = m_currentModule->getType();
		
	for (int index = 0; index < count; index++) {		
		SWKey* key = searchResult.GetElement(index);
		progress.setProgress(index);
		if ( progress.wasCancelled() )
			return;		
		KApplication::kApplication()->processEvents(10); //do not lock the GUI!							
		if (!key)
			break;
		if (type == CSwordModuleInfo::Bible || type == CSwordModuleInfo::Commentary) {
			CSwordVerseKey vKey(m_currentModule);
			vKey.setKey(QString::fromLocal8Bit((const char*)*key));
			text += QString("%1:\n\t%2\n").arg( vKey.getKey() ).arg( vKey.getStrippedText() );			
		}
		else if (type == CSwordModuleInfo::Lexicon) {
			CSwordLDKey ldKey(m_currentModule);
			ldKey.setKey(QString::fromLocal8Bit((const char*)*key));
			text += QString("%1:\n\t%2\n").arg( ldKey.getKey() ).arg( ldKey.getStrippedText() );			
		}		
	}	
	progress.setProgress(searchResult.Count());
	CToolClass::savePlainFile( file, text);
}

//------------class CSearchDialofResultView-----------//
CSearchDialogResultView::CSearchDialogResultView(CImportantClasses* importantClasses, QWidget *parent, const char *name)
	: QListBox(parent,name) {
	m_important = importantClasses;
	m_currentItem = 0;
	m_module = 0;
	
	initView();
	initConnections();
}

CSearchDialogResultView::~CSearchDialogResultView() {
}

/** Initializes the tree of this ResultView */
void CSearchDialogResultView::setupTree() {
	ListKey& moduleSearchResult = m_module->getSearchResult();
	clear();
	const int count = moduleSearchResult.Count();
	
	setUpdatesEnabled(false);
	for (int index = 0; index < count; index++) {
		insertItem( QString::fromLocal8Bit((const char*)*moduleSearchResult.GetElement(index)), -1);
	}
	setUpdatesEnabled(true);	
}

/** Initializes the connections of this class */
void CSearchDialogResultView::initConnections() {
	connect(m_popup, SIGNAL(aboutToShow()),
		this, SLOT(popupAboutToShow()));
	connect(this, SIGNAL(currentChanged(QListBoxItem*)),
		this, SLOT(mousePressed(QListBoxItem*)));	
	connect(this, SIGNAL(rightButtonPressed(QListBoxItem*, const QPoint&)),
		this, SLOT(rightButtonPressed(QListBoxItem*, const QPoint&)));
}

/** Initializes this widget */
void CSearchDialogResultView::initView(){
	QWhatsThis::add(this, WT_SD_RESULT_RESULT_VIEW);
	setSelectionMode(QListBox::Extended);
	
	m_popup = new KPopupMenu(this);		
	m_copyPopup = new KPopupMenu(m_popup);	
	m_copyPopup->insertItem(i18n("Key"), this, SLOT(slotCopyCurrent()));	
	m_copyPopup->insertItem(i18n("Key with text"), this, SLOT(slotCopyCurrentWithKeytext()));
	
	m_printPopup = new KPopupMenu(m_popup);		
	m_printPopup->insertItem(i18n("Key with text"), this, SLOT(printItem()));
	
	m_savePopup = new KPopupMenu(m_popup);		
	m_savePopup->insertItem(i18n("Key"), this, SLOT(slotSaveCurrent()));
	m_savePopup->insertItem(i18n("Key with text"), this, SLOT(slotSaveCurrentWithKeytext()));
	
	m_popup->insertItem(SmallIcon(ICON_EDIT_COPY), i18n("Copy..."), m_copyPopup);
	m_popup->insertItem(SmallIcon(ICON_FILE_PRINT),i18n("Add to printing queue..."),m_printPopup);
	m_popup->insertItem(SmallIcon(ICON_FILE_SAVE), i18n("Save..."), m_savePopup);		
	
	setFocusPolicy(QWidget::WheelFocus);
}


/** Is called before the menu is shown */
void CSearchDialogResultView::popupAboutToShow(){
}

/**  */
void CSearchDialogResultView::viewportMouseMoveEvent(QMouseEvent *e){
  QListBox::viewportMouseMoveEvent(e);

	if ( !(e->state() & QMouseEvent::LeftButton) )
		return;
		
	//Is it time to start a drag?
 	if (abs(e->pos().x() - m_pressedPos.x()) > KGlobalSettings::dndEventDelay() ||
 		abs(e->pos().y() - m_pressedPos.y()) > KGlobalSettings::dndEventDelay() ){
 		qDebug("start a drag");
 		if (m_currentItem) {
 			QString mod;
 			QString ref;
 			ASSERT(m_currentItem);

 			mod = m_module->module()->Name();
 			ref = m_currentItem->text();

 			QTextDrag *d = new QTextDrag(CToolClass::encodeReference(mod,ref),this->viewport());
 			if (d){
 				d->setSubtype(REFERENCE);
 				d->setPixmap( REFERENCE_ICON_SMALL );
 				d->drag();
 			}
 		}
 	}
}

/**  */
void CSearchDialogResultView::printItem() {
	if (currentText().isEmpty())
		return;
	QList<QListBoxItem> list = selectedItems();
	for (list.first(); list.current(); list.next()) {
		CPrintItem*	printItem = new CPrintItem();
		if ( dynamic_cast<CSwordBibleModuleInfo*>(m_module) ) {	//a bible or a commentary
			CSwordVerseKey* verseKey = new CSwordVerseKey(m_module); 	//the key is deleted by the CPrintItem
			verseKey->setKey(currentText());
			printItem->setStartKey(verseKey);
			printItem->setStopKey(verseKey);
		}
		else if (dynamic_cast<CSwordLexiconModuleInfo*>(m_module)) {	//a lexicon
			CSwordLDKey* ldKey = new CSwordLDKey(m_module);	//the key is deleted by the CPrintItem
			ldKey->setKey(currentText());
			printItem->setStartKey(ldKey);
			printItem->setStopKey(ldKey);	
		}	
		printItem->setModule(m_module);		
		m_important->printer->addItemToQueue( printItem );
	}
}

/** This slot is called when the current item changed. */
void CSearchDialogResultView::itemChanged(QListBoxItem* item){
	m_currentItem = item;
}

/** Opens the popup menu. */
void CSearchDialogResultView::rightButtonPressed( QListBoxItem* item, const QPoint& p){
	m_currentItem = item;	
	if (m_currentItem) {
		QString text = QString::null;
		if ( dynamic_cast<CSwordBibleModuleInfo*>(m_module) ) {
			CSwordVerseKey key(m_module);
			key.setKey(item->text());
			text = key.getRenderedText();
		}
		else if (dynamic_cast<CSwordLexiconModuleInfo*>(m_module)) {
			CSwordLDKey key(m_module);
			key.setKey(item->text());
			text = key.getRenderedText();
		}
		if (!text.isEmpty())		
			emit keySelected( text  );		
		m_popup->popup(p);
	}
}

/**  */
void CSearchDialogResultView::mousePressed(QListBoxItem* item){
	m_currentItem = item;
	if (!item)
		return;
	
	QString text = QString::null;
	if (dynamic_cast<CSwordBibleModuleInfo*>(m_module)) {
		CSwordVerseKey key(m_module);
		key.setKey(item->text());
		text = key.getRenderedText();
	}
	else if (dynamic_cast<CSwordLexiconModuleInfo*>(m_module) ) {
		CSwordLDKey key(m_module);				
		key.setKey(item->text());
		text = key.getRenderedText();
	}
	if (!text.isEmpty())
		emit keySelected( text  );
}

/** This slot copies the current active item into the clipboard. */
void CSearchDialogResultView::slotCopyCurrent(){
	QString text;
	QList<QListBoxItem> list = selectedItems();
	for (list.first(); list.current(); list.next()) {
		text += list.current()->text();
	}
	KApplication::clipboard()->setText(text);
}

/** This slot copies the current active item into the clipboard. */
void CSearchDialogResultView::slotCopyCurrentWithKeytext(){
	QList<QListBoxItem> list = selectedItems();
	QString text;
	QString keyText, keyName;
	for (list.first(); list.current(); list.next()) {
		if (dynamic_cast<CSwordBibleModuleInfo*>(m_module)) {
			CSwordVerseKey key(m_module);
			key.setKey(list.current()->text());
			keyName = key.getKey();
			keyText += key.getStrippedText();
		}
		else if (dynamic_cast<CSwordLexiconModuleInfo*>(m_module) ) {
			CSwordLDKey key(m_module);						
			key.setKey(list.current()->text());
			keyName = key.getKey();			
			keyText += key.getStrippedText();
		}
		text += QString("%1\n%2\n\n").arg(keyName).arg(keyText);
	}
	
	KApplication::clipboard()->setText(text);
}

/** This slot copies the current active item into the clipboard. */
void CSearchDialogResultView::slotSaveCurrent(){
 	const QString file = KFileDialog::getSaveFileName (QString::null, i18n("*.txt | Text files\n *.* | All files (*.*)"), 0, i18n("Save key ..."));	
	if (!file.isNull()) {
		QList<QListBoxItem> list = selectedItems();
		QString text;
		for (list.first(); list.current(); list.next())
			text += list.current()->text() += "\n";
		CToolClass::savePlainFile(file, text);
	}
}

/** This slot copies the current active item into the clipboard. */
void CSearchDialogResultView::slotSaveCurrentWithKeytext(){
// 	QString keyText = QString::null;
//	if (dynamic_cast<CSwordBibleModuleInfo*>(m_module)) {
//		CSwordVerseKey key(m_module);
//		key.setKey(m_currentItem->text());
//		keyText = key.getStrippedText();
//	}
//	else if (dynamic_cast<CSwordLexiconModuleInfo*>(m_module) ) {
//		CSwordLDKey key(m_module);						
//		key.setKey(m_currentItem->text());
//		keyText = key.getStrippedText();
//	}	 	
	QList<QListBoxItem> list = selectedItems();
	QString text;
	QString keyText, keyName;
	for (list.first(); list.current(); list.next()) {
		if (dynamic_cast<CSwordBibleModuleInfo*>(m_module)) {
			CSwordVerseKey key(m_module);
			key.setKey(list.current()->text());
			keyName = key.getKey();
			keyText += key.getStrippedText();
		}
		else if (dynamic_cast<CSwordLexiconModuleInfo*>(m_module) ) {
			CSwordLDKey key(m_module);						
			key.setKey(list.current()->text());
			keyName = key.getKey();			
			keyText += key.getStrippedText();
		}
		text += QString("%1\n%2\n\n").arg(keyName).arg(keyText);
	}
	
 	const QString file = KFileDialog::getSaveFileName (QString::null, i18n("*.txt | Text files\n *.* | All files (*.*)"), 0, i18n("Save key ..."));	
	if (!file.isNull()) {
		CToolClass::savePlainFile( file, text);
	}
}

/** Returns the selected items of this listbox. */
QList<QListBoxItem> CSearchDialogResultView::selectedItems(){
	QList<QListBoxItem> list;
	QListBoxItem* item = firstItem();
	while (item) {
		if (item->selected())
			list.append(item);
		item = item->next();
	}
	return list;
}
