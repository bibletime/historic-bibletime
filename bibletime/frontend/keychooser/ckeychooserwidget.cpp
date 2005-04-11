/********* Read the file LICENSE for license details. *********/
//BibleTime includes
#include "ckeychooserwidget.h"
#include "cscrollbutton.h"

//BibleTime frontend includes
#include "frontend/cbtconfig.h"

//Qt includes
#include <qlineedit.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qlistbox.h>
#include <qtoolbutton.h>
#include <qevent.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qtooltip.h>
#include <qrect.h>

const unsigned int WIDTH = 16;
const unsigned int ARROW_HEIGHT = 12;
const unsigned int MOVER_HEIGHT = 6;

CKCComboBox::CKCComboBox(bool rw,QWidget* parent,const char* name)
  : QComboBox(rw,parent,name)
{
	setFocusPolicy(QWidget::WheelFocus);
  if (lineEdit()) {
  	installEventFilter( lineEdit() );
  }
}

/** Reimplementation. */
bool CKCComboBox::eventFilter( QObject *o, QEvent *e ){
	if (e->type() == QEvent::FocusOut) {
 		QFocusEvent* f = static_cast<QFocusEvent*>(e);
 		
		if (o == lineEdit() && f->reason() == QFocusEvent::Tab) {
 	    int index = listBox()->index( listBox()->findItem(currentText()) );
	    if (index == -1) {
				index = 0;// return 0 if not found
			}
	  	setCurrentItem( index );
	    emit focusOut( index );
			
			return false;
 	  }
	  else if (f->reason() == QFocusEvent::Popup) {
			return false;
		}
	  else if (f->reason() == QFocusEvent::ActiveWindow) {
			emit activated(currentText());
			return false;
		}
 	  else if (f->reason() == QFocusEvent::Mouse) {
 			emit activated(currentText());
  		return false;
 		}		
	  else if (o == this) {
			emit activated(currentText());
			return false;
		}
	}
  
	return QComboBox::eventFilter(o,e);	
}

/** Scrolls in the list if the wheel of the mouse was used. */
void CKCComboBox::wheelEvent( QWheelEvent* e ) {
	return QComboBox::wheelEvent(e);

	const signed int change = (int)((float)e->delta()/(float)120);
	int current = currentItem();
	
	if ((current+change >= 0) && (current+change<count()) ) {
		setCurrentItem(current+change);
		e->accept();
		emit activated( currentItem() );
	}
	else {
		e->ignore();
	}
}

//**********************************************************************************/

CKeyChooserWidget::CKeyChooserWidget(int count, const bool useNextPrevSignals,  QWidget *parent, const char *name) : QWidget(parent,name) {
	m_useNextPrevSignals = useNextPrevSignals;
	for (int index=1; index <= count; index++)
		m_list.append( QString::number(index) );	
	init();
	reset(m_list,0,false);
};

CKeyChooserWidget::CKeyChooserWidget(QStringList *list, const bool useNextPrevSignals, QWidget *parent, const char *name ) : QWidget(parent,name) {
	m_useNextPrevSignals = useNextPrevSignals;		
	if (list) {
		m_list = *list;//copy the items of list
	}
	else {
		m_list.clear();
	}
	
	init();
	reset(m_list,0,false);	
}

void CKeyChooserWidget::changeCombo(int i){
	if (!isUpdatesEnabled()) {
		return;
	}
	
	setUpdatesEnabled(false);
	
	int current = comboBox()->currentItem();

	//index of highest Item
	const int count = comboBox()->count()-1;
	int j = current + i;
	if (i > 0){
		if (j <= count) {
			comboBox()->setCurrentItem(j);
		}
		else {
			comboBox()->setCurrentItem(count);
		}
	}
	else if (i < 0){
		if (j>=0) {
			comboBox()->setCurrentItem(j);
		}
		else {
			comboBox()->setCurrentItem(0);
		}
	}
	
	if (!isResetting && !btn_fx->isLocked() && (current != comboBox()->currentItem())) {
		emit changed(comboBox()->currentItem());
	}
		
	setUpdatesEnabled(true);		
}

void CKeyChooserWidget::reset(const int count, int index, bool do_emit){
	if (!isUpdatesEnabled())
		return;

	m_list.clear();
	for (int i=1; i <= count; i++) { //TODO: CHECK
		m_list.append( QString::number(i) );
	}
	
	reset(&m_list,index,do_emit);
}

void CKeyChooserWidget::reset(QStringList& list, int index, bool do_emit){
	if (!isUpdatesEnabled())
		return;
	
	m_list = list;
	reset(&m_list,index,do_emit);
	
	isResetting = false;
}


void CKeyChooserWidget::reset(QStringList *list, int index, bool do_emit){
	if (isResetting || !isUpdatesEnabled())
		return;
	isResetting = true;	
	
	oldKey = QString::null;

//  m_comboBox->setUpdatesEnabled(false);
  //DON'T REMOVE THE HIDE: Otherwise QComboBox's sizeHint() function won't work properly
  m_comboBox->hide();
  m_comboBox->clear(); 
	if (list) {
		m_comboBox->insertStringList(*list);
  }

  if (!list || (list && !list->count())) { //nothing in the combobox
		btn_up->setEnabled( true );
		btn_fx->setEnabled( true );
		btn_down->setEnabled( true );	
		setEnabled(false);
	}
	else if (!isEnabled()) { //was disabled
		setEnabled(true);	
		const bool enableButtons = list && (list->count()>=1);
		btn_up->setEnabled( enableButtons );
		btn_fx->setEnabled( enableButtons );
		btn_down->setEnabled( list && (list->count()>1) );
	}

  if (list->count()) {
  	m_comboBox->setCurrentItem(index);
  }
	if (do_emit) {
		emit changed(m_comboBox->currentItem());				
	}

  const QSize dummySize = m_comboBox->sizeHint(); //without this function call the combo box won't be properly sized!
  //DON'T REMOVE OR MOVE THE show()! Otherwise QComboBox's sizeHint() function won't work properly!
  m_comboBox->show();

//  m_comboBox->setFont( m_comboBox->font() );
//  m_comboBox->setUpdatesEnabled(true);

	isResetting = false;	
}

void CKeyChooserWidget::lock(void){
	comboBox()->setEditable(false);
	oldKey = comboBox()->currentText();
}

void CKeyChooserWidget::unlock(void){
	comboBox()->setEditable(true);
	comboBox()->setEditText(comboBox()->text(comboBox()->currentItem()));
	if (comboBox()->currentText() != oldKey) {
		emit changed(comboBox()->currentItem());
	}
}

/** Initializes this widget. We need this function because we have more than one constructor. */
void CKeyChooserWidget::init(){
	oldKey = QString::null;
	btn_up = btn_down = btn_fx = 0;

	setFocusPolicy(QWidget::WheelFocus);			
			
	m_comboBox = new CKCComboBox( true, this );
	m_comboBox->setAutoCompletion( true );
	m_comboBox->setInsertionPolicy(QComboBox::NoInsertion);
	m_comboBox->setFocusPolicy(QWidget::WheelFocus);
	
	m_mainLayout = new QHBoxLayout( this );
	m_mainLayout->addWidget(m_comboBox,5);
  
	QVBoxLayout* m_buttonLayout = new QVBoxLayout();	
	m_buttonLayout->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);

	btn_up = new QToolButton( UpArrow, this, "btn_up" );	
	btn_up->setFixedSize(WIDTH, ARROW_HEIGHT);
	btn_up->setFocusPolicy(QWidget::NoFocus);	
	
	btn_fx = new CScrollButton( this, "btn_fx" );
	btn_fx->setFixedSize(WIDTH, MOVER_HEIGHT);
	btn_fx->setFocusPolicy(QWidget::NoFocus);	
	
	btn_down = new QToolButton( DownArrow, this, "btn_down" );	
	btn_down->setFixedSize(WIDTH, ARROW_HEIGHT);
	btn_down->setFocusPolicy(QWidget::NoFocus);	
	
	m_buttonLayout->addWidget( btn_up,0 );	
	m_buttonLayout->addWidget( btn_fx,0 );	
	m_buttonLayout->addWidget( btn_down,0 );
	
	m_mainLayout->addLayout( m_buttonLayout );
	m_mainLayout->addSpacing(2);

	setTabOrder(m_comboBox, 0);
		
// signals and slots connections
	if (m_useNextPrevSignals) {
		connect(btn_up, SIGNAL(clicked()), SIGNAL(prev_requested()) );	
		connect(btn_down, SIGNAL(clicked()), SIGNAL(next_requested()) );
	}
	else {
		connect(btn_up, SIGNAL(clicked()), SLOT(previous()) );	
		connect(btn_down, SIGNAL(clicked()), SLOT(next()) );	
	}
	
	connect(btn_fx, SIGNAL(lock()), SLOT(lock()) );
	connect(btn_fx, SIGNAL(unlock()), SLOT(unlock()) );
	connect(btn_fx, SIGNAL(change_requested(int)), SLOT(changeCombo(int)) );
	
	connect(m_comboBox, SIGNAL(activated(int)), SLOT(slotComboChanged(int)));
	connect(m_comboBox, SIGNAL(activated(const QString&)), SLOT(slotReturnPressed(const QString&)));
 	connect(m_comboBox, SIGNAL(focusOut(int)), SIGNAL(focusOut(int)));	
		
	isResetting = false;
}

/** Is called when the return key was presed in the combobox. */
void CKeyChooserWidget::slotReturnPressed( const QString& text){
	for (int index=0; index < comboBox()->count(); index++) {
		if (comboBox()->text(index) == text) {
			if (/*!oldKey.isNull() &&*/ text != oldKey)	//if the key has changed
				emit changed(index);
			break;
		}
	}
}

/** Is called when the current item of the combo box was changed. */
void CKeyChooserWidget::slotComboChanged(int index){
//	qWarning("CKeyChooserWidget::slotComboChanged(int index)");
	if (!isUpdatesEnabled())
		return;
	setUpdatesEnabled(false);	
	
	const QString key = comboBox()->text( index );
	if (oldKey.isNull() || (oldKey != key))
		emit changed(index);
	oldKey = key;

	setUpdatesEnabled(true);		
}

/** Sets the tooltips for the given entries using the parameters as text. */
void CKeyChooserWidget::setToolTips( const QString comboTip, const QString nextEntryTip, const QString scrollButtonTip, const QString previousEntryTip){
//  qWarning("add tooltip heklps: %s, %s, %s, %s", comboTip.latin1(), nextEntryTip.latin1(), scrollButtonTip.latin1(), previousEntryTip.latin1());

  QToolTip::add(comboBox(),comboTip);
	QToolTip::add(btn_fx,  scrollButtonTip);

	QToolTip::add(btn_down, nextEntryTip);
	QToolTip::add(btn_up,	previousEntryTip);
}

/** Sets the current item to the one with the given text */
bool CKeyChooserWidget::setItem( const QString item ){
	bool ret = false;
	const int count = comboBox()->count();
	for (int i = 0; i < count; ++i) {
		if (comboBox()->text(i) == item) {
			comboBox()->setCurrentItem(i);
			ret = true;
			break;
		}
	}
	if (!ret)
		comboBox()->setCurrentItem(-1);
	return ret;
}

/** Jump to the next entry. */
void CKeyChooserWidget::next(){
	if (comboBox()->currentItem() != comboBox()->count()-1) {// not last entry
		comboBox()->setCurrentItem( comboBox()->currentItem()+1 );
		emit changed(comboBox()->currentItem());
	}
}

/** Jump to the previous entry. */
void CKeyChooserWidget::previous(){
	if (comboBox()->currentItem() != 0) {// not last entry
		comboBox()->setCurrentItem( comboBox()->currentItem()-1 );
		emit changed(comboBox()->currentItem());
	}
}

QComboBox* CKeyChooserWidget::comboBox(){
	return m_comboBox;
}

