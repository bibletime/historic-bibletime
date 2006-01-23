/********* Read the file LICENSE for license details. *********/

#include "cmoduleresultview.h"

#include "backend/cswordmoduleinfo.h"

#include "frontend/cexportmanager.h"

#include "util/cresmgr.h"
#include "util/ctoolclass.h"

//Qt includes

//KDE includes
#include <klocale.h>
#include <kaction.h>
#include <kpopupmenu.h>

/********************************************
************  ModuleResultList **************
********************************************/

CModuleResultView::CModuleResultView(QWidget* parent, const char* name) :
	KListView(parent, name) {
	initView();
	initConnections();
   strongsResults = NULL;
};

CModuleResultView::~CModuleResultView() {}
;

/** Initializes this widget. */
void CModuleResultView::initView() {
	addColumn(i18n("Work"));
	addColumn(i18n("Hits"));

	//  setFullWidth(true);
	setSorting(0, true);
	setSorting(1, true);
	setAllColumnsShowFocus(true);


	//setup the popup menu
	m_popup = new KPopupMenu(this);
	// m_popup->insertTitle(i18n("Bible window"));

	m_actions.copyMenu = new KActionMenu(i18n("Copy..."), CResMgr::searchdialog::result::moduleList::copyMenu::icon, m_popup);
	m_actions.copy.result = new KAction(i18n("Reference only"), KShortcut(0), this, SLOT(copyResult()), this);
	m_actions.copyMenu->insert(m_actions.copy.result);
	m_actions.copy.resultWithText = new KAction(i18n("Reference with text"), KShortcut(0), this, SLOT(copyResultWithText()), this);
	m_actions.copyMenu->insert(m_actions.copy.resultWithText);
	m_actions.copyMenu->plug(m_popup);

	m_actions.saveMenu = new KActionMenu(i18n("Save..."), CResMgr::searchdialog::result::moduleList::saveMenu::icon, m_popup);
	m_actions.save.result = new KAction(i18n("Reference only"), KShortcut(0), this, SLOT(saveResult()), this);
	m_actions.saveMenu->insert(m_actions.save.result);
	m_actions.save.resultWithText = new KAction(i18n("Reference with text"), KShortcut(0), this, SLOT(saveResultWithText()), this);
	m_actions.saveMenu->insert(m_actions.save.resultWithText);
	m_actions.saveMenu->plug(m_popup);

	m_actions.printMenu = new KActionMenu(i18n("Print..."), CResMgr::searchdialog::result::moduleList::printMenu::icon, m_popup);
	m_actions.print.result = new KAction(i18n("Reference with text"), KShortcut(0), this, SLOT(printResult()), this);
	m_actions.printMenu->insert(m_actions.print.result);


	m_actions.printMenu->plug(m_popup);
}

/** Initializes the connections of this widget, */
void CModuleResultView::initConnections() {
	connect(this, SIGNAL(executed(QListViewItem*)),
			SLOT(executed(QListViewItem*)));
	connect(this, SIGNAL(contextMenu(KListView*, QListViewItem*, const QPoint&)),
			this, SLOT(showPopup(KListView*, QListViewItem*, const QPoint&)));
}

/** Setups the tree using the given list of modules. */
void CModuleResultView::setupTree( ListCSwordModuleInfo modules, const QString& searchedText ) {
	clear();
	QListViewItem* item = 0;
	QListViewItem* oldItem = 0;
	sword::ListKey result;

   if (strongsResults) {
      delete(strongsResults);
      strongsResults = NULL;
      }
	ListCSwordModuleInfo::iterator end_it = modules.end();
	for (ListCSwordModuleInfo::iterator it(modules.begin()); it != end_it; ++it) {
		//   for (modules.first(); modules.current(); modules.next()) {
		result = (*it)->searchResult();

		item = new KListViewItem(this, (*it)->name(), QString::number(result.Count()) );
		item->setPixmap(0,CToolClass::getIconForModule(*it) );
		oldItem = item;
      if (searchedText.find("strong:", 0) == 0) {
         QString sNumber;
         sNumber = searchedText.right(searchedText.length() - 7);
         setupStrongsResults((*it), item, sNumber);
         item->setOpen(TRUE);
      }
	};

	setSelected(currentItem(), true);
	executed(currentItem());
}

void CModuleResultView::setupStrongsResults(CSwordModuleInfo* module, QListViewItem* parent,
                                            const QString& sNumber)
   {
   QString lText;
   KListViewItem* item = 0;

   strongsResults = new StrongsResultClass(module, sNumber);

   for (int cnt = 0; cnt < strongsResults->Count(); cnt++)
      {
      lText = strongsResults->keyText(cnt);
      item = new KListViewItem(parent, lText, QString::number(strongsResults->keyCount(cnt)));
      item->setText(0, lText);
      }
   }


/** Is executed when an item was selected in the list. */
void CModuleResultView::executed( QListViewItem* i ) {
   QString itemText, lText;

	if (CSwordModuleInfo* m = CPointers::backend()->findModuleByName(i->text(0))) {
		emit moduleChanged();
		emit moduleSelected(m);
      return;
	}
   if (!strongsResults)
      return;

   itemText = i->text(0);
   for (int cnt = 0; cnt < strongsResults->Count(); cnt++)
      {
      lText = strongsResults->keyText(cnt);
      if (lText == itemText)
         {
         //clear the verses list
         emit moduleChanged();
         emit strongsSelected(activeModule(), strongsResults->getKeyList(cnt));
         return;
         }
      }
}

/** Returns the currently active module. */
CSwordModuleInfo* const CModuleResultView::activeModule() {
	Q_ASSERT(currentItem());
   QListViewItem* item = currentItem();

   if (item == NULL)
      return 0;

   // we need to find the parent most node because that is the node
   // that is the module name.
   while (item->parent())
      item = item->parent();

   if (item)
      return CPointers::backend()->findModuleByName(item->text(0));

   return 0;
}

/** No descriptions */
void CModuleResultView::showPopup(KListView*, QListViewItem*, const QPoint& point) {
	//make sure that all entries have the correct status
	m_popup->exec(point);
}

/** Copies the whole search result into the clipboard. */
void CModuleResultView::copyResult() {
	if (CSwordModuleInfo* m = activeModule()) {
		sword::ListKey result = m->searchResult();
		CExportManager mgr(i18n("Copy search result..."), true, i18n("Copying search result"));
		mgr.copyKeyList(&result,m,CExportManager::Text,false);
	};
}

/** Copies the whole search result with the text into the clipboard. */
void CModuleResultView::copyResultWithText() {
	if (CSwordModuleInfo* m = activeModule()) {
		sword::ListKey result = m->searchResult();
		CExportManager mgr(i18n("Copy search result..."), true, i18n("Copying search result"));
		mgr.copyKeyList(&result,m,CExportManager::Text,true);
	};
}

/** Saves the search result keys. */
void CModuleResultView::saveResult() {
	if (CSwordModuleInfo* m = activeModule()) {
		sword::ListKey result = m->searchResult();
		CExportManager mgr(i18n("Save search result..."), true, i18n("Saving search result"));
		mgr.saveKeyList(&result,m,CExportManager::Text,false);
	};
}

/** Saves the search result with it's text. */
void CModuleResultView::saveResultWithText() {
	if (CSwordModuleInfo* m = activeModule()) {
		sword::ListKey result = m->searchResult();
		CExportManager mgr(i18n("Save search result..."), true, i18n("Saving search result"));
		mgr.saveKeyList(&result,m,CExportManager::Text,true);
	};
}

/** Appends the whole search result to the printer queue. */
void CModuleResultView::printResult() {
	if (CSwordModuleInfo* m = activeModule()) {
		sword::ListKey result = m->searchResult();
		CExportManager mgr(i18n("Print search result..."), true, i18n("Printing search result"));
		mgr.printKeyList(&result,m);
	};
}
