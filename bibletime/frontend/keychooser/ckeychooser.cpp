/***************************************************************************
                          ckeychooser.cpp  -  description
                             -------------------
    begin                : Wed Sep 20 2000
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

#include "ckeychooser.h"
#include "../../backend/cswordmoduleinfo.h"
#include "../../backend/cswordbiblemoduleinfo.h"
#include "../../backend/cswordcommentarymoduleinfo.h"
#include "../../backend/cswordlexiconmoduleinfo.h"

#include "clexiconkeychooser.h"
#include "cbiblekeychooser.h"
#include "cbookkeychooser.h"

CKeyChooser::CKeyChooser(ListCSwordModuleInfo, CSwordKey *, QWidget *parent, const char *name )
	: QWidget(parent, name){
}

CKeyChooser* CKeyChooser::createInstance(ListCSwordModuleInfo modules, CSwordKey *key, QWidget *parent){	
	if (!modules.count())
		return 0;

  CKeyChooser* ck = 0;
 	switch ( modules.first()->type() ){
 	  case CSwordModuleInfo::Commentary:	 //Bibles and commentaries uise the same key chooser
 	  case CSwordModuleInfo::Bible:
 	  	ck = new CBibleKeyChooser(modules,key,parent);
      break;
 	  case CSwordModuleInfo::Lexicon:
			ck = new CLexiconKeyChooser(modules,key,parent);	
      break;
 	  case CSwordModuleInfo::GenericBook:
 	  	ck = new CBookKeyChooser(modules,key,parent);
      break;
 	  default:
 	  	return 0;
 	}
  return ck;
}
