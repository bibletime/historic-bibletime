/***************************************************************************
                       csearchdialogtext.cpp  -  description
                             -------------------
    begin                : Mit Jan 19 12:57:15 CET 2000
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


//BibleTime includes
#include "csearchdialogtext.h"
#include "csearchdialogscope.h"
#include "../../structdef.h"
#include "../../backend/sword_backend/cswordmodulesearch.h"
#include "../keychooser/ckeychooser.h"

//QT includes
#include <qpixmap.h>
#include <qlayout.h>
#include <qvbuttongroup.h>
#include <qhbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlineedit.h>

//KDE includes
#include <kiconloader.h>
#include <kprogress.h>
#include <klocale.h>


CSearchDialogText::CSearchDialogText(CImportantClasses *importantClasses, QWidget *parent, const char *name)
						: QWidget(parent, name, 48)
{
	m_importantClasses = importantClasses;
	
	/* the first main Group - search text, options etc.*/
	QGroupBox *textBox = new QGroupBox(2,Qt::Vertical,i18n("Search properties"),this,"textBox");

	editSearchText = new QLineEdit(textBox, "LineEdit_1");
	editSearchText->setFocusPolicy(QWidget::StrongFocus);
	editSearchText->setFrame( true );
	editSearchText->setAlignment( AlignLeft );
	
	/* manages the radio buttons next to the checkbox (horizontal)*/
	QHBox *settingsBox = new QHBox(textBox,"settingsBox");
	/* manages the radio buttons (vertical)*/
	QVBox *radioBox = new QVBox(settingsBox,"radioBox");

	radioMultipleWords = new QRadioButton( radioBox,"RadioButton_1");
	radioMultipleWords->setFocusPolicy(QWidget::TabFocus);
	radioMultipleWords->setText( i18n( "Multiple Words" ) );
	radioMultipleWords->setAutoResize( true );
	radioMultipleWords->setChecked( true );

	radioExactSearch = new QRadioButton(radioBox, "RadioButton_2");
	radioExactSearch->setFocusPolicy(QWidget::TabFocus);
	radioExactSearch->setText( i18n( "Exact Search" ) );
	radioExactSearch->setAutoResize( true );

	radioRegularExpression = new QRadioButton(radioBox, "RadioButton_3");
	radioRegularExpression->setFocusPolicy(QWidget::TabFocus);
	radioRegularExpression->setText( i18n( "Regular Expression" ) );
	radioRegularExpression->setAutoResize( true );
	
	/* this widget is hidden - it is only used for exclusive radio button management*/
	QVButtonGroup *bgroup1 = new QVButtonGroup();
	bgroup1->insert(radioMultipleWords); //they are automatically exclusive
	bgroup1->insert(radioExactSearch);
	bgroup1->insert(radioRegularExpression);

	checkCaseSensitive = new QCheckBox(settingsBox, "CheckBox_1");
	checkCaseSensitive->setFocusPolicy(QWidget::TabFocus);
	checkCaseSensitive->setText( i18n( "Case Sensitive" ) );
	
  QGroupBox *scopeBox = new QGroupBox(2,Qt::Horizontal,i18n("Search scope"),this,"scopeBox");
  scopeChooser = new CSearchDialogScopeChooser( scopeBox, "scopeChooser");
	
	/*the third main groupbox - progress indication */
	QGroupBox *progressBox = new QGroupBox(2,Qt::Horizontal,i18n("Search progress"),this,"progressBox");

	/* manages the 2 buttons (vertically)*/
	QVBox *buttonBox = new QVBox(progressBox,"currentBox");

	(void)new QLabel(editSearchText,i18n("Current module:"),buttonBox);
	(void)new QLabel(editSearchText,i18n("Overall:"),buttonBox);

	/* manages the 2 bars (vertically) */
	QVBox *barBox = new QVBox(progressBox,"overallBox");
	currentProgressBar = new KProgress(0,100,0,Horizontal,barBox, "currentProgressBar");
	overallProgressBar = new KProgress(0,100,0,Horizontal,barBox, "overallProgressBar");

	/* the main layout for the 3 groupboxes (vertical)*/
	QVBoxLayout* layout_1 = new QVBoxLayout( this );
	layout_1->setAutoAdd(false);
	
	textBox->setFixedHeight( textBox->sizeHint().height() );	
	layout_1->addWidget( textBox,1, Qt::AlignTop );
	
	layout_1->addWidget( scopeBox, Qt::AlignVCenter);
	scopeBox->setFixedHeight( scopeBox->sizeHint().height() );	
	
	progressBox->setFixedHeight( progressBox->sizeHint().height() );	
	layout_1->addWidget( progressBox,1,Qt::AlignBottom );
}

const QString CSearchDialogText::getText() const {
	return editSearchText->text();
}

void CSearchDialogText::setText(const QString text){
	editSearchText->setText(text);
}

const bool CSearchDialogText::isCaseSensitive() {
	return checkCaseSensitive->isChecked();
}

const int CSearchDialogText::getSearchType() {
	int ret = CSwordModuleSearch::multipleWords;	//"multiple words" is standard
	if (radioExactSearch->isChecked()) {
		ret = CSwordModuleSearch::exactPhrase;
	}
	else if (radioRegularExpression->isChecked()) {
		ret = CSwordModuleSearch::regExp;
	}
	return ret;
}

void CSearchDialogText::updateCurrentProgress(const unsigned short int percent){
	currentProgressBar->setValue(percent);
}

void CSearchDialogText::updateOverallProgress(const unsigned short int percent){
	overallProgressBar->setValue(percent);
}

/** resets the widgets. */
void CSearchDialogText::reset(){
	currentProgressBar->setValue(0);
	overallProgressBar->setValue(0);
	editSearchText->clear();
}
