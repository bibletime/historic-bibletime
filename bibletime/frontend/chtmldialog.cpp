/***************************************************************************
                          chtmldialog.cpp  -  description
                             -------------------
    begin                : Tue Jan 25 2000
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
#include "chtmldialog.h"
#include "ctoolclass.h"

//QT includes
#include <qlayout.h>
#include <qhbox.h>
#include <qfile.h>
#include <qtextbrowser.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qpushbutton.h>

//KDE includes
#include <kcombobox.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kapp.h>
#include <kurl.h>


CHTMLDialog::CHTMLDialog(QWidget* parent, const char *name )
	: QDialog ( parent, name, true, WStyle_MinMax | WStyle_DialogBorder /*| WStyle_Dialog */)
{
	init(false);
}

CHTMLDialog::CHTMLDialog(const QString& url, QWidget* parent, const char *name)
	: QDialog ( parent, name, true, WStyle_MinMax | WStyle_DialogBorder /*| WStyle_Dialog */)
{
	init();
	QString file = CToolClass::locatehtml(url);
	
	m_textBrowser->mimeSourceFactory()->setFilePath(file);
	m_textBrowser->setSource(file);
}

CHTMLDialog::~CHTMLDialog(){
}

/** Sets the content of the widget */
void CHTMLDialog::setText(const QString& text){
	m_textBrowser->setText(text);
}

/** Initializes this widget. */
void CHTMLDialog::init(const bool enableHistory){
	resize(600,400);		
	QVBoxLayout* layout = new QVBoxLayout(this, 5);
		
	m_textBrowser = new QTextBrowser(this);		
	KURL url(CToolClass::locatehtml("bibletime/helpdialog/index.docbook"));	
	m_textBrowser->mimeSourceFactory()->addFilePath(url.directory());
	url.cd(QString::fromLatin1(".."));
	m_textBrowser->mimeSourceFactory()->addFilePath(url.directory());
	url.cd(QString::fromLatin1(".."));
	m_textBrowser->mimeSourceFactory()->addFilePath(url.directory());
	url.cd(QString::fromLatin1(".."));
	m_textBrowser->mimeSourceFactory()->addFilePath(url.directory());
	
	
	if (enableHistory) {
		QHBox* toolBar = new QHBox(this, "history toolbar");
		
		QToolButton *button = 0;		
		button = new QToolButton(toolBar);
		button->setIconSet(SmallIcon("back"));
		button->setTextLabel(i18n("Backward"));		
		button->setEnabled(false);		
		button->setUsesTextLabel(true);
		button->setFixedSize(button->sizeHint());
		connect(button, SIGNAL(clicked()), m_textBrowser, SLOT(backward()));
		connect(m_textBrowser, SIGNAL(backwardAvailable(bool)),
			button, SLOT(setEnabled(bool)));		
    connect( m_textBrowser, SIGNAL( textChanged() ),	
	     this, SLOT( textChanged() ) );

		m_historyCombo = new KComboBox(toolBar);
		toolBar->setStretchFactor(m_historyCombo, 5);		
    connect( m_historyCombo, SIGNAL( activated( const QString & ) ),
	     this, SLOT( historyItemSelected( const QString & ) ) );
		
	     				
		button = new QToolButton(toolBar);		
		button->setEnabled(false);
		button->setTextLabel(i18n("Forward"));		
		button->setUsesTextLabel(true);		
		button->setIconSet(SmallIcon("forward"));
		button->setFixedSize(button->sizeHint());
		connect(button, SIGNAL(clicked()), m_textBrowser, SLOT(forward()));		
		connect(m_textBrowser, SIGNAL(forwardAvailable(bool)),
			button, SLOT(setEnabled(bool)));
			
		layout->addWidget(toolBar);		
	}		
	layout->addSpacing(5);	
	layout->addWidget(m_textBrowser,5);
	
	QHBoxLayout* hBox = new QHBoxLayout();
	QPushButton* closeButton = new QPushButton(this);
	closeButton->setText(i18n("Close"));
	connect(closeButton, SIGNAL(pressed()), this, SLOT(close()));
	hBox->addStretch(5);
	hBox->addWidget(closeButton,1);
	
	layout->addLayout(hBox);
}

/** Is called when an item of the history combo was chosen. */
void CHTMLDialog::historyItemSelected( const QString & file ){
	m_textBrowser->setSource(file);
}

/** Called when the content of the textbrowser was changed. */
void CHTMLDialog::textChanged(){
	if ( m_textBrowser->documentTitle().isNull() )
		setCaption( m_textBrowser->context() );
	else
		setCaption( m_textBrowser->documentTitle() ) ;

	QString selectedURL = caption();
	if ( !selectedURL.isEmpty() && m_historyCombo ) {
		bool exists = false;
		int i;
		for ( i = 0; i < m_historyCombo->count(); ++i ) {
			if ( m_historyCombo->text( i ) == selectedURL ) {
				exists = true;
				break;
			}
		}
		if ( !exists ) {
	    m_historyCombo->insertItem( selectedURL, -1 );
	    m_historyCombo->setCurrentItem( 0 );
		} else
	    m_historyCombo->setCurrentItem( i );
		selectedURL = QString::null;
	}	
}
