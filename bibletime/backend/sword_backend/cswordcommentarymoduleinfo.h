/***************************************************************************
                          cswordcommentarymoduleinfo.h  -  description
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

#ifndef CSWORDCOMMENTARYMODULEINFO_H
#define CSWORDCOMMENTARYMODULEINFO_H

//own includes
#include "cswordbiblemoduleinfo.h"

//Qt includes

//Sword includes

/**
  * @author The BibleTime team
  * @version $Id$
  */
class CSwordCommentaryModuleInfo : public CSwordBibleModuleInfo  {
public:
	CSwordCommentaryModuleInfo( CSwordBackend* backend, SWModule* module );
	~CSwordCommentaryModuleInfo();
  /**
  * No descriptions
  */
  virtual const CSwordModuleInfo::type getType() const;
};

/** No descriptions */
inline const CSwordModuleInfo::type CSwordCommentaryModuleInfo::getType() const{
	return CSwordModuleInfo::Commentary;
}

#endif
