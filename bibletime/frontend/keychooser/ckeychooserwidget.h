/***************************************************************************
                          ckeychooserwidget.h  -  description
                             -------------------
    begin                : Tue Sep 19 2000
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

#ifndef CKEYCHOOSERWIDGET_H
#define CKEYCHOOSERWIDGET_H

#include <qwidget.h>
#include <qcombobox.h>

class cfx_btn;
class CLexiconKeyChooser;
//class CMinMaxLayout;

class QIconSet;
class QComboBox;
class QToolButton;
class QStringList;
class QPoint;
class QMouseEvent;
class QWheelEvent;

/*
* We use this class to contol the focus move in the combobox
* This class is used in the key chooser widgets
*/
class CKCComboBox : public QComboBox {
  Q_OBJECT

public:
  CKCComboBox(bool rw, QWidget * parent=0, const char * name=0 );
  /**
  * Returns the size this widget would like to have.
  */
//  virtual QSize sizeHint() const;

protected:
  /**
  * Reimplementation.
  */
  virtual bool eventFilter( QObject *o, QEvent *e );
  /**
  * Scrolls in the list if the wheel of the mouse was used.
  */
  virtual void wheelEvent( QWheelEvent* e);

signals:
  /**
  * Emitted when the user moves the focus away from the combo by pressing tab
  */
	void focusOut(int itemIndex);
};

/**
	* This class implements the KeyCooser Widget, which
	* consists of a @ref QComboBox, two normal ref @QToolButton
	* and a enhanced @ref cfx_btn
	*
  * @author The BibleTime team
  */
class CKeyChooserWidget : public QWidget  {
   Q_OBJECT
public:
	/**
	* the constructor
	*/
	CKeyChooserWidget(QStringList *list=0, const bool useNextPrevSignals = false, QWidget *parent=0, const char *name=0);
	/**
	* the constructor
	*/
	CKeyChooserWidget(int count=0, const bool useNextPrevSignals = false, QWidget *parent=0, const char *name=0);	
	/**
	* This function does clear the combobox, then fill in
	* the StringList, set the ComboBox' current item to index
	* and if do_emit is true, it will emit @ref #changed
	*
	* @param list the stringlist to be inserted
	* @param index the index that the combobox is to jump to
	* @param do_emit should we emit @ref #changed(int)
	*/	
	void reset(const int count, int index, bool do_emit);
	void reset(QStringList& list, int index, bool do_emit);
	void reset(QStringList *list, int index, bool do_emit);	
 	/**
 	* Initializes this widget. We need this function because
 	* we have more than one constructor.
 	*/
 	virtual void init();
  /**
  *
  */
  virtual void adjustSize();
  /**
  * Sets the QWhatsThis help for the different parts.
  */
  void setWhatsThis(const QString comboTip, const QString nextEntry, const QString scrollButton, const QString previousEntry);
  /**
  * Sets the tooltips for the given entries using the parameters as text.
  */
  void setToolTips( const QString comboTip, const QString nextEntry, const QString scrollButton, const QString previousEntry);
  /**
  * Sets the current item to the one with the given text
  */
  bool setItem( const QString item);
  /**
  * Return the combobox of this key chooser widget.
  */
  QComboBox* comboBox();

public slots:
	/**
	* is called to lock the combobox
	*/
	void lock();
	/**
	* is called to unlock the combobox
	*/
	void unlock();
	/**
	* is called to move the combobox to a certain index
	* @param index the index to jump to
	*/
	void changeCombo(int index);

signals:
	/**
	* is emittd to proceed to the next entry, which may
	*	require changes in other widgets also
	*/
	void next_requested(void);
	/**
	* see @ref #next_requested
	*/
	void prev_requested(void);
	/**
	* Is emitted if the widget changed, but
	* only if it is not locked or being reset
	*
	* @param the current ComboBox index
	*/
	void changed(int index);
	/**
	* Is emitted if the widget was left with a focus out event.
	* @param index The new index of the ComboBox
	*/
	void focusOut(int index);

protected:
	/**
	* indicates wheter we are resetting at the moment
	*/
	bool isResetting;
	/**
 	* Returns the icons set which contains the UP button.
 	*/
	QIconSet getUpIconSet();
  /**
  * Returns the icons set which contains the button used to change the current item.
  */
  QIconSet getMoverIconSet();
  /**
  *  Returns the icons set which contains the down button.
  */
  QIconSet getDownIconSet();
	/**
	*
	*/
	QString oldKey;
  /**
	*
	*/
	QToolButton* btn_up;
	/**
	*
	*/
	QToolButton* btn_down;
	/**
	*
	*/
	cfx_btn* btn_fx;
	
protected slots: // Protected slots
  /**
  * Is called when the return key was presed in the combobox.
  */
  void slotReturnPressed( const QString& );
	/**
  * Is called when the current item of the combo box was changed.
  */
  void slotComboChanged(int);

private:
	friend class CLexiconKeyChooser;
	QStringList	m_list;
	bool m_useNextPrevSignals;
	/**
	* Members should never be public!!
	*/
	CKCComboBox* m_comboBox;

private slots: // Private slots
  /**
  * Jump to the previous entry.
  */
  void previous();
  /**
  * Jump to the next entry.
  */
  void next();
};

#endif
