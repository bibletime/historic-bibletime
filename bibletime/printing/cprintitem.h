/***************************************************************************
                          cprintitem.h  -  description
                             -------------------
    begin                : Sun Aug 20 2000
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

#ifndef CPRINTITEM_H
#define CPRINTITEM_H

#include <qobject.h>
#include <qguardedptr.h>
#include <qlist.h>

class CSwordKey;
class CSwordModuleInfo;

class CStyle;
class CPrintItemList;
class CPrinter;

class QListViewItem;
class QListView;

class CPrintItem;
typedef QList<CPrintItem> printItemList;

/**
	* The class which implements the printable items.
  * @author The BibleTime team
  */
class CPrintItem {
public: 
	CPrintItem();
	~CPrintItem();
  /**
 	* Returns thestyle used by this item.
 	*/
  CStyle* getStyle() const;
  /**
	* Sets the style for this item.
 	*/
	void setStyle( CStyle* );
  /**
 	*
 	*/
  void setHeader( const QString& );
  /**
 	* Returns the text of the header.
 	*/
  const QString& getHeader() const;
  /**
 	* Sets the module text.
 	*/
  void setModuleText( const QString& );
  /**
 	* Returns the moduletext used by this item.
 	*/
  const QString& getModuleText();
  /**
 	* Sets the decsription.
 	*/
	void setDescription( const QString& );
  /**
  * Returns the description. Only valid for inserted bookmarks.
  */
  const QString& getDescription() const;
  /**
  * Sets the used module.
  */
  void setModule( CSwordModuleInfo* );
  /**
 	* Returns the used module.
 	*/
  CSwordModuleInfo* getModule() const;
  /**
 	* Returns the last covered key.
 	*/
  CSwordKey* getStopKey() const;
  /**
 	* Sets the end key.
 	*/
  void setStopKey( CSwordKey* );
  /**
 	* Sets the startkey.
 	*/
  void setStartKey( CSwordKey* );
  /**
 	* Returns the first key covered by this entry.
 	*/
  CSwordKey* getStartKey() const;
  /**
 	* Returns the listview item for this printitem.
 	*/
  QListViewItem* getListViewItem( CPrintItemList* );
  /**
 	* Updates the item.
 	*/
  void updateListViewItem();
  /**
	* Returns the used ListView item.
	*/
  QListViewItem* getListViewItem() const;
  /**
 	* Deletes the list view item.
 	*/
  void deleteListViewItem();
  /**
  * Draws the content of this item on the page.
  */
  void draw(QPainter* p, CPrinter* printer);

private: // Protected attributes
  void clearData();
  const QString& getHeaderText();

  CStyle* m_style;
  QString m_description;
  QString m_headerText;
  QString m_moduleText;
  CSwordKey*	m_startKey;
  CSwordKey*	m_stopKey;
  CSwordModuleInfo* m_module;
  QListViewItem* m_listViewItem;
};

#endif
