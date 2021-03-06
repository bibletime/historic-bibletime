/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#ifndef CSWORDMODULESEARCH_H
#define CSWORDMODULESEARCH_H

//BibleTime - backend
#include "cswordmoduleinfo.h"

//BibleTime - utils
#include "util/cpointers.h"

//Qt includes
#include <qptrlist.h>
#include <qstring.h>
#include <qsignal.h>

//System includes
#include <pthread.h>

//Sword includes
#include <listkey.h>

/**
 * CSwordModuleSearch manages the search on Sword modules. It manages the thread(s)
 * and manages the different modules.
  *
  * @author The BibleTime team
  * @version $Id$
  */

class CSwordModuleSearch: public CPointers {

public:
	CSwordModuleSearch();
	/**
	* The destructor of this class. It cleans uop memory before it's deleted.
	*/
	virtual ~CSwordModuleSearch();
	/**
	* Sets the text which should be search in the modules.
	*/
	void setSearchedText( const QString& );
	/**
	* Starts the search for the search text.
	*/
	const bool startSearch();
	/**
	* This function sets the modules which should be searched.
	*/
	void setModules( const ListCSwordModuleInfo& );
	/**
	* Sets the search scope.
	*/
	void setSearchScope( const sword::ListKey& scope );
	/**
	* Sets the seaech scope back.
	*/
	void resetSearchScope();
	/**
	* @return "true" if in the last search the searcher found items, if no items were found return "false"
	*/
	const bool foundItems() const;
	/**
	* Returns a copy of the used search scope.
	*/
	const sword::ListKey& searchScope() const;

	void connectFinished( QObject * receiver, const char * member );
	void searchFinished();

	/**
	* Returns true if all of the specified modules have indices already built.
	*/
	const bool modulesHaveIndices( const ListCSwordModuleInfo& );

protected:
	QString m_searchedText;
	sword::ListKey m_searchScope;
	ListCSwordModuleInfo m_moduleList;

	int m_searchOptions;

	bool m_foundItems;

private:
 	QSignal m_finishedSig;
	static CSwordModuleSearch* searcher;
};

#endif
