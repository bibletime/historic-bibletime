#include "cswordsetupmodulelistview.h"

#include "backend/cswordbackend.h"
#include "backend/cswordmoduleinfo.h"

#include "util/cresmgr.h"
#include "util/ctoolclass.h"

//QT includes

//KDE includes
#include <klocale.h>
#include <kiconloader.h>

using namespace sword;

namespace InstallationManager {

CSwordSetupModuleListView::CSwordSetupModuleListView(QWidget *parent, bool is_remote) :
		QListView(parent) {
	
	m_is_remote = is_remote;
		
	addColumn(i18n("Name"));
	setColumnWidthMode( 0, QListView::Manual );
	setColumnWidth( 0, 150 ); //don`t get too broad
	addColumn(i18n("Status"));
	addColumn(i18n("Installed")); //version
	if (m_is_remote){
		addColumn(i18n("Remote")); //version
	}
	else{
		addColumn(i18n("Location"));
	}
	setAllColumnsShowFocus(true);
//  	setFullWidth(true);
	setRootIsDecorated(true);
	
	init();

}

void CSwordSetupModuleListView::init(){
 #if QT_VERSION >= 320
	m_categoryBible = new QCheckListItem(this, i18n("Bibles"), QCheckListItem::CheckBoxController);
	m_categoryCommentary = new QCheckListItem(this, i18n("Commentaries"), QCheckListItem::CheckBoxController);
	m_categoryLexicon = new QCheckListItem(this, i18n("Lexicons"), QCheckListItem::CheckBoxController);
	m_categoryBook = new QCheckListItem(this, i18n("Books"), QCheckListItem::CheckBoxController);
	m_categoryDevotionals = new QCheckListItem(this, i18n("Daily Devotionals"), QCheckListItem::CheckBoxController);
	m_categoryGlossaries = new QCheckListItem(this, i18n("Glossaries"), QCheckListItem::CheckBoxController);
#else
// 	Qt <= 3.1.x doesn't support the CheckBoxController!, remove the define as soon as we switch to the new Qt
	m_categoryBible = new QCheckListItem(this, i18n("Bibles"), QCheckListItem::Controller);
	m_categoryCommentary = new QCheckListItem(this, i18n("Commentaries"), QCheckListItem::Controller);
	m_categoryLexicon = new QCheckListItem(this, i18n("Lexicons"), QCheckListItem::Controller);
	m_categoryBook = new QCheckListItem(this, i18n("Books"), QCheckListItem::Controller);
	m_categoryDevotionals = new QCheckListItem(this, i18n("Daily Devotionals"), QCheckListItem::Controller);
	m_categoryGlossaries = new QCheckListItem(this, i18n("Glossaries"), QCheckListItem::Controller);
#endif

  m_categoryBible->setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));
  m_categoryCommentary->setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));
  m_categoryLexicon->setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));
  m_categoryBook->setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));
  m_categoryDevotionals->setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));
  m_categoryGlossaries->setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));

  m_categoryBible->setOpen(true);
  m_categoryCommentary->setOpen(true);
  m_categoryLexicon->setOpen(true);
  m_categoryBook->setOpen(true);
  m_categoryDevotionals->setOpen(true);
  m_categoryGlossaries->setOpen(true);
}

void CSwordSetupModuleListView::finish(){
  if (!m_categoryBible->childCount())
    delete m_categoryBible;
  if (!m_categoryCommentary->childCount())
    delete m_categoryCommentary;
  if (!m_categoryBook->childCount())
    delete m_categoryBook;
  if (!m_categoryLexicon->childCount())
    delete m_categoryLexicon;
  if (!m_categoryDevotionals->childCount())
    delete m_categoryDevotionals;
  if (!m_categoryGlossaries->childCount())
    delete m_categoryGlossaries;
}

void CSwordSetupModuleListView::clear(){
	QListView::clear();
	init();
}

void CSwordSetupModuleListView::addModule(CSwordModuleInfo* module, QString localVersion){

	QListViewItem* parent = 0;
	switch ( module->type() ) {
		case CSwordModuleInfo::Bible:
			parent = m_categoryBible;
			break;
		case CSwordModuleInfo::Commentary:
			parent = m_categoryCommentary;
			break;
		case CSwordModuleInfo::Lexicon:
			parent = m_categoryLexicon;
			break;
		case CSwordModuleInfo::GenericBook:
			parent = m_categoryBook;
			break;
		default:
			parent = 0;
			break;
	}

	//handling for special module types
	if ((parent == m_categoryLexicon) && (module->category() == CSwordModuleInfo::Glossary)) {
		parent = m_categoryGlossaries;
	}
	if ((parent == m_categoryLexicon) && (module->category() == CSwordModuleInfo::DailyDevotional)) {
		parent = m_categoryDevotionals;
	}

	//now we know the category, find the right language group in that category
	const CLanguageMgr::Language* const lang = module->language();
	QString langName = lang->translatedName();
	if (!lang->isValid()) {
		langName = QString::fromLatin1(module->module()->Lang());
	}

	QListViewItem * langFolder = 0;
	
	if (parent){
		langFolder = parent->firstChild();
		while( langFolder ) { //try to find language folder if it exsists
			if (langFolder->text(0) == langName) { //found right folder
				break;
			}
			langFolder = langFolder->nextSibling();
		}
	}

	if (!langFolder) { //not yet there
#if QT_VERSION >= 320
		langFolder = new QCheckListItem(parent, langName, QCheckListItem::CheckBoxController);
#else
		langFolder = new QCheckListItem(parent, langName, QCheckListItem::Controller);
#endif
		langFolder->setPixmap(0, SmallIcon(CResMgr::mainIndex::closedFolder::icon, 16));
		langFolder->setOpen(false);
	}

	QListViewItem* newItem = 0;
	if (langFolder) {
#if QT_VERSION >= 320
		newItem = new QCheckListItem(langFolder, module->name(), QCheckListItem::CheckBoxController);
#else
		newItem = new QCheckListItem(langFolder, module->name(), QCheckListItem::CheckBox);
#endif
	}
	else { //shouldn't happen
#if QT_VERSION >= 320
		newItem = new QCheckListItem(this, module->name(), QCheckListItem::CheckBoxController);
#else
		newItem = new QCheckListItem(this, module->name(), QCheckListItem::CheckBox);
#endif
	}

	newItem->setPixmap(0, CToolClass::getIconForModule(module));
	if (m_is_remote){
		newItem->setText(1, localVersion.isEmpty() ? i18n("New") : i18n("Updated"));
	}
	else{
		newItem->setText(1, i18n("Installed") );
	}
	newItem->setText(2, localVersion);
	if (m_is_remote)
		newItem->setText(3, module->config(CSwordModuleInfo::ModuleVersion));
	else
		newItem->setText(3, module->config(CSwordModuleInfo::AbsoluteDataPath));
}

QStringList CSwordSetupModuleListView::selectedModules(){
	QStringList moduleList;

	QListViewItemIterator list_it( this );
	while ( list_it.current() ) {
		QCheckListItem* i = dynamic_cast<QCheckListItem*>( list_it.current() );
		if (i && i->isOn() && !(i->text(1).isEmpty()) ) {
			moduleList << i->text(0);
		}
		++list_it;
	}

	return moduleList;
}

} //NAMESPACE