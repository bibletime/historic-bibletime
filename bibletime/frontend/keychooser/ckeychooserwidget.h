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

class cfx_btn;
class CLexiconKeyChooser;
class CMinMaxLayout;

class QIconSet;
class QVBoxLayout;
class QHBoxLayout;
class QComboBox;
class QToolButton;
class QStringList;
class QPoint;
class QMouseEvent;

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
	CKeyChooserWidget(QStringList *list=0, QWidget *parent=0, const char *name=0);
	/**
		* the constructor
		*/
	CKeyChooserWidget(int count=0, QWidget *parent=0, const char *name=0);	
	/**
		* the destructor
		*/
	~CKeyChooserWidget();
	/**
		*
		*/
	QComboBox* ComboBox;
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
	/**
		*
		*/	
	void reset(QStringList *list, int index, bool do_emit);	
 	/**
 		* Initializes this widget. We need this function because
 		* we have more than one constructor.
 		*/
 	virtual void init( QStringList* list );
  /**  */
  virtual void adjustSize();
  /** No descriptions */
  void setWhatsThis(const QString comboTip, const QString nextEntry, const QString scrollButton, const QString previousEntry);
  /** Sets the tooltips for the given entries using the parameters as text. */
  void setToolTips( const QString comboTip, const QString nextEntry, const QString scrollButton, const QString previousEntry);

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

protected:
	/**
		* indicates wheter we are resetting at the moment
		*/
	bool isResetting;
	/**
 		* Returns the icons set which contains the UP button.
 		*/
	QIconSet* getUpIconSet();
  /**
  	* Returns the icons set which contains the button used to change the current item.
  	*/
  QIconSet* getMoverIconSet();
  /**
  	*  Returns the icons set which contains the down button.
  	*/
  QIconSet* getDownIconSet();
	/**
		*
		*/
	QString oldKey;
	
protected slots: // Protected slots
  /**
  	* Is called when the return key was presed in the combobox.
  	*/
  void slotReturnPressed( const QString& );
	/**
  	* Is called when the current item of the combo boy was changed.
  	*/
  void slotComboChanged(int);

private:
	friend class CLexiconKeyChooser;
	
	/**
		* This is only used and created when we use the int-constructor
		*/
	QStringList*	m_list;
	/**
		*
		*/
	QHBoxLayout* m_mainLayout;
	/**
		*
		*/
	QVBoxLayout* m_buttonLayout;
	/**
		*
		*/
	QIconSet*	m_upIconSet;
	/**
		*
		*/
	QPixmap*	m_upPixmap;
	/**
		*
		*/
	QIconSet*	m_moverIconSet;
	/**
		*
		*/
	QPixmap*	m_moverPixmap;
	/**
		*
		*/
	QIconSet*	m_downIconSet;
	/**
		*
		*/
	QPixmap*	m_downPixmap;
};

#endif
