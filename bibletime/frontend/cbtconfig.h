/***************************************************************************
                          cbtconfig.h  -  description
                             -------------------
    begin                : Mon Nov 19 2001
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

#ifndef CBTCONFIG_H
#define CBTCONFIG_H

#include <qstring.h>
#include <qcolor.h>
#include <qfont.h>
#include <qvaluelist.h>

#include "backend/cswordbackend.h"

//Forward declarations
class KAccel;

/**
 * This class is the interface to the config object of BibleTime
 * @author The BibleTime team
 */
class CBTConfig {
public:
	enum strings {
		bibletimeVersion,
		language,
		standardBible,
		standardCommentary,
		standardLexicon,
		standardHebrewStrongsLexicon,
		standardGreekStrongsLexicon,
		standardHebrewMorphLexicon,
		standardGreekMorphLexicon
	};
	enum fonts {
		standard,
		unicode
	};
	enum colors {
		textColor,
		backgroundColor,
		highlightedVerseColor,
		footnotesColor,
		strongsColor,
		morphsColor,
		jesuswordsColor,
		swordRefColor  //SWORD hyperlink
	};
	enum bools {
		firstSearchDialog,

		toolbar,
		mainIndex,

		autoTile,
		autoCascade,

		lexiconCache,

  	footnotes,
  	strongNumbers,
  	headings,
  	morphTags,
		lemmas,
		hebrewPoints,
		hebrewCantillation,
		greekAccents,
		textualVariants,

		lineBreaks,
		verseNumbers,
		scroll,

		tips,
		logo,
		restoreWorkspace,

    crashedLastTime,
    crashedTwoTimes    
	};
	enum intLists {
		splitterSizes
	};
	enum stringLists {
		searchCompletionTexts,
		searchTexts		
	};
	enum keys {
		bookWindow,
		bibleWindow,
		commentaryWindow,
		lexiconWindow,
		application
	};

  static const QString 		get( const CBTConfig::strings );
  static const QFont 			get( const CBTConfig::fonts );
  static const bool 			get( const CBTConfig::bools );
  static const QColor 		get( const CBTConfig::colors );
  static const QValueList<int>	get( const CBTConfig::intLists );
  static const QStringList			get( const CBTConfig::stringLists );

	

	static void set( const CBTConfig::strings, 	const QString value );
	static void set( const CBTConfig::fonts, 		const QFont value );
	static void set( const CBTConfig::bools, 		const bool value );
	static void set( const CBTConfig::colors, 	const QColor value );
	static void set( const CBTConfig::intLists,	const QValueList<int> value );
  static void set( const CBTConfig::stringLists, const QStringList value);


  static const CSwordBackend::FilterOptionsBool getFilterOptionDefaults();
  static const CSwordBackend::DisplayOptionsBool getDisplayOptionDefaults();

  static void setupAccel(const CBTConfig::keys type, KAccel* const accel);

private:
	static const QString getKey( const CBTConfig::strings );
	static const QString getKey( const CBTConfig::fonts );
	static const QString getKey( const CBTConfig::bools );
	static const QString getKey( const CBTConfig::colors );
	static const QString getKey( const CBTConfig::intLists );
	static const QString getKey( const CBTConfig::stringLists );

	static const QString 				 getDefault( const CBTConfig::strings );
	static const QFont	 				 getDefault( const CBTConfig::fonts );
	static const bool		 				 getDefault( const CBTConfig::bools );
	static const QColor	 				 getDefault( const CBTConfig::colors );
	static const QValueList<int> getDefault( const CBTConfig::intLists );
	static const QStringList		 getDefault( const CBTConfig::stringLists );
};


#endif
