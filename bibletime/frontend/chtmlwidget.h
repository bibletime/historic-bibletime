/***************************************************************************
                          chtmlwidget.h  -  description
                             -------------------
    begin                : Sun Feb 27 2000
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

#ifndef CHTMLWIDGET_H
#define CHTMLWIDGET_H

//BibleTime includes
#include "cpointers.h"
#include "backend/cswordbackend.h"

//#define private public // It's a really bad hack, but I don't know how to do it in another way
//#include "thirdparty/qt3stuff/qtextedit.h"
//#undef private

//Qt includes
#include <qwidget.h>
#include <qguardedptr.h>
#include <qtooltip.h>
#include <qtextedit.h>

class CHTMLDialog;
class CBiblePresenter;
class KPopupMenu;
class KAction;
class KToggleAction;
class KFontSizeAction;
class KToolBar;
class QToolButton;
class KFontAction;
class QComboBox;
class KColorButton;


/** This widget enhances the KHTMLWidget to support BibleTime's colour
	* management and to support verse references and the like.
  * @author The BibleTime Team
  */
class CHTMLWidget : public QTextEdit, public CPointers {
   Q_OBJECT
   friend class CHTMLDialog;

public:
	/**
	* The constructor of the CHTMLWidget class.
	* @param useColorsAndFonts Set this to false if you don't want to use the standard colorsa dnfonts defined in the optionsdialog of BibleTime
	*/
	CHTMLWidget(const bool useColorsAndFonts = true, QWidget *parent=0, const char *name=0);
	virtual ~CHTMLWidget();
  /**
 	* Reinitialize the colors, fonts etc.
 	*/
  virtual void refresh();
  /**
 	* Loads a HTML file in the widget.
 	*/
  virtual void setHTMLSource(const QString& url);
  /**
 	* Installes a menu which will popup if the right mouse button was pressed on an anchor.
 	*/
  virtual void installAnchorMenu( QPopupMenu* );
  /**
 	* Returns the current anchor.
 	*/
  virtual QString getCurrentAnchor();
  /**
  * No descriptions
  */
  void installPopup( QPopupMenu* popup);
  /**
  * Returns true if this HTML widget is read only, false if it's editable
  */
  virtual bool isReadOnly() const;
  /**
  * Sets the HTML widget ediatable or not
  */
  virtual void setReadOnly( const bool readOnly = true);
  /**
  * This function returns the edit toolbar used for the HTML widget.
	* The items of the toolbar are already connected to the correct slots.
	*/
  void createEditToolbar( KToolBar* bar );
  /**
  * Returns the document used by this widget
  */
//  virtual QTextDocument* getDocument() const;
  /**
  * Places the cursor at position pos
  */
//  virtual void placeCursor( const QPoint &pos, QTextCursor *c = 0);
  /**
  * Sets the list of modules used by the display window which uses this widget.
  */
  void setModules( ListCSwordModuleInfo& modules ) ;
  /**
  * Returns a list of modules which are used by the display window which uses this HTML widget.
  */
  ListCSwordModuleInfo& modules() const;

  QPoint m_pressedPos;

public slots: // Public slots
  /**
  * Saves the HTML page
  */
  void slotSaveAsHTML();
  /**
  * Saves the plain text page
  */
  void slotSaveAsText();
  /**
  * Copies the displayed document into the clipboard.
  */
  void copyDocument();
  /**
  * Sets the source of this widget.
  */
  void setSource(const QString& name);


protected: // Protected attributes
  /*
  * Initializes the colors of this widget (backgrouznd, links, text)
  */
  void initColors();
  /**
  * Initializes the fonts of the HTML-widget
  */
  void initFonts();
  /**
  * Initializes the connections to SIGNALs
  */
  void initConnections();
  /**
  * Initializes the view of this widget (childs, popups etc.)
  */
  void initView();
  /**
 	* Reimplementation.
 	*/
  virtual void contentsDragEnterEvent(QDragEnterEvent* e);
  /**
 	* Reimplementation.
 	*/
  virtual void contentsDragMoveEvent (QDragMoveEvent* e);
  /**
 	* Reimplementation.
 	*/
  virtual void contentsDropEvent(QDropEvent* e);
  /**
 	* Reimplementation.
 	*/
  virtual void contentsMousePressEvent(QMouseEvent* e);
  /**
 	* Reimplementation.
 	*/
  virtual void contentsMouseMoveEvent(QMouseEvent* e);
  /**
  * Returns true if the links are enabled.
  */
  virtual bool linksEnabled() const;

protected slots: // Protected slots
  /**
  * No descriptions
  */
  void slotSaveDocument();
  /**
  * No descriptions
  */
  void slotToggleBold();
  /**
  * No descriptions
  */
  void slotToggleItalic();
  /**
  * No descriptions	
  */
  void slotToggleUnderline();
	/**
	*
	*/
	void slotSetCurrentFont(const QString& font);	
  /**
  *
  */
  void slotSetCurrentFontSize( int );	
	/**
	*
	*/
	void slotSetCurrentColor( const QColor& color);
  /**
  * No descriptions
  */
  void slotCurrentColorChanged( const QColor& );
  /**
  * No descriptions
  */
  void slotCurrentFontChanged( const QFont& );
  /**
  * No descriptions
  */
  void slotCurrentAlignementChanged( int );
  /**
  * No descriptions
  */
  void slotAlignRight();
  /**
  * No descriptions
  */
  void slotAlignCenter();
  /**
  * No descriptions
  */
  void slotAlignLeft();
  /**
  * No descriptions
  */
  void slotAlignJustify();
  /**
  * No descriptions
  */
  void slotSelectAll();
	
private:
  virtual void emitLinkClicked( const QString& s);
  virtual void emitHighlighted( const QString& s);

	class ToolTip : public QToolTip, public CPointers {
		public:
			ToolTip(QWidget* parent);
			void maybeTip( const QPoint &pos);
	};
	ToolTip* m_toolTip;

	bool m_readOnly;
	bool m_selectedWord;
	QString m_anchor;


  QPopupMenu* m_popup;
  QGuardedPtr<QPopupMenu>	m_anchorMenu;
    		
	KAction* m_docDeleteAction;	
	KAction* m_docSaveAction;
	
	KToggleAction* m_boldAction;
	KToggleAction* m_italicAction;
	KToggleAction* m_underlineAction;
	
	KToggleAction* m_alignLeftAction;			
	KToggleAction* m_alignCenterAction;		
	KToggleAction* m_alignRightAction;	
	KToggleAction* m_alignJustifyAction;
	
	KFontSizeAction* m_fontSizeAction;
	KFontAction* m_fontAction;
	KColorButton* m_colorChooser;
	QPoint m_hoverPos;
	
	ListCSwordModuleInfo* m_moduleList;
			
signals: // Signals
  void sigDeleteDocument();
  void sigSaveDocument(const QString);
	/**
	* First parameter is the module, the second the key.
	*/
  void referenceClicked(const QString& module, const QString& key);
	/**
	* First parameter is the module, the second the key.
	*/
  void referenceDropped(const QString& key);
  void linkClicked(const QString&);
	void insertReference(const QString& reference);
};


#endif
