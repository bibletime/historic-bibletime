/********* Read the file LICENSE for license details. *********/

#include "cmdiarea.h"
#include "backend/cswordmoduleinfo.h"
#include "backend/cswordversekey.h"

#include "frontend/displaywindow/cdisplaywindow.h"
#include "frontend/keychooser/ckeychooser.h"

#include "config.h"

//KDE includes
#include <kpopupmenu.h>
#include <kapplication.h>
#include <klocale.h>

//QT includes
#include <qobjectlist.h>
#include <qtimer.h>
#include <qevent.h>
#include <qtimer.h>


CMDIArea::CMDIArea(QWidget *parent, const char *name )
		: QWorkspace(parent, name),
		m_guiOption(Nothing),
		m_childEvent(false),
		m_appCaption(QString::null)
{
	initView();
	initConnections();
	readSettings();
}


/** Initializes the view of the MDI area */
void CMDIArea::initView() {
	setPaletteBackgroundColor( parentWidget()->paletteBackgroundColor() );	//work around a KDE bug (IMHO was in KDE 2.x)
}

/** Initilizes the connectiosn to SIGNALS */
void CMDIArea::initConnections() {
	connect(this, SIGNAL(windowActivated(QWidget*)),
	        this, SLOT(slotClientActivated(QWidget*)));
}

/** Called whan a client window was activated */
void CMDIArea::slotClientActivated(QWidget* client) {
	if (!client || !isUpdatesEnabled()) {
		return;
	}

	CDisplayWindow* sp = dynamic_cast<CDisplayWindow*>(client);
	if (!sp || !sp->isReady()) {
		return;
	}

	QWidgetList windows = windowList();
	for ( QWidget* w = windows.first(); w; w = windows.next() ) {
		//Don't use!! It would disable accel enabling for the active window, see CDisplayWindow::windowActivated
		/*    if (w == client)
						continue;
		*/
		CDisplayWindow* window = dynamic_cast<CDisplayWindow*>(w);
		window->windowActivated( (window == sp) ? true : false);
	}

	emit sigSetToplevelCaption( ( m_appCaption = client->caption().stripWhiteSpace() ) );
}

/** Reimplementation. Used to make use of the fixedGUIOption part. */
void CMDIArea::childEvent( QChildEvent * e ) {
	QWorkspace::childEvent(e);

	if ( m_childEvent /*|| !isUpdatesEnabled()*/ || !e) {
/*		if (windowList().count() == 0) {
			m_deleting = false;
		}*/
		return;
	}

	m_childEvent = true;

	if (!windowList().count()) {
		m_appCaption = QString::null;
		emit sigSetToplevelCaption( KApplication::kApplication()->makeStdCaption(m_appCaption) );
		emit sigLastPresenterClosed();
	}

// 	Q_ASSERT(!m_deleting /*&& isUpdatesEnabled()*/);
	if ((e->inserted() || e->removed()) ) {
		if (e->inserted() && e->child() && e->child()->inherits("CDisplayWindow")) {
			e->child()->installEventFilter(this); //make sure we catch the events of the new window
		} else if (e->removed() && e->child() && e->child()->inherits("CDisplayWindow")) {
			e->child()->removeEventFilter(this); //make sure we catch the events of the new window
		}

		triggerWindowUpdate();
	}

	m_childEvent = false;

/*	if (windowList().count() == 0) {
		m_deleting = false;
	}*/
}

/** Reimplementation */
void CMDIArea::resizeEvent(QResizeEvent* e) {
	QWorkspace::resizeEvent(e);

	if (/*m_deleting || */!isUpdatesEnabled()) {
		return;
	};

	triggerWindowUpdate();
}

/**  */
void CMDIArea::saveSettings() {}

/**  */
void CMDIArea::readSettings() {}

/** Deletes all the presenters in the MDI area. */
void CMDIArea::deleteAll() {

	closeAllWindows();
	/*
	will be set to false in childEvent, deleting windows is processed in the background, i.e.
	control leaves this function
	*//*
	blockSignals(true);
	//     m_deleting = true;

	QWidgetList windows( windowList() ); //copy just the pointers
	windows.setAutoDelete( false );

	const int count = windows.count();
	for (int i = count-1; i >= 0; --i) {
	    QWidget* w = windows.at(i);
	    windows.remove(i);
	    delete w;
	};

	blockSignals(false);*/
	//  m_deleting = false;
}

/** Enable / disable autoCascading */
void CMDIArea::setGUIOption( const MDIOption& newOption ) {
	//now do the initial action
	m_guiOption = newOption;

	triggerWindowUpdate();
}

/**  */
void CMDIArea::myTileVertical() {
	if (!isUpdatesEnabled() || !usableWindowList().count() ) {
		return;
	}

	QPtrList<QWidget> windows = usableWindowList();

	if ((windows.count() == 1) && windows.at(0)) {
		m_appCaption = windows.at(0)->caption();
		windows.at(0)/*->parentWidget()*/->showMaximized();
	} else {
		QWidget* active = activeWindow();
		QWorkspace::tile();
		active->setFocus();
	}
}

void CMDIArea::myTileHorizontal() {
	if (/*m_deleting || */!isUpdatesEnabled() || !usableWindowList().count() ) {
		return;
	}

	QPtrList<QWidget> windows = usableWindowList();

	if ((windows.count() == 1) && windows.at(0)) {
		m_appCaption = windows.at(0)->caption();
		windows.at(0)/*->parentWidget()*/->showMaximized();
	} else {

		QWidget* active = activeWindow();
		if (active->isMaximized()) {
			active->showNormal();
		}

		blockSignals(true);
		setUpdatesEnabled(false);
		int heightForEach = height() / windows.count();
		int y = 0;
		for ( int i = 0; i < int(windows.count()); ++i ) {
			QWidget *window = windows.at(i);
			// 			if ( window->isMaximized() ) { // prevent flicker
			window->parentWidget()->showNormal();
			qApp->sendPostedEvents( 0, QEvent::ShowNormal );
			// 			}

			const int preferredHeight = window->minimumHeight() + window->parentWidget()->baseSize().height();
			const int actHeight = QMAX(heightForEach, preferredHeight);

			window->parentWidget()->setGeometry( 0, y, width(), actHeight );
			y += actHeight;
		}

		setUpdatesEnabled(true);
		active->setFocus();
		blockSignals(false);
	}

}

/**  */
void CMDIArea::myCascade() {
	if (/*m_deleting || */!isUpdatesEnabled() || !usableWindowList().count() ) {
		return;
	}
	
	QPtrList<QWidget> windows = usableWindowList();
	if ( !windows.count() ) {
		return;
	}

	if ((windows.count() == 1) && windows.at(0)) {
		m_appCaption = windows.at(0)->caption();
		windows.at(0)->parentWidget()->showMaximized();
	} else {

		const int offsetX = 40;
		const int offsetY = 40;
		const int windowWidth =  width() - (windows.count()-1)*offsetX;
		const int windowHeight = height() - (windows.count()-1)*offsetY;

		int x = 0;
		int y = 0;

		QWidget* const active = activeWindow();
		if (active->isMaximized()) {
			active->showNormal();
		}

		blockSignals(true);
		setUpdatesEnabled(false);

// 		for ( int i(windows.count()-1); i >= 0; --i ) {
		for (int i(0); i < int(windows.count()); ++i) {
			QWidget* window = windows.at(i);
			if (window == active) { //leave out the active window which should be the top window
				continue;
			}
			
			window->setUpdatesEnabled(false);
			
 			window->parentWidget()->raise(); //make it the on-top-of-window-stack window to make sure they're in the right order
			window->parentWidget()->setGeometry(x, y, windowWidth, windowHeight);
 			x += offsetX;
 			y += offsetY;

			window->setUpdatesEnabled(true);
		}

		setUpdatesEnabled(true);

		active->parentWidget()->setGeometry(x, y, windowWidth, windowHeight);
		active->parentWidget()->raise();
		active->setActiveWindow();

		blockSignals(false);
	}
}

/*!
    \fn CMDIArea::emitWindowCaptionChanged()
 */
void CMDIArea::emitWindowCaptionChanged() {
	if (activeWindow()) {
		m_appCaption = activeWindow()->caption();
	}

	emit sigSetToplevelCaption(currentApplicationCaption());
}


/*!
    \fn CMDIArea::usableWindowsCount()
 */
QPtrList<QWidget> CMDIArea::usableWindowList() {
	QPtrList<QWidget> ret;

	QWidgetList windows = windowList();
	for ( QWidget* w = windows.first(); w; w = windows.next() ) {
		if (w->isMinimized() || w->isHidden()) { //not usable for us
			continue;
		}

		ret.append( w );
	}

	return ret;
}

bool CMDIArea::eventFilter( QObject *o, QEvent *e ) {
	QWidget* w = dynamic_cast<QWidget*>( o );
	bool ret = QWorkspace::eventFilter(o,e);

	if ( w && (e->type() == QEvent::WindowStateChange) ) {
// 		Q_ASSERT(o->inherits("CDisplayWindow"));

		if (o->inherits("CDisplayWindow") && ((w->windowState() & Qt::WindowMinimized) || w->isHidden())) { //window was minimized, trigger a tile/cascade update if necessary
			triggerWindowUpdate();
			ret = false;
		} 
		else if (!o->inherits("CDisplayWindow")) {
 			qDebug("bad mdi child classname: %s", o->className());
			o->dumpObjectInfo();
			o->dumpObjectTree();
		}
	}

	return ret; // standard event processing
}


/*!
    \fn CMDIArea::triggerWindowUpdate()
 */
void CMDIArea::triggerWindowUpdate() {
	// 	Q_ASSERT(!(m_deleting || !isUpdatesEnabled() || !usableWindowList().count()));
	if (/*m_deleting || */!isUpdatesEnabled() || !usableWindowList().count() ) {
		return;
	}

	switch (m_guiOption) {
		case autoTileVertical:
			QTimer::singleShot(0, this, SLOT(myTileVertical()));
			break;
		case autoTileHorizontal:
			QTimer::singleShot(0, this, SLOT(myTileHorizontal()));
			break;
		case autoCascade:
			QTimer::singleShot(0, this, SLOT(myCascade()));
			break;
		default:
			qDebug("CMDIArea::triggerWindowUpdate: no known m_guiType");
			break;
	}
}
