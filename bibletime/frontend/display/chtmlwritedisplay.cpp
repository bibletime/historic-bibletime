/***************************************************************************
                          chtmlwritedisplay.cpp  -  description
                             -------------------
    begin                : Fre Nov 29 2002
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

#include "chtmlwritedisplay.h"
#include "frontend/displaywindow/cwritewindow.h"
#include "frontend/cresmgr.h"

//KDE includes
#include <kaction.h>
#include <ktoolbar.h>
#include <klocale.h>
#include <kfontcombo.h>
#include <kcolorbutton.h>

CHTMLWriteDisplay::CHTMLWriteDisplay(CWriteWindow* parentWindow, QWidget* parent)
  : CPlainWriteDisplay(parentWindow,parent)
{
//  qWarning("constructor of CHTMLWriteDisplay");
  setTextFormat(Qt::RichText);
}

CHTMLWriteDisplay::~CHTMLWriteDisplay(){
}

//void CHTMLWriteDisplay::print( const CDisplay::TextPart ) {
//
//};
//
//const bool CHTMLWriteDisplay::isModified() const {
//  return QTextEdit::isModified();
//};
//
//void CHTMLWriteDisplay::setModified( const bool modified ) {
//  QTextEdit::setModified(modified);
//};

const QString CHTMLWriteDisplay::plainText() {
  return QTextEdit::text();
};

//QPopupMenu* CHTMLWriteDisplay::createPopupMenu( const QPoint& pos ) {
//
//};
//
//QPopupMenu* CHTMLWriteDisplay::createPopupMenu() {
//
//}

void CHTMLWriteDisplay::toggleBold() {
  setBold( m_actions.bold->isChecked() );
};

void CHTMLWriteDisplay::toggleItalic() {
  setItalic( m_actions.italic->isChecked() );
};

void CHTMLWriteDisplay::toggleUnderline() {
  setUnderline( m_actions.underline->isChecked() );
};


void CHTMLWriteDisplay::alignLeft() {
  setAlignment(Qt::AlignLeft);
};

void CHTMLWriteDisplay::alignCenter() {
  setAlignment(Qt::AlignHCenter);
};

void CHTMLWriteDisplay::alignRight() {
  setAlignment(Qt::AlignRight);
};

//void CHTMLWriteDisplay::alignJustify() {
//  setAlignment(Qt::AlignJustify);
//};



/** The text's alignment changed. Enable the right buttons. */
void CHTMLWriteDisplay::slotAlignmentChanged( int a ){
  bool alignLeft = false;
  bool alignCenter = false;
  bool alignRight = false;
//  bool alignJustify = false;

  if (a & Qt::AlignLeft) {
    alignLeft = true;
  }
  else if ((a & Qt::AlignHCenter) || (a & Qt::AlignCenter)) {
    alignCenter = true;
  }
  else if (a & Qt::AlignRight) {
    alignRight = true;
  }
//  else if (a & Qt::AlignJustify) {
//    alignJustify = true;
//  }
  else {
    alignLeft = true;
    qWarning("unknown alignment %i", a);
  }

  m_actions.alignLeft->setChecked( alignLeft );
  m_actions.alignCenter->setChecked( alignCenter );
  m_actions.alignRight->setChecked( alignRight );    
//  m_actions.alignJustify->setChecked( alignJustify );
}


/** Is called when a new color was selected. */
void CHTMLWriteDisplay::slotColorSelected( const QColor& c){
  setColor( c );
}

/** Is called when a text with another color was selected. */
void CHTMLWriteDisplay::slotColorChanged(const QColor& c){
  m_colorButton->setColor(c);
}


void CHTMLWriteDisplay::slotFontChanged( const QFont& font ) {
  m_actions.fontChooser->setFont( font.family() );
  m_actions.fontSizeChooser->setFontSize( font.pointSize() );

  m_actions.bold->setChecked( font.bold() );
  m_actions.italic->setChecked( font.italic() );
  m_actions.underline->setChecked( font.underline() );
};

void CHTMLWriteDisplay::setupToolbar(KToolBar * bar, KActionCollection * actions) {
  m_actions.fontChooser = new KFontAction( i18n("Choose a font"),
    CResMgr::displaywindows::writewindow::underlinedText::icon,
    CResMgr::displaywindows::writewindow::underlinedText::accel,
    actions
  );
  m_actions.fontChooser->setToolTip( CResMgr::displaywindows::writewindow::underlinedText::tooltip );
  m_actions.fontChooser->setWhatsThis( CResMgr::displaywindows::writewindow::underlinedText::whatsthis );
  m_actions.fontChooser->plug(bar);
  connect(m_actions.fontChooser, SIGNAL(activated(const QString&)), this, SLOT(setFamily(const QString&)));


  m_actions.fontSizeChooser = new KFontSizeAction( i18n("Choose a font size"),
    CResMgr::displaywindows::writewindow::underlinedText::icon,
    CResMgr::displaywindows::writewindow::underlinedText::accel,
    actions
  );
  m_actions.fontSizeChooser->setToolTip( CResMgr::displaywindows::writewindow::underlinedText::tooltip );
  m_actions.fontSizeChooser->setWhatsThis( CResMgr::displaywindows::writewindow::underlinedText::whatsthis );
  m_actions.fontSizeChooser->plug(bar);
  connect(m_actions.fontSizeChooser, SIGNAL(fontSizeChanged(int)), this, SLOT(setPointSize(int)));

  m_colorButton = new KColorButton(bar);
  connect(m_colorButton, SIGNAL(changed(const QColor&)), this, SLOT(slotColorSelected(const QColor&)));
  bar->insertWidget(50, m_colorButton->sizeHint().width(), m_colorButton);

  (new KActionSeparator())->plug(bar); //seperate font options from formatting buttons
  
 	m_actions.bold = new KToggleAction( i18n("Bold"),
    CResMgr::displaywindows::writewindow::boldText::icon,
    CResMgr::displaywindows::writewindow::boldText::accel,
    this, SLOT( toggleBold()  ),
    actions
  );
  m_actions.bold->setToolTip( CResMgr::displaywindows::writewindow::boldText::tooltip );
  m_actions.bold->setWhatsThis( CResMgr::displaywindows::writewindow::boldText::whatsthis );
  m_actions.bold->plug(bar);

 	m_actions.italic = new KToggleAction( i18n("Italic"),
    CResMgr::displaywindows::writewindow::italicText::icon,
    CResMgr::displaywindows::writewindow::italicText::accel,
    this, SLOT( toggleItalic()  ),
    actions
  );
  m_actions.italic->setToolTip( CResMgr::displaywindows::writewindow::italicText::tooltip );
  m_actions.italic->setWhatsThis( CResMgr::displaywindows::writewindow::italicText::whatsthis );
  m_actions.italic->plug(bar);


 	m_actions.underline = new KToggleAction( i18n("Underline"),
    CResMgr::displaywindows::writewindow::underlinedText::icon,
    CResMgr::displaywindows::writewindow::underlinedText::accel,
    this, SLOT( toggleUnderline()  ),
    actions
  );
  m_actions.underline->setToolTip( CResMgr::displaywindows::writewindow::underlinedText::tooltip );
  m_actions.underline->setWhatsThis( CResMgr::displaywindows::writewindow::underlinedText::whatsthis );
  m_actions.underline->plug(bar);


  (new KActionSeparator())->plug(bar); //seperate formatting from alignment buttons
  
  
 	m_actions.alignLeft = new KToggleAction( i18n("Left"),
    CResMgr::displaywindows::writewindow::alignLeft::icon,
    CResMgr::displaywindows::writewindow::alignLeft::accel,
    this, SLOT( alignLeft()  ),
    actions
  );
  m_actions.alignLeft->setToolTip( CResMgr::displaywindows::writewindow::alignLeft::tooltip );
  m_actions.alignLeft->setWhatsThis( CResMgr::displaywindows::writewindow::alignLeft::whatsthis );
  m_actions.alignLeft->plug(bar);  

 	m_actions.alignCenter = new KToggleAction( i18n("Center"),
    CResMgr::displaywindows::writewindow::alignCenter::icon,
    CResMgr::displaywindows::writewindow::alignCenter::accel,
    this, SLOT( alignCenter()  ),
    actions
  );
  m_actions.alignCenter->setToolTip( CResMgr::displaywindows::writewindow::alignCenter::tooltip );
  m_actions.alignCenter->setWhatsThis( CResMgr::displaywindows::writewindow::alignCenter::whatsthis );
  m_actions.alignCenter->plug(bar);

 	m_actions.alignRight = new KToggleAction( i18n("Right"),
    CResMgr::displaywindows::writewindow::alignRight::icon,
    CResMgr::displaywindows::writewindow::alignRight::accel,
    this, SLOT( alignRight()  ),
    actions
  );
  m_actions.alignRight->setToolTip( CResMgr::displaywindows::writewindow::alignRight::tooltip );
  m_actions.alignRight->setWhatsThis( CResMgr::displaywindows::writewindow::alignRight::whatsthis );
  m_actions.alignRight->plug(bar);

// 	m_actions.alignJustify = new KToggleAction( i18n("Justify"),
//    CResMgr::displaywindows::writewindow::alignJustify::icon,
//    CResMgr::displaywindows::writewindow::alignJustify::accel,
//    this, SLOT( alignJustify()  ),
//    actions
//  );
//  m_actions.alignJustify->setToolTip( CResMgr::displaywindows::writewindow::alignJustify::tooltip );
//  m_actions.alignJustify->setWhatsThis( CResMgr::displaywindows::writewindow::alignJustify::whatsthis );
//  m_actions.alignJustify->plug(bar);


  connect(this, SIGNAL(currentFontChanged(const QFont&)), SLOT(slotFontChanged(const QFont&)));
  connect(this, SIGNAL(currentAlignmentChanged(int)), SLOT(slotAlignmentChanged(int)));
  connect(this, SIGNAL(currentColorChanged(const QColor&)), SLOT(slotColorChanged(const QColor&)));          
}