/***************************************************************************
                          cindexitem.cpp  -  description
                             -------------------
    begin                : Sam Jun 22 2002
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

//BibleTime includes
#include "cindexitem.h"
#include "cmainindex.h"
#include "backend/creferencemanager.h"
#include "frontend/ctoolclass.h"
#include "frontend/ctooltipmanager.h"
#include "resource.h"

//Qt includes
#include <qdragobject.h>
#include <qstringlist.h>

//KDE includes
#include <kconfig.h>
#include <klocale.h>


CItemBase::CItemBase(CMainIndex* mainIndex, const Type type) : KListViewItem(mainIndex), m_type(type) {
  m_type = type;
}

CItemBase::CItemBase(CItemBase* parentItem, const Type type) : KListViewItem(parentItem), m_type(type) {
  m_type = type;
}

CItemBase::~CItemBase() {
}

const QString CItemBase::toolTip() {
  return QString::null;
}

/** Returns the used main index. */
CMainIndex* CItemBase::listView() const{
  return dynamic_cast<CMainIndex*>( QListViewItem::listView() );
}

void CItemBase::init() {
  update();
}

void CItemBase::update() {
}

const CItemBase::Type& CItemBase::type() const{
  return m_type;
};

void CItemBase::moveAfter( CItemBase* const item ){
	if (!item)
		return;
	
	if ( parent() != item->parent() ) { //different levels
//		if (type == AllowDifferentParents) { //different parents are allowed
//			if (item->parent())
//				item->parent()->insertItem(this); //insert item to the childs
//			else
//				listView()->insertItem(this);
//			moveItem(item);
//		}
	}
	else {
		moveItem(item); //both items are on the same level, so we can use moveItem
	}
}

void CItemBase::dropped( QDropEvent* e ) {
}


/** Returns true if the given action should be enabled in the popup menu. */
const bool CItemBase::enableAction( const MenuAction action ){
  return false;
}

/** No descriptions */
const bool CItemBase::isMovable(){
  return false;
}

/* ---------------------------------------------- */
/* ---------- new class: CModuleItem ------------ */
/* ---------------------------------------------- */

CModuleItem::CModuleItem(CTreeFolder* parentItem, CSwordModuleInfo* module) : CItemBase(parentItem), m_module(module) {
}

CModuleItem::~CModuleItem() {
}

/** No descriptions */
void CModuleItem::update(){
  if (m_module) {
    setPixmap(0, CToolClass::getIconForModule(m_module));
    setText(0,m_module->name() );
  }
}

void CModuleItem::init(){
  setDragEnabled(false);
  setDropEnabled(true);

  update();
}


/** Reimplementation to handle text drops on a module. In this case open the searchdialog. In the case of a referebnce open the module at the given position. */
bool CModuleItem::acceptDrop( const QMimeSource* src ) const {
  qWarning("CModuleItem::acceptDrop( const QMimeSource* src )");
  if (src->provides("text/"REFERENCE) || src->provides("text/"BOOKMARK)) {
    QString str;
    QCString submime;
    if (QTextDrag::canDecode(src) && (QTextDrag::decode(src,str,submime=REFERENCE) || QTextDrag::decode(src,str,submime=BOOKMARK)) ) {
		  QString ref;
      QString mod;
      CReferenceManager::decodeReference(str,mod,ref);

      CSwordModuleInfo* m = backend()->findModuleByName(mod);
      if (m && module()->type() == m->type()) { //it makes only sense to create a new window for a module with the same type
        return true;
      }
      else
        return false;
    }
  }
  if (src->provides("text/"TEXT))
    return true;
  return false;
}

/** No descriptions */
void CModuleItem::dropped( QDropEvent* e ){
  QString str;
  QCString submime;
  if (QTextDrag::canDecode(e) && (QTextDrag::decode(e,str,submime=REFERENCE) || QTextDrag::decode(e,str,submime=BOOKMARK)) ) {
		QString ref;
    QString mod;
    CReferenceManager::decodeReference(str,mod,ref);

    CSwordModuleInfo* m = backend()->findModuleByName(mod);
    if (m && module()->type() == m->type()) { //it makes only sense to create a new window for a module with the same type
      ListCSwordModuleInfo modules;
      modules.append(module());

      listView()->emitModulesChosen(modules, ref);
    }
  }
  else if (QTextDrag::canDecode(e) && QTextDrag::decode(e,str,submime=TEXT)){
		//plain text was dragged -> open searchdialog
		if ( module() ) {
      ListCSwordModuleInfo modules;
      modules.append(module());

      listView()->openSearchDialog(modules, str);
    }
  }

}


/** Reimplementation. */
const QString CModuleItem::toolTip(){
    QString text;
		text = i18n("Module") + QString::fromLatin1(": <B>%1</B><HR>").arg( module()->name() );
		text += module()->config(CSwordModuleInfo::Description) + QString::fromLatin1("<HR>");
		text += i18n("Language")+ QString::fromLatin1(": %1<BR>").arg(module()->module()->Lang());
		if (module()->isEncrypted())
			text += i18n("Unlock key") + QString::fromLatin1(": %1<BR>")
				.arg(!module()->config(CSwordModuleInfo::CipherKey).isEmpty() ? module()->config(CSwordModuleInfo::CipherKey) : QString("<FONT COLOR=\"red\">%1</FONT>").arg(i18n("not set")));
		if (module()->hasVersion())
			text += i18n("Version") + QString::fromLatin1(": %1<BR>").arg(module()->config(CSwordModuleInfo::ModuleVersion));
     	     	
   	QString options;
   	unsigned int opts;
   	for (opts = CSwordBackend::filterOptionsMIN; opts <= CSwordBackend::filterOptionsMAX; ++opts){
   		if (module()->has( (CSwordBackend::FilterOptions) opts )) {
     		if (!options.isEmpty())
     			options += QString::fromLatin1(", ");
     		options += CSwordBackend::translatedOptionName( (CSwordBackend::FilterOptions) opts);
   		}
   	}
   	if (!options.isEmpty())
   		text += i18n("Options") + QString::fromLatin1(": <font size= \"-1\">") + options + QString::fromLatin1("</font>");
     		
		if (text.right(4) == QString::fromLatin1("<BR>"))
			text = text.left(text.length()-4);
		return text;
}

/** Returns the used module. */
CSwordModuleInfo* const CModuleItem::module() const {
  return m_module;
}

/** Returns the about module info of the used module. */
const QString CModuleItem::aboutInfo(){
	QString text;	

  if (module()->hasVersion())
    text += QString::fromLatin1("<b>%1:</b> %2<br>")
    	.arg(i18n("Version"))
    	.arg(module()->config(CSwordModuleInfo::ModuleVersion));

	text += QString::fromLatin1("<b>%1:</b> %2<br><b>%3:</b> %4<br>")
		.arg(i18n("Location"))
		.arg(module()->config(CSwordModuleInfo::AbsoluteDataPath))
		.arg(i18n("Language"))
		.arg(module()->module()->Lang());

	if (module()->module()->isWritable())
		text += QString::fromLatin1("<b>%1:</b> %2<br>")
							.arg(i18n("Writable"))
							.arg(i18n("yes"));

	if ( module()->isEncrypted() )
		text += QString::fromLatin1("<b>%1:</b> %2<br>")
							.arg(i18n("Unlock key"))
							.arg(module()->config(CSwordModuleInfo::CipherKey));	
	text += QString::fromLatin1("<b>%1:</b> %2<br>").arg(i18n("Encoding")).arg(module()->isUnicode() ? i18n("Unicode") : i18n("iso8859-1"));

	QString options;
	unsigned int opts;
	for (opts = CSwordBackend::filterOptionsMIN; opts <= CSwordBackend::filterOptionsMAX; ++opts){
		if (module()->has( static_cast<CSwordBackend::FilterOptions>(opts) )){
  		if (!options.isEmpty())
  			options += QString::fromLatin1(", ");
  		options += CSwordBackend::translatedOptionName( static_cast<CSwordBackend::FilterOptions>(opts) );
		}
	}
	if (!options.isEmpty())
		text += QString("<b>%1:</b> %2<br>")
			.arg(i18n("Features"))
			.arg(options);

	text += QString::fromLatin1("<b>%1:</b><br> <font size=\"-1\">%2</font>")
						.arg("About")
						.arg(module()->config(CSwordModuleInfo::AboutInformation));
  return text;
}


/** Reimplementation from  CItemBase. */
const bool CModuleItem::enableAction( const MenuAction action ){
  if (action == SearchInModules || action == AboutModule)
    return true;
  if (module()->isEncrypted() && action == UnlockModule)
    return true;
  return false;
}

/* ----------------------------------------------*/
/* ---------- new class: CBookmarkItem ------------*/
/* ----------------------------------------------*/

CBookmarkItem::CBookmarkItem(CFolderBase* parentItem, CSwordModuleInfo* module, const QString& key, const QString& description) : CItemBase(parentItem), m_key(key), m_description(description), m_module(module) {
}

CBookmarkItem::~CBookmarkItem() {

}

/** No descriptions */
void CBookmarkItem::update(){
//  CItemBase::update();
  const QString title = QString::fromLatin1("%1 (%2)").arg(key()).arg(module() ? module()->name() : i18n("unknown"));
  setText(0, title);
  setPixmap(0,BOOKMARK_ICON_SMALL);
}

void CBookmarkItem::init(){
//  CItemBase::init();
  update();
  setDropEnabled(false);
  setDragEnabled(false);
}

/** Reimplementation. */
const QString CBookmarkItem::toolTip(){
  return CTooltipManager::textForReference(module()->name(), key(), description());
}

/** Returns the used module. */
CSwordModuleInfo* const CBookmarkItem::module() {
  return m_module;
}

/** Returns the used key. */
const QString& CBookmarkItem::key(){
  return m_key;
}

/** Returns the used description. */
const QString& CBookmarkItem::description(){
   return m_description;
}

/** No descriptions */
const bool CBookmarkItem::isMovable(){
  return true;
}

/** Reimplementation to handle  the menu entries of the main index. */
const bool CBookmarkItem::enableAction(const MenuAction action){
  if (action == ChangeBookmark || action == PrintBookmarks || action == DeleteEntries)
    return true;

  return false;
}

void CBookmarkItem::print(){

}

/** Changes this bookmark. */
void CBookmarkItem::rename(){

}

/****************************************/
/*****  class: CItemFolder  *************/
/****************************************/

CFolderBase::CFolderBase(CMainIndex* mainIndex, const Type type) : CItemBase(mainIndex, type) {
}

CFolderBase::CFolderBase(CFolderBase* parentItem, const Type type) : CItemBase(parentItem, type) {
}

CFolderBase::CFolderBase(CFolderBase* parentFolder, const QString& caption) : CItemBase(parentFolder){
  setText(0, caption);
}

CFolderBase::~CFolderBase() {
}

const bool CFolderBase::isFolder() {
  return true;
}

void CFolderBase::update() {
  CItemBase::update();
  if (isOpen() && childCount())
    setPixmap(0,GROUP_OPEN_ICON_SMALL);
  else
    setPixmap(0,GROUP_ICON_SMALL);
}

void CFolderBase::init() {
  CItemBase::init();
  setDropEnabled(false);
  setDragEnabled(false);
}

/** No descriptions */
void CFolderBase::setOpen( bool open ){
  KListViewItem::setOpen(open);
  update();
}

/** The function which renames this folder. */
void CFolderBase::rename(){
  startRename(0);
}

/** Creates a new sub folder of this folder. */
void CFolderBase::newSubFolder(){
  if (dynamic_cast<CBookmarkFolder*>(this) || dynamic_cast<CBookmarkFolder::SubFolder*>(this) ) {
    CBookmarkFolder::SubFolder* f = new CBookmarkFolder::SubFolder(this, i18n("New folder"));
    f->init();

    listView()->setCurrentItem(f);
    listView()->ensureItemVisible(f);
    f->rename();
  }
}

/****************************************/
/*****  class: CTreeFolder  *************/
/****************************************/


CTreeFolder::CTreeFolder(CMainIndex* mainIndex, const Type type, const QString& language) : CFolderBase(mainIndex, type) {
  m_language = language;
}

CTreeFolder::CTreeFolder(CFolderBase* item, const Type type, const QString& language) : CFolderBase(item, type) {
  m_language = language;
}

CTreeFolder::~CTreeFolder(){
}

void CTreeFolder::addGroup(const Type type, const QString language){
  CTreeFolder* i = 0;
  if (type == BookmarkFolder)
    i = new CBookmarkFolder(this);
  else if (type == OldBookmarkFolder)
    i = new COldBookmarkFolder(this);
  else
    i = new CTreeFolder(this, type, language);
  i->init();
  if (!i->childCount())
    delete i;
}

void CTreeFolder::addModule(CSwordModuleInfo* const module){
  CModuleItem* i = new CModuleItem(this, module);
  i->init();
}

void CTreeFolder::addBookmark(CSwordModuleInfo* module, const QString& key, const QString& description){
  CBookmarkItem* i = new CBookmarkItem(this, module, key, description);
  i->init();
}

void CTreeFolder::update(){
  CFolderBase::update();
}

void CTreeFolder::init(){
  if (language() == "*") {
    switch (type()) {
      case BibleModuleFolder:
        setText(0,i18n("Bibles"));
        break;
      case CommentaryModuleFolder:
        setText(0,i18n("Commentaries"));
        break;
      case LexiconModuleFolder:
        setText(0,i18n("Lexicon"));
        break;
      case BookModuleFolder:
        setText(0,i18n("Books"));
        break;
      case DevotionalModuleFolder:
        setText(0,i18n("Daily devotionals"));
        break;
      case GlossaryModuleFolder:
        setText(0,i18n("Glossaries"));
        break;
      case BookmarkFolder:
        setText(0,i18n("Bookmark"));
        break;
      case OldBookmarkFolder:
        setText(0,i18n("Old bookmarks"));
        break;
      default:
        setText(0, i18n("Unknown"));
        break;
    };
  }
  else {
    setText(0, language());
  }
  initTree();
  update();
}

void CTreeFolder::initTree(){
  qWarning("CTreeMgr::initTree");
  if (type() == Unknown)
    return;

  CSwordModuleInfo::ModuleType moduleType = CSwordModuleInfo::Unknown;
  if (type() == BibleModuleFolder) {
    qWarning("insert Bibles!");
    moduleType = CSwordModuleInfo::Bible;
  }
  else if (type() == CommentaryModuleFolder)
    moduleType = CSwordModuleInfo::Commentary;
  else if (type() == LexiconModuleFolder || type() == GlossaryModuleFolder || type() == DevotionalModuleFolder)
    moduleType = CSwordModuleInfo::Lexicon;
  else if (type() == BookModuleFolder)
    moduleType = CSwordModuleInfo::GenericBook;

  //get all modules by the given type
  ListCSwordModuleInfo allModules = backend()->moduleList();
  ListCSwordModuleInfo usedModules;
  for (CSwordModuleInfo* m = allModules.first(); m; m = allModules.next()) {
    if (m->type() == moduleType) { //found a module, check if the type is correct (devotional etc.)
      if (type() == GlossaryModuleFolder && !m->has(CSwordModuleInfo::Glossary)) {
        continue;
      }
      if (type() == DevotionalModuleFolder && !m->has(CSwordModuleInfo::DailyDevotional)) {
        continue;
      }
      if (language() == QString::fromLatin1("*") || (language() != QString::fromLatin1("*") && QString::fromLatin1(m->module()->Lang()) == language()) )//right type and language!
        usedModules.append(m);
    }
  }

  qWarning("using %i modules language %s", usedModules.count(), language().latin1());

  //we have now all modules we want to have
  if (language() == QString::fromLatin1("*")) { //create subfolders for each language
    QStringList usedLangs;
    for (CSwordModuleInfo* m = usedModules.first(); m; m = usedModules.next()) {
      QString lang = QString::fromLatin1(m->module()->Lang());
      if (lang.isEmpty())
        lang = "xx";
      if (!usedLangs.contains(lang)) {
        usedLangs.append(lang);
      }
    }
    QStringList::iterator it;
    for (it = usedLangs.begin(); it != usedLangs.end(); ++it) {
      addGroup(/**it,*/ type(), *it);
    }
  }
  else if (usedModules.count() > 0){ //create subitems with the given type and language
    for (CSwordModuleInfo* m = usedModules.first(); m; m = usedModules.next()) {
      addModule(m);
    }
  }

  sortChildItems(0,true);
}

const QString CTreeFolder::language() const {
  return m_language;
};


/* ----------------------------------------------*/
/* ---------- new class: COldBookmarkImportItem -*/
/* ----------------------------------------------*/

COldBookmarkFolder::COldBookmarkFolder(CTreeFolder* folder) : CBookmarkFolder(folder, OldBookmarkFolder) {
}

COldBookmarkFolder::~COldBookmarkFolder() {
}

/** Reimplementation to handle special bookmark tree. */
void COldBookmarkFolder::initTree(){
//  qWarning("Init bookmark tree!");
  KConfig* configFile = new KConfig("bt-groupmanager", false, false );
	KConfigGroupSaver groupSaver(configFile, "Groupmanager");	
  readGroups(configFile);

  CFolderBase* parent = 0;
	QStringList	groupList = configFile->readListEntry("Groups");

	//read in all bookmarks
	QStringList	bookmarkList 							= configFile->readListEntry("Bookmarks");
	QStringList	bookmarkModulesList 			= configFile->readListEntry("Bookmark modules");
	QStringList	bookmarkDescriptionsList 	= configFile->readListEntry("Bookmark descriptions");
	QValueList<int>	parentList						= configFile->readIntListEntry("Bookmark parents");

	QStringList::Iterator it_bookmarks 	= bookmarkList.begin();
	QStringList::Iterator it_modules 		= bookmarkModulesList.begin();
	QStringList::Iterator it_descriptions	= bookmarkDescriptionsList.begin();
	QValueList<int>::Iterator it_parents = parentList.begin();

	CSwordModuleInfo* moduleInfo = 0;
	CItemBase *myItem = 0;	
	CItemBase *oldItem = 0;

	while ( it_bookmarks != bookmarkList.end() && it_parents != parentList.end()
			 		&& it_modules != bookmarkModulesList.end() )
  {
		moduleInfo = backend()->findModuleByName((*it_modules));
		if ( (*it_parents) == -1) {
 			myItem = new CBookmarkItem(this, moduleInfo, (*it_bookmarks), (*it_descriptions));
		}
		else {
			parent = findParent( (*it_parents) );
			if (parent)
				myItem = new CBookmarkItem(parent, moduleInfo, (*it_bookmarks), (*it_descriptions));
			else
				myItem = new CBookmarkItem(this, moduleInfo, (*it_bookmarks), (*it_descriptions));
      myItem->init();
		}
		if (myItem && oldItem) {
			myItem->moveAfter(oldItem);
		}
		oldItem = myItem;

		++it_parents;
		++it_modules;
		++it_descriptions;
		++it_bookmarks;
	}
}

CFolderBase* COldBookmarkFolder::findParent( const int ID ){
//	qWarning("CBookmarkItem::findParent( const int ID )");
  CFolderBase* myItem = 0;
	int index = 0;
	
	QListViewItemIterator it = QListViewItemIterator(this);

  //traverse the tree and try to find the group with the id ID index using comparision
  for( ; it.current(); it++ ) {
 		myItem = dynamic_cast<CFolderBase*>( it.current() );
    Q_ASSERT(myItem);
		if (myItem != this && myItem && myItem->isFolder() ) {//group item
			if (index == ID) {
				return myItem;
			}
			else
				index++;
		}
	}		
	return 0;	// return 0 if parent wasn't found
}

const bool COldBookmarkFolder::readGroups(KConfig* configFile ) {
//	qWarning("CBookmarkItem::readGroups(KConfig* configFile, CGroupManagerItem* group, const Action action)");
 //read and create group entries
	CFolderBase* parent = 0;
	bool groupExists = false;	
	CFolderBase* oldItem = 0;
	CFolderBase* newItem = 0;
	
	QStringList	groupList = configFile->readListEntry("Groups");
	QValueList<int>	parentList = configFile->readIntListEntry("Group parents");	
	QStringList::Iterator it_groups = groupList.begin();
	QValueList<int>::Iterator it_parents = parentList.begin();
	
	while ( (it_groups != groupList.end()) && (it_parents != parentList.end()) ) {
		groupExists = false;
		if ( (*it_parents) == -1) { //no parent item saved
			parent = this;
		}
		else {
			parent = findParent( (*it_parents) );
//		 	if (!parentItem)
//				parentItem = this;
		}
		
		if (!groupExists) {
			newItem = new SubFolder(parent ? parent : this, (*it_groups));
		  newItem->init();

			if (newItem && oldItem ) {			
				newItem->moveAfter( oldItem );
			}
			if (newItem) {
				/* we can't move a topgroup behind a subgroup, so we use multiple
				 parent calls
				*/
//				while (parentId(newItem) > (*it_parents) ) {
//					if ( newItem && newItem->parent() )
//						newItem = dynamic_cast<CItem*>(newItem->parent());
//					else
//						break;
//				}
				oldItem = newItem;
			}					
		}
		
		++it_parents;
		++it_groups;
	}	
	return true;	
};

const int COldBookmarkFolder::parentId(CItemBase *item) {
	int ret = -1; 	// the view and the parent item have id -1
	int index = 0;
	CItemBase* myItem = 0;
	
	if (item) {
		if (item->parent()) {
		  QListViewItemIterator it = QListViewItemIterator( this );
		  	
			while (it.current() && (it.current() != item->parent()) ) {
				myItem = dynamic_cast<CItemBase*>(it.current());
				if (myItem != this && myItem && myItem->isFolder()) {
					index++;
				}
				it++;
			}
			if (it.current())
				ret = index;
		}
	}
	return ret;
}

/* --------------------------------------------------*/
/* ---------- new class: CBookmarkFolder::SubFolder--*/
/* --------------------------------------------------*/

CBookmarkFolder::SubFolder::SubFolder(CFolderBase* parentItem, const QString& caption) : CFolderBase(parentItem, caption) {
}

CBookmarkFolder::SubFolder::~SubFolder() {

}

void CBookmarkFolder::SubFolder::init() {
  CFolderBase::init();
  setDropEnabled(true);
  setRenameEnabled(0,true);
}

/** Is called when an item was dropped on this subfolder. */
void CBookmarkFolder::SubFolder::dropped(QDropEvent * e) {
  QString str;
  QCString submime;

  if (acceptDrop(e) && QTextDrag::decode(e,str,submime=REFERENCE) ) { //a drag object, we can handle
    QString mod;
    QString key;
    CReferenceManager::decodeReference(str,mod,key);

    CSwordModuleInfo* module = backend()->findModuleByName(mod);
    CBookmarkItem* i = new CBookmarkItem(this, module, key, QString::null);
    i->init();
  }
  else if (acceptDrop(e) && QTextDrag::decode(e,str,submime=BOOKMARK) ) { //a drag object, we can handle
  }
}

bool CBookmarkFolder::SubFolder::acceptDrop(const QMimeSource * src) const {
  qWarning("CBookmarkFolder::SubFolder::acceptDrop(const QMimeSource * src)");
  if (src->provides("text/"REFERENCE) || src->provides("text/"BOOKMARK)) {
    return true;
  }
  return false;
}

/** Reimplementation from  CItemBase. */
const bool CBookmarkFolder::SubFolder::enableAction(const MenuAction action){
  if (action == ChangeFolder || action == NewFolder || action == DeleteEntries || action == ImportBookmarks )
    return true;

  if (childCount() > 0 && action == ExportBookmarks)
    return true;

  return false;
}

/* --------------------------------------------------*/
/* ---------- new class: CBookmarkFolder ------------*/
/* --------------------------------------------------*/

CBookmarkFolder::CBookmarkFolder(CMainIndex* mainIndex, const Type type) : CTreeFolder(mainIndex, type, "*") {

}

CBookmarkFolder::CBookmarkFolder(CFolderBase* parentItem, const Type type) : CTreeFolder(parentItem, type, "*") {

}

CBookmarkFolder::~CBookmarkFolder() {

}

void CBookmarkFolder::initTree(){
  addGroup(OldBookmarkFolder, "*");
}

/** Reimplementation. */
const bool CBookmarkFolder::enableAction(const MenuAction action){
  if (action == NewFolder || action == ImportBookmarks)
    return true;

  if (action == ExportBookmarks && childCount())
    return true;

  return false;
}


void CBookmarkFolder::exportBookmarks(){
}


void CBookmarkFolder::importBookmarks(){
}

bool CBookmarkFolder::acceptDrop(const QMimeSource * src) const {
  qWarning("CBookmarkFolder::acceptDrop(const QMimeSource * src)");
  if (src->provides("text/"REFERENCE) || src->provides("text/"BOOKMARK)) {
    return true;
  }
  return false;
}

void CBookmarkFolder::dropped(QDropEvent * e) {
  QString str;
  QCString submime;

  if (acceptDrop(e) && QTextDrag::decode(e,str,submime=REFERENCE) ) { //a drag object, we can handle
    QString mod;
    QString key;
    CReferenceManager::decodeReference(str,mod,key);

    CSwordModuleInfo* module = backend()->findModuleByName(mod);
    CBookmarkItem* i = new CBookmarkItem(this, module, key, QString::null);
    i->init();
  }
  else if (acceptDrop(e) && QTextDrag::decode(e,str,submime=BOOKMARK) ) { //a drag object, we can handle
  }
}