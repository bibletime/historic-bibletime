/***************************************************************************
                          cprofile.h  -  description
                             -------------------
    begin                : Sun Jul 29 2001
    copyright            : (C) 2001 by The BibleTime team
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

#ifndef CPROFILE_H
#define CPROFILE_H

#include "cprofilewindow.h"

//Qt includes
#include <qlist.h>

/**	Manages one profile file. Provides functions to save and restore settings of the available display windows.
  *	@author The BibleTime team
  */
class CProfile {
public: 
	CProfile(const QString& fileName, const QString& name = QString::null);
	~CProfile();

  /**
  * Saves the profile to the file given in the constructor.
  * @param windows The list of windows available in the profile.
  */
  const bool save( QList<CProfileWindow> windows );
  /**
  * Saves the profile to the file given in the constructor.
  * @param windows The list of windows available in the profile.
  */
  const bool save();
  /**
  * Loads the profile from the file given in the constructor.
  * @return The list of profiled window which exist in the profile.
  */
 	QList<CProfileWindow> load();
  /**
  * Returns the name of this profile.
  */
  const QString& name();
  /**
  * Returns the filename used for this profile.
  */
  const QString& filename();
	
	
private:
	QList<CProfileWindow> m_profileWindows;
	QString m_name;
	QString m_filename;
};

#endif