/********* Read the file LICENSE for license details. *********/

#include "cbookreadwindow.h"
#include "cmodulechooserbar.h"
#include "cbuttons.h"

#include "backend/cswordtreekey.h"

#include "frontend/display/cdisplay.h"
#include "frontend/keychooser/cbooktreechooser.h"
#include "frontend/cprofilewindow.h"

#include "util/ctoolclass.h"
#include "util/cresmgr.h"

//Qt includes
#include <qsplitter.h>

//KDE includes
#include <kaction.h>
#include <klocale.h>
#include <kdeversion.h>

CBookReadWindow::CBookReadWindow(ListCSwordModuleInfo modules, CMDIArea* parent, const char *name) : CLexiconReadWindow(modules, parent, name){
}

CBookReadWindow::~CBookReadWindow(){
}

void CBookReadWindow::applyProfileSettings( CProfileWindow* profileWindow ) {
  CLexiconReadWindow::applyProfileSettings(profileWindow);

	const bool enable = static_cast<bool>( profileWindow->windowSettings() );
	if (enable) {
    m_treeAction->activate();
  }
};

void CBookReadWindow::storeProfileSettings( CProfileWindow* profileWindow ) {
  CLexiconReadWindow::storeProfileSettings(profileWindow);

  //store information about our show tree structure button
	profileWindow->setWindowSettings( static_cast<int>( m_treeAction->isChecked() ) );
};

void CBookReadWindow::initActions() {
  CLexiconReadWindow::initActions();
  
  m_treeAction = new KToggleAction(
  	i18n("Toggle tree view"),
  	CResMgr::displaywindows::bookWindow::toggleTree::icon,
  	CResMgr::displaywindows::bookWindow::toggleTree::accel,
  	this, SLOT(treeToggled()),
  	actionCollection(), "toggleTree");
};

void CBookReadWindow::insertKeyboardActions( KActionCollection* const a ) {
	CLexiconReadWindow::insertKeyboardActions(a);

  new KToggleAction(
  	i18n("Toggle tree view"),
  	CResMgr::displaywindows::bookWindow::toggleTree::icon,
  	CResMgr::displaywindows::bookWindow::toggleTree::accel,
  	a, "toggleTree");
}

/** No descriptions */
void CBookReadWindow::initConnections(){
	CLexiconReadWindow::initConnections();

 	connect(m_treeChooser, SIGNAL(keyChanged(CSwordKey*)),
 		this, SLOT(lookup(CSwordKey*)));
 	connect(m_treeChooser, SIGNAL(keyChanged(CSwordKey*)),
 		keyChooser(), SLOT(updateKey(CSwordKey*)));
 	connect(keyChooser(), SIGNAL(keyChanged(CSwordKey*)),
 		m_treeChooser, SLOT(updateKey(CSwordKey*)));
}

/** Init the view */
void CBookReadWindow::initView(){
  QSplitter* splitter = new QSplitter(this);
  setMainToolBar( new KToolBar(this) );
	addDockWindow(mainToolBar());

	m_treeChooser = new CBookTreeChooser(modules(), key(), splitter);
  setDisplayWidget( CDisplay::createReadInstance(this, splitter) ); 	 	

  setKeyChooser( CKeyChooser::createInstance(modules(), key(), mainToolBar()) );
	
	Q_ASSERT( mainToolBar() );
  setModuleChooserBar( new CModuleChooserBar(modules(), modules().first()->type(), mainToolBar()) );
	moduleChooserBar()->setButtonLimit(1);

  setDisplaySettingsButton( new CDisplaySettingsButton( &displayOptions(), &filterOptions(), modules(), mainToolBar()) );
  
  
	m_treeChooser->hide();
  	
  splitter->setResizeMode(m_treeChooser, QSplitter::FollowSizeHint);
  setCentralWidget( splitter );
	setIcon(CToolClass::getIconForModule(modules().first()));
}

void CBookReadWindow::initToolbars(){
	Q_ASSERT(m_treeAction);
	
	mainToolBar()->insertWidget(0,keyChooser()->sizeHint().width(),keyChooser());
	m_treeAction->plug(mainToolBar());
  m_treeAction->setChecked(false);
	
	mainToolBar()->insertWidget(1,moduleChooserBar()->sizeHint().width(),moduleChooserBar());
	mainToolBar()->insertWidget(2,displaySettingsButton()->size().width(),displaySettingsButton());

 	mainToolBar()->setFullSize(false);
	
	KAction* action = actionCollection()->action(
		CResMgr::displaywindows::general::search::actionName
	);
	if (action) {
		action->plug(mainToolBar());
	}
	
#if KDE_VERSION_MINOR < 1
	action->plugAccel( accel() );
#endif
}

/** Is called when the action was executed to toggle the tree view. */
void CBookReadWindow::treeToggled(){
	if (m_treeAction->isChecked()) {
		m_treeChooser->show();
	}
	else {
		m_treeChooser->hide();
	}
}

/** Reimplementation to take care of the tree chooser. */
void CBookReadWindow::modulesChanged(){
	CLexiconReadWindow::modulesChanged();
	m_treeChooser->setModules(modules());
}
