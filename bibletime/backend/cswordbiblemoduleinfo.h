/***************************************************************************
                          cswordbiblemoduleinfo.h  -  description
                             -------------------
    begin                : Thu Oct 5 2000
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

#ifndef CSWORDBIBLEMODULEINFO_H
#define CSWORDBIBLEMODULEINFO_H

//own includes
#include "cswordmoduleinfo.h"

//Qt includes
#include <qstringlist.h>

//Sword includes


/**
	* This is the CModuleInfo imlementation for Bible modules managed by Sword.
	*	
	*	@short Implementation for Sword Bibles
  *	@author The BibleTime team
  * @version $Id$
  */
class CSwordBibleModuleInfo : public CSwordModuleInfo  {
public:	
	enum Testament {
		OldTestament = 1,
		NewTestament = 2
	};
		
	/**
	* The constructor of this class
	*/
	CSwordBibleModuleInfo( SWModule* module  );
	CSwordBibleModuleInfo( const CSwordBibleModuleInfo& m );
	/**
	* The destructor of this class
	*/
	~CSwordBibleModuleInfo();
  /**
 	* Returns the number of avalable verses for the given chapter and book.
 	*
 	* @param book The book we should use
 	* @param chapter The chapter we should use
 	* @return The number of verses for the given book and chapter
 	*/
  virtual const unsigned int verseCount( const unsigned int book, const unsigned int chapter );
  /**
 	* @return The number of available chapters of the given book.
 	* @return The number of chapters for the given book
 	*/
  virtual const unsigned int chapterCount( const unsigned int book );
  /** Return all book of this module.
  * @return A QStringList containing the books which are available in this module.
  */
  virtual QStringList* books();
  /**
  * Reimplementation, Returns the type
  */
  virtual const CSwordModuleInfo::ModuleType type() const;
  /**
  * @return the book number, values starting with 1; 0 if not found
  */
  const unsigned int bookNumber(const QString &book);
  /**
  * Returns true if his module has the text of desired type of testament
  */
  const bool hasTestament( CSwordBibleModuleInfo::Testament );
  virtual CSwordModuleInfo* clone();

private:
	QStringList*	m_bookList;	//This booklist is cached
	QString m_cachedLocale;
	short int m_hasOT;
	short int m_hasNT;
};

inline const CSwordModuleInfo::ModuleType CSwordBibleModuleInfo::type() const {
	return CSwordModuleInfo::Bible;
}


#endif
