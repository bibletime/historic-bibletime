/***************************************************************************
                          cswordbookmoduleinfo.cpp  -  description
                             -------------------
    begin                : Thu Jan 24 2002
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

//BibleTime includes
#include "cswordbookmoduleinfo.h"

//Sword includes
#include <treekeyidx.h>
#include <treekey.h>

#include <iostream>

using std::cout;

CSwordBookModuleInfo::CSwordBookModuleInfo( SWModule* module )
	: CSwordModuleInfo(module), m_depth(-1)
{
	//checking if it works as expected
//	qWarning("Book module: %s", module->Name());
//	if (TreeKeyIdx* treeKey = tree()) {
//		TreeKeyIdx root = *treeKey;
//  	root.root();
//		printTree(root, treeKey, 0);
//	}
//	else
//		qWarning("treeKey not valid");
//	qWarning("%s has depth: %i\n\n", module->Name(), depth());
}

CSwordBookModuleInfo::CSwordBookModuleInfo( const CSwordBookModuleInfo& module )
	: CSwordModuleInfo(module)
{
	m_depth = module.m_depth;
}	

CSwordBookModuleInfo::~CSwordBookModuleInfo(){
}

void CSwordBookModuleInfo::printTree(TreeKeyIdx treeKey, TreeKeyIdx* target, int level ){
	if (!target)
		target = &treeKey;

//  unsigned long currentOffset = target->getOffset();
  for (int i = 0; i < level; i++)
  	cout << "    ";
  cout << treeKey.getLocalName() << endl;
  if (treeKey.firstChild()) {
    printTree(treeKey, target, level+1);
    treeKey.parent();
  }
  if (treeKey.nextSibling())
		printTree(treeKey, target, level);
}

const int CSwordBookModuleInfo::depth() {
	if (m_depth == -1) {
  	TreeKeyIdx* treeKey = tree();
  	if (treeKey) {
    	TreeKeyIdx root = *treeKey;
    	root.root();
			computeDepth(root, treeKey, 0);
		}
	}
	return m_depth;
}

void CSwordBookModuleInfo::computeDepth(TreeKeyIdx treeKey, TreeKeyIdx* target, int level ){
	if (!target)
		target = &treeKey;

	if (level > m_depth)
		m_depth = level;

  if (treeKey.firstChild()) {
    computeDepth(treeKey, target, level+1);
    treeKey.parent();
  }
  if (treeKey.nextSibling())
		computeDepth(treeKey, target, level);

}

/** Returns a treekey filled with the structure of this module */
TreeKeyIdx* const  CSwordBookModuleInfo::tree() const {
	TreeKeyIdx* treeKey = dynamic_cast<TreeKeyIdx*>((SWKey*)*(module()));
	if (treeKey)
		return treeKey;
	return 0;
}
