/***************************************************************************
                          coptionsdialog.cpp  -  description
                             -------------------
    begin                : Wed Jan 19 2000
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

#include "coptionsdialog.h"
#include "cprofile.h"

#include "backend/cswordbackend.h"
#include "backend/cswordmoduleinfo.h"
#include "backend/cdisplaytemplatemgr.h"
#include "backend/cdisplayrendering.h"

#include "frontend/displaywindow/cbiblereadwindow.h"
#include "frontend/displaywindow/ccommentaryreadwindow.h"
#include "frontend/displaywindow/clexiconreadwindow.h"
#include "frontend/displaywindow/cbookreadwindow.h"
#include "frontend/displaywindow/creadwindow.h"

#include "util/cresmgr.h"
#include "util/ctoolclass.h"

#include <stdio.h>
#include <stdlib.h>

//QT includes
#include <qhbox.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qdict.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qbuttongroup.h>
#include <qhbuttongroup.h>
#include <qradiobutton.h>
#include <qcolor.h>
#include <qtooltip.h>

#include <qstringlist.h>
#include <qinputdialog.h>
#include <qdir.h>

//KDE includes
#include <kapp.h>
#include <klocale.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kkeydialog.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <kiconloader.h>
#include <ktabctl.h>
#include <kapp.h>
#include <klistbox.h>
#include <kkeydialog.h>
#include <kaction.h>
#include <kconfigbase.h>
#include <kconfig.h>

//Sword includes
#include <localemgr.h>

using std::string;
using std::list;

COptionsDialog::COptionsDialog(QWidget *parent, const char *name, KAccel* accel )
	: KDialogBase(IconList, i18n("Preferences"), Ok | Cancel | Apply, Ok, parent, name, true, true, QString::null, QString::null, QString::null) {

	m_settings.keys.application.accel = accel;
	setIconListAllVisible(true);

	initStartup();
	initFonts();
	initSword();
	initAccelerators();
}

/**  */
void COptionsDialog::newDisplayWindowFontSelected(const QFont &newFont){
  CBTConfig::FontSettingsPair oldSettings = m_settings.fonts.fontMap[ m_settings.fonts.usage->currentText() ];
	m_settings.fonts.fontMap.replace( m_settings.fonts.usage->currentText(), CBTConfig::FontSettingsPair(oldSettings.first, newFont) );		
}

/** Called when the combobox contents is changed */
void COptionsDialog::newDisplayWindowFontAreaSelected(const QString& usage){
  useOwnFontClicked( m_settings.fonts.fontMap[usage].first );
  m_settings.fonts.useOwnFontBox->setChecked( m_settings.fonts.fontMap[usage].first );

  m_settings.fonts.fontChooser->setFont( m_settings.fonts.fontMap[usage].second );
}

/** Called if the OK button was clicked */
void COptionsDialog::slotOk(){
	saveAccelerators();
	saveDisplayStyle();
	saveFonts();
	saveSword();
	saveStartup();
	
	KDialogBase::slotOk();
  emit signalSettingsChanged( );
}

/*called if the apply button was clicked*/
void COptionsDialog::slotApply(){
	saveAccelerators();
	saveDisplayStyle();
	saveFonts();
	saveSword();
	saveStartup();

	KDialogBase::slotApply();
  emit signalSettingsChanged( );
}

/** Opens the page which contaisn the given part ID. */
const bool COptionsDialog::showPart( COptionsDialog::Parts /*ID*/ ){
	bool ret = false;
/*	switch (ID) {
		default:
			break;
	}*/
	return ret;
}

/** Initializes the startup section of the OD. */
void COptionsDialog::initStartup(){
	QFrame* page = addPage(i18n("Display"), QString::null, DesktopIcon(CResMgr::settings::startup::icon,32));
	QVBoxLayout* layout = new QVBoxLayout(page);
	layout->setSpacing( 5 );

  {//daily tips
		m_settings.startup.showTips = new QCheckBox(page);
		m_settings.startup.showTips->setText(i18n("Show tip of the day"));
		QToolTip::add(m_settings.startup.showTips, CResMgr::settings::startup::dailyTip::tooltip );	

		
		m_settings.startup.showTips->setChecked( CBTConfig::get(CBTConfig::tips) );
	}
	layout->addWidget(m_settings.startup.showTips);
	layout->addSpacing(5);
	
	{ //startup logo
		m_settings.startup.showLogo = new QCheckBox(page);
		m_settings.startup.showLogo->setText(i18n("Show startuplogo"));
		QToolTip::add(m_settings.startup.showLogo, CResMgr::settings::startup::showLogo::tooltip);		

		
		m_settings.startup.showLogo->setChecked(CBTConfig::get(CBTConfig::logo));			
		layout->addWidget(m_settings.startup.showLogo);
		layout->addSpacing(20);
	}		
	
	layout->addWidget(
		CToolClass::explanationLabel(page, i18n("Display templates"),
			i18n("Display templates define how text is displayed. Please choose a template you like.")
		)
	);
	
	layout->addSpacing( 5 );

	QHBoxLayout* hboxlayout = new QHBoxLayout();

	m_settings.displayStyle.styleChooser = new QComboBox( page ); //create first to enable buddy for label
	connect( m_settings.displayStyle.styleChooser, SIGNAL( activated( int ) ), 
		this, SLOT( updateStylePreview() ) );
	
	hboxlayout->addWidget(
		new QLabel(m_settings.displayStyle.styleChooser, i18n("Available display styles:"), page)
	);
	hboxlayout->addWidget( m_settings.displayStyle.styleChooser );
	hboxlayout->addStretch();
	layout->addLayout( hboxlayout );

	m_settings.displayStyle.stylePreview = new KHTMLPart(page);
	layout->addWidget(
		new QLabel(m_settings.displayStyle.stylePreview->view(), i18n("Style preview"), page)
		);
	layout->addWidget(m_settings.displayStyle.stylePreview->view());

	CDisplayTemplateMgr* tMgr = CPointers::displayTemplateManager();
	m_settings.displayStyle.styleChooser->insertStringList( tMgr->availableTemplates() );
	int i = m_settings.displayStyle.styleChooser->listBox()->index(
		m_settings.displayStyle.styleChooser->listBox()->findItem( CBTConfig::get(CBTConfig::displayStyle), Qt::CaseSensitive )
	);
	m_settings.displayStyle.styleChooser->setCurrentItem( i );
	
	updateStylePreview(); //render it
}

/** Init fonts section. */
void COptionsDialog::initFonts(){
	QFrame* page = addPage(i18n("Languages"), QString::null, DesktopIcon(CResMgr::settings::fonts::icon, 32));
	QVBoxLayout* layout = new QVBoxLayout(page,5);
	layout->setResizeMode( QLayout::Minimum );
	
	{ //Sword locales
		layout->addWidget(
			CToolClass::explanationLabel(page, i18n("Specify a language for biblical booknames"),
				i18n("Sword has a number of locales available which can be used to internationalize the \
	booknames of the bible. You can specify which locale to use. If you want to \
	create a new locale, see http://www.crosswire.org/sword/develop for details."))
		);
	
		m_settings.fonts.swordLocaleCombo = new QComboBox(page);
		QLabel* label = new QLabel(m_settings.fonts.swordLocaleCombo, i18n("Language for biblical booknames"), page);
		QToolTip::add(m_settings.fonts.swordLocaleCombo, CResMgr::settings::sword::general::language::tooltip);

		
		QHBoxLayout* hBoxLayout = new QHBoxLayout();
		hBoxLayout->addWidget(label);
		hBoxLayout->addWidget(m_settings.fonts.swordLocaleCombo);
		hBoxLayout->addStretch();
		layout->addLayout(hBoxLayout);
	
		QStringList languageNames;
		languageNames.append( i18n("English") );
		
		const list<sword::SWBuf> locales = sword::LocaleMgr::getSystemLocaleMgr()->getAvailableLocales();
		for (list<sword::SWBuf>::const_iterator it = locales.begin(); it != locales.end(); it++) {
			const CLanguageMgr::Language* const l = CPointers::languageMgr()->languageForAbbrev(
				sword::LocaleMgr::getSystemLocaleMgr()->getLocale((*it).c_str())->getName()
			);
			
			if (l->isValid()) {
				languageNames.append( l->translatedName() );
			}
			else {
				languageNames.append(
					sword::LocaleMgr::getSystemLocaleMgr()->getLocale((*it).c_str())->getDescription() 
				);
			}
		}
		
		languageNames.sort();
		m_settings.fonts.swordLocaleCombo->insertStringList( languageNames );
		
		const CLanguageMgr::Language* const l = CPointers::languageMgr()->languageForAbbrev( 
			CBTConfig::get(CBTConfig::language)
		);
		QString currentLanguageName;
		if ( l->isValid() && languageNames.contains(l->translatedName()) ) { //tranlated language name is in the box
			currentLanguageName = l->translatedName();
		}
		else { //a language like "German Abbrevs" might be the language to set
			sword::SWLocale* locale = sword::LocaleMgr::LocaleMgr::getSystemLocaleMgr()->getLocale(
				CBTConfig::get(CBTConfig::language).local8Bit()
			);
			if (locale) {
				currentLanguageName = QString::fromLatin1(locale->getDescription());
			}
		}
		
		if (currentLanguageName.isEmpty()) { // set english as default if nothing was chosen
			currentLanguageName = i18n("English");
		}
		
		//now set the item with the right name as current item
		for (int i = 0; i < m_settings.fonts.swordLocaleCombo->count(); ++i) {
			if (currentLanguageName == m_settings.fonts.swordLocaleCombo->text(i)) {
				m_settings.fonts.swordLocaleCombo->setCurrentItem(i);
				break; //item found, finish the loop
			}
		}
	
		layout->addSpacing( 20 );
	}
	{ //Font settings
		layout->addWidget(
			CToolClass::explanationLabel(
				page,
				i18n("Select custom fonts per-language"),
				i18n("Here you find a list of all languages of the installed works. \
	You can specify a custom font for each language that needs a special font \
	to be displayed correctly.")
			)
		);
		layout->addSpacing(5);
		QHBoxLayout* hLayout = new QHBoxLayout();
	
		m_settings.fonts.usage = new QComboBox(page);
		QToolTip::add(m_settings.fonts.usage, CResMgr::settings::fonts::typeChooser::tooltip);

	
		hLayout->addWidget(m_settings.fonts.usage);
	
		CLanguageMgr::LangMap langMap = languageMgr()->availableLanguages();
	
		for ( CLanguageMgr::LangMapIterator it( langMap ); it.current(); ++it ) {
			const QString name = it.current()->translatedName().isEmpty() ? 
				it.current()->abbrev() : it.current()->translatedName();
			m_settings.fonts.fontMap.insert(name, CBTConfig::get(it.current()) );
		}
	
		for( QMap<QString, CBTConfig::FontSettingsPair>::Iterator it = m_settings.fonts.fontMap.begin(); it != m_settings.fonts.fontMap.end(); ++it ) {
			if ( m_settings.fonts.fontMap[it.key()].first ) { //show font icon
				m_settings.fonts.usage->insertItem(SmallIcon("fonts"), it.key() );
			}
			else {	//don't show icon for font
				m_settings.fonts.usage->insertItem(it.key());
			}
		}

		m_settings.fonts.useOwnFontBox = new QCheckBox(i18n("Use custom font"), page, "font checkbox");
		connect(m_settings.fonts.useOwnFontBox, SIGNAL(toggled(bool)), SLOT(useOwnFontClicked(bool)));
		hLayout->addWidget(m_settings.fonts.useOwnFontBox);
	
		layout->addLayout(hLayout);
	//#warning TODO: remember the last selected font and jump there.
	
		m_settings.fonts.fontChooser = new KFontChooser(page, "fonts", false, QStringList(), true, 5);
		m_settings.fonts.fontChooser->setSampleText(i18n("The quick brown fox jumps over the lazy dog."));
		layout->addWidget(m_settings.fonts.fontChooser);
	
		connect(m_settings.fonts.fontChooser, SIGNAL(fontSelected(const QFont&)), SLOT(newDisplayWindowFontSelected(const QFont&)));
		connect(m_settings.fonts.usage, SIGNAL(activated(const QString&)), SLOT(newDisplayWindowFontAreaSelected(const QString&)));
	
		m_settings.fonts.fontChooser->setFont( m_settings.fonts.fontMap[m_settings.fonts.usage->currentText()].second );
		useOwnFontClicked( m_settings.fonts.fontMap[m_settings.fonts.usage->currentText()].first );
		m_settings.fonts.useOwnFontBox->setChecked( m_settings.fonts.fontMap[m_settings.fonts.usage->currentText()].first );
		m_settings.fonts.fontChooser->setMinimumSize(m_settings.fonts.fontChooser->sizeHint());
	}
}


/** Init accel key section. */
void COptionsDialog::initAccelerators(){
  QVBox* page = addVBoxPage(i18n("HotKeys"),QString::null, DesktopIcon(CResMgr::settings::keys::icon,32));

  KTabCtl* tabCtl = new KTabCtl(page);

// ----- new tab: All display windows ------ //
  QFrame* currentTab = new QVBox(tabCtl);
	currentTab->setMargin(3);
  tabCtl->addTab(currentTab, i18n("Application-wide"));

	CBTConfig::setupAccel( CBTConfig::application, m_settings.keys.application.accel  );
//	CSwordPresenter::insertKeyboardActions( m_settings.keys.application.accel );
	m_settings.keys.application.accel->readSettings();

 	m_settings.keys.application.keyChooser = new KKeyChooser( m_settings.keys.application.accel, currentTab, false );
	
	
// 	QToolTip::add(m_settings.keys.application.keyChooser, TT_OD_KEYS_CHOOSER);

// ----- new tab: All display windows ------ //
	currentTab = new QVBox(tabCtl);
	currentTab->setMargin(3);
	tabCtl->addTab(currentTab, i18n("All read windows"));

	m_settings.keys.general.accel = new KAccel(this); //delete in destructor
	CBTConfig::setupAccel( CBTConfig::readWindow, m_settings.keys.general.accel  );
	CReadWindow::insertKeyboardActions( m_settings.keys.general.accel );
	m_settings.keys.general.accel->readSettings();

 	m_settings.keys.general.keyChooser = new KKeyChooser( m_settings.keys.general.accel, currentTab );
//	QToolTip::add(m_settings.keys.general.keyChooser, TT_OD_DISPLAY_WINDOW_KEYS_GENERAL);

// ----- new tab: Bible windows ------ //
	currentTab = new QVBox(tabCtl);
	currentTab->setMargin(3);
	tabCtl->addTab(currentTab, i18n("Bible windows"));

	m_settings.keys.bible.accel = new KAccel(this); //delete in destructor
	CBTConfig::setupAccel( CBTConfig::bibleWindow, m_settings.keys.bible.accel  );
	CBibleReadWindow::insertKeyboardActions( m_settings.keys.bible.accel );
	m_settings.keys.bible.accel->readSettings();

 	m_settings.keys.bible.keyChooser = new KKeyChooser( m_settings.keys.bible.accel, currentTab/*, false*/ );
//	QToolTip::add(m_settings.keys.bible.keyChooser, TT_OD_DISPLAY_WINDOW_KEYS_BIBLE);

// ----- new tab: Commentary windows ------ //
	currentTab = new QVBox(tabCtl);
	currentTab->setMargin(3);
	tabCtl->addTab(currentTab, i18n("Commentary windows"));

	m_settings.keys.commentary.accel = new KAccel(this); //delete in destructor
	CBTConfig::setupAccel( CBTConfig::commentaryWindow, m_settings.keys.commentary.accel  );
	CCommentaryReadWindow::insertKeyboardActions( m_settings.keys.commentary.accel );
//	m_settings.keys.commentary.accel->setConfigGroup("Lexicon shortcuts");
	m_settings.keys.commentary.accel->readSettings();

 	m_settings.keys.commentary.keyChooser = new KKeyChooser( m_settings.keys.commentary.accel, currentTab, false );
// 	QToolTip::add(m_settings.keys.commentary.keyChooser, TT_OD_DISPLAY_WINDOW_KEYS_COMMENTARY);

// ----- new tab: Lexicon windows ------ //
	currentTab = new QVBox(tabCtl);
	currentTab->setMargin(3);
	tabCtl->addTab(currentTab, i18n("Lexicon windows"));

	m_settings.keys.lexicon.accel = new KAccel(this); //delete in destructor
	CBTConfig::setupAccel( CBTConfig::lexiconWindow, m_settings.keys.lexicon.accel  );
	CLexiconReadWindow::insertKeyboardActions( m_settings.keys.lexicon.accel );
//	m_settings.keys.lexicon.accel->setConfigGroup("Lexicon shortcuts");
	m_settings.keys.lexicon.accel->readSettings();

 	m_settings.keys.lexicon.keyChooser = new KKeyChooser( m_settings.keys.lexicon.accel, currentTab, false );
// 	QToolTip::add(m_settings.keys.lexicon.keyChooser, TT_OD_DISPLAY_WINDOW_KEYS_LEXICON);


// ----- new tab: Book windows ------ //
	currentTab = new QVBox(tabCtl);
	currentTab->setMargin(3);
	tabCtl->addTab(currentTab, i18n("Book windows"));

	m_settings.keys.book.accel = new KAccel(this); //delete in destructor
	CBTConfig::setupAccel( CBTConfig::bookWindow, m_settings.keys.book.accel  );
	CBookReadWindow::insertKeyboardActions( m_settings.keys.book.accel );
//	m_settings.keys.book.accel->setConfigGroup("Book shortcuts");
	m_settings.keys.book.accel->readSettings();

	m_settings.keys.book.keyChooser = new KKeyChooser( m_settings.keys.book.accel, currentTab, false );
// 	QToolTip::add(m_settings.keys.book.keyChooser, TT_OD_DISPLAY_WINDOW_KEYS_LEXICON);
}

/** Init Sword section. */
void COptionsDialog::initSword(){
	QVBox* page = addVBoxPage(i18n("Desk"),QString::null, DesktopIcon(CResMgr::settings::sword::icon,32));
  KTabCtl* tabCtl = new KTabCtl(page);
	
	{ //Standard works
		QFrame* currentTab = new QFrame(tabCtl);
		tabCtl->addTab(currentTab, i18n("Standard works"));
		QGridLayout* gridLayout = new QGridLayout(currentTab,10,2, 5,5); //the last row is for stretching available space
		gridLayout->setResizeMode(QLayout::Minimum);
	
		gridLayout->addMultiCellWidget(
			CToolClass::explanationLabel(currentTab, i18n("Standard works"),
				i18n("Standard works are used when no particular work is specified, \
	for example when a hyperlink into a Bible or lexicon was clicked .")),
			0,0,0,-1 /*fill the horizontal space*/
		);
	
		m_settings.swords.standardBible = new QComboBox(currentTab);
		QLabel* label = new QLabel(m_settings.swords.standardBible, i18n("Standard Bible"), currentTab);
		label->setAutoResize(true);
		QToolTip::add(m_settings.swords.standardBible, CResMgr::settings::sword::modules::bible::tooltip);

		gridLayout->addWidget(label,1,0);
		gridLayout->addWidget(m_settings.swords.standardBible,1,1);
	
		m_settings.swords.standardCommentary = new QComboBox(currentTab);
		label = new QLabel(m_settings.swords.standardCommentary, i18n("Standard Commentary"), currentTab);
		label->setAutoResize(true);
		QToolTip::add(m_settings.swords.standardCommentary, CResMgr::settings::sword::modules::commentary::tooltip);

		gridLayout->addWidget(label,2,0);
		gridLayout->addWidget(m_settings.swords.standardCommentary,2,1);
	
	
		m_settings.swords.standardLexicon = new QComboBox(currentTab);
		label = new QLabel(m_settings.swords.standardLexicon, i18n("Standard Lexicon"), currentTab);
		label->setAutoResize(true);
		QToolTip::add(m_settings.swords.standardLexicon, CResMgr::settings::sword::modules::lexicon::tooltip);

		gridLayout->addWidget(label,3,0);
		gridLayout->addWidget(m_settings.swords.standardLexicon,3,1);
	
		m_settings.swords.standardDailyDevotional = new QComboBox(currentTab);
		label = new QLabel(m_settings.swords.standardDailyDevotional, i18n("Standard Daily Devotional"), currentTab);
		label->setAutoResize(true);
		QToolTip::add(m_settings.swords.standardDailyDevotional, CResMgr::settings::sword::modules::dailyDevotional::tooltip);

		gridLayout->addWidget(label,4,0);
		gridLayout->addWidget(m_settings.swords.standardDailyDevotional,4,1);
	
		m_settings.swords.standardHebrewStrong = new QComboBox(currentTab);
		label = new QLabel(m_settings.swords.standardHebrewStrong, i18n("Standard Hebrew Strong's Lexicon"), currentTab);
		label->setAutoResize(true);
		QToolTip::add(m_settings.swords.standardHebrewStrong, CResMgr::settings::sword::modules::hebrewStrongs::tooltip);

		gridLayout->addWidget(label,5,0);
		gridLayout->addWidget(m_settings.swords.standardHebrewStrong,5,1);
	
		m_settings.swords.standardGreekStrong = new QComboBox(currentTab);
		label = new QLabel(m_settings.swords.standardGreekStrong, i18n("Standard Greek Strong's Lexicon"), currentTab);
		label->setAutoResize(true);
		QToolTip::add(m_settings.swords.standardGreekStrong, CResMgr::settings::sword::modules::greekStrongs::tooltip);

		gridLayout->addWidget(label,6,0);
		gridLayout->addWidget(m_settings.swords.standardGreekStrong,6,1);
			
		m_settings.swords.standardHebrewMorph = new QComboBox(currentTab);
		label = new QLabel(m_settings.swords.standardHebrewMorph, i18n("Standard Hebrew Morphological Lexicon"), currentTab);
		QToolTip::add(m_settings.swords.standardHebrewMorph, CResMgr::settings::sword::modules::hebrewMorph::tooltip);

		gridLayout->addWidget(label,7,0);
		gridLayout->addWidget(m_settings.swords.standardHebrewMorph,7,1);
	
		m_settings.swords.standardGreekMorph = new QComboBox(currentTab);
		label = new QLabel(m_settings.swords.standardGreekMorph, i18n("Standard Greek Morphological Lexicon"), currentTab);
		label->setAutoResize(true); 	 	
		QToolTip::add(m_settings.swords.standardGreekMorph, CResMgr::settings::sword::modules::greekMorph::tooltip);

		gridLayout->addWidget(label,8,0);
		gridLayout->addWidget(m_settings.swords.standardGreekMorph,8,1);
	
		gridLayout->setRowStretch(9,5);							
			
		//fill the comboboxes with the right modules
		ListCSwordModuleInfo& modules = backend()->moduleList();
		QString modDescript;
	/*  for ( modules.first(); modules.current(); modules.next() ) {*/
		ListCSwordModuleInfo::iterator end_it = modules.end();
		for (ListCSwordModuleInfo::iterator it(modules.begin()); it != end_it; ++it) {
			modDescript = (*it)->config(CSwordModuleInfo::Description);
			
			switch ((*it)->type()) {
				case CSwordModuleInfo::Bible:
					m_settings.swords.standardBible->insertItem(modDescript);
					break;
				case CSwordModuleInfo::Commentary:
					m_settings.swords.standardCommentary->insertItem(modDescript);				
					break;
				case CSwordModuleInfo::Lexicon:
				{
					bool inserted = false;
					if ((*it)->has(CSwordModuleInfo::HebrewDef)) {
						m_settings.swords.standardHebrewStrong->insertItem(modDescript);
						inserted = true;          
					}
					if ((*it)->has(CSwordModuleInfo::GreekDef)) {
						m_settings.swords.standardGreekStrong->insertItem(modDescript);
						inserted = true;
					}
					if ((*it)->has(CSwordModuleInfo::HebrewParse)) {
						m_settings.swords.standardHebrewMorph->insertItem(modDescript);				
						inserted = true;
					}
					if ((*it)->has(CSwordModuleInfo::GreekParse)) {       
						m_settings.swords.standardGreekMorph->insertItem(modDescript);
						inserted = true;
					}
					if ((*it)->category() == CSwordModuleInfo::DailyDevotional) {
						m_settings.swords.standardDailyDevotional->insertItem(modDescript);
						inserted = true;
					}
					
					if (!inserted) {//daily dvotionals, striong lexicons etc. are not very useful for word lookups
						m_settings.swords.standardLexicon->insertItem(modDescript);
					}
					
					break;
				} 				
				default://unknown type
					break;
			}
		}
	
	//using two lists and one loop is better than six loops with almost the same code :)
		QPtrList<QComboBox> comboList;
		comboList.setAutoDelete(false);//don't delete the combos accidentally
		QStringList moduleList;
		
		for (int i = 0; i < (int)CBTConfig::lastModuleType; ++i) {
			//fill the combobox list in the right order (i.e. same order as the CBTConfig::module enum list)
			CBTConfig::modules moduleType = (CBTConfig::modules)(i);
			switch (moduleType) {
				case CBTConfig::standardBible:
					comboList.append(m_settings.swords.standardBible);
					break;
				case CBTConfig::standardCommentary:
					comboList.append(m_settings.swords.standardCommentary);
					break;
				case CBTConfig::standardLexicon:
					comboList.append(m_settings.swords.standardLexicon);
					break;
				case CBTConfig::standardDailyDevotional:
					comboList.append(m_settings.swords.standardDailyDevotional); 
					break;
				case CBTConfig::standardHebrewStrongsLexicon:
					comboList.append(m_settings.swords.standardHebrewStrong);
					break;
				case CBTConfig::standardGreekStrongsLexicon:
					comboList.append(m_settings.swords.standardGreekStrong);
					break;
				case CBTConfig::standardHebrewMorphLexicon:
					comboList.append(m_settings.swords.standardHebrewMorph);
					break;
				case CBTConfig::standardGreekMorphLexicon:
					comboList.append(m_settings.swords.standardGreekMorph);
					break;
			};
			
			//fill the module list
			CSwordModuleInfo* const m = CBTConfig::get( (CBTConfig::modules)(i) );
			if (m) {
				moduleList << m->config(CSwordModuleInfo::Description);
			}
			else {
				moduleList << QString::null;
			}
		}
	
		QString module = QString::null;
		int item = 0;
		int count = 0; 	
		for (QComboBox* combo = comboList.first(); combo; combo = comboList.next() ) {		
			module = moduleList[comboList.at()];
			count = combo->count();
			combo->setMaximumWidth(300);
			
			for (item = 0; item < count; item++) {
				if (combo->text(item) == module ) {
					combo->setCurrentItem(item);
					break;
				}
			}
		}
	}
	{ // ---------- new tab: filters -------- //
		QFrame* currentTab = new QFrame(tabCtl);
		tabCtl->addTab(currentTab, i18n("Text filters"));
		QVBoxLayout* layout = new QVBoxLayout(currentTab,5);
	
		layout->addWidget( CToolClass::explanationLabel(currentTab, i18n("Text filters"),
			i18n("Filters control the appearance of text. \
				Here you can specify default settings for all filters. \
				You can change the filter settings in each display window, of course.")) );
	
		layout->addSpacing(5);
				
		m_settings.swords.lineBreaks = new QCheckBox(currentTab);
		m_settings.swords.lineBreaks->setText(i18n("Insert line break after each verse"));
		m_settings.swords.lineBreaks->setChecked(CBTConfig::get(CBTConfig::lineBreaks));
		layout->addWidget(m_settings.swords.lineBreaks);
	
		m_settings.swords.verseNumbers = new QCheckBox(currentTab);
		m_settings.swords.verseNumbers->setText(i18n("Show verse numbers"));
		m_settings.swords.verseNumbers->setChecked(CBTConfig::get(CBTConfig::verseNumbers));		
		layout->addWidget(m_settings.swords.verseNumbers);
	
		m_settings.swords.headings = new QCheckBox(currentTab);
		m_settings.swords.headings->setText(i18n("Show section headings"));
		m_settings.swords.headings->setChecked(CBTConfig::get(CBTConfig::headings));
		layout->addWidget(m_settings.swords.headings);
	
	
		m_settings.swords.scriptureReferences = new QCheckBox(currentTab);
		m_settings.swords.scriptureReferences->setText(i18n("Show scripture cross-references"));
		m_settings.swords.scriptureReferences->setChecked(CBTConfig::get(CBTConfig::scriptureReferences));
		layout->addWidget(m_settings.swords.scriptureReferences);
	
		m_settings.swords.greekAccents = new QCheckBox(currentTab);
		m_settings.swords.greekAccents->setText(i18n("Show Greek accents"));
		m_settings.swords.greekAccents->setChecked(CBTConfig::get(CBTConfig::greekAccents));
		layout->addWidget(m_settings.swords.greekAccents);
	
		m_settings.swords.hebrewPoints = new QCheckBox(currentTab);
		m_settings.swords.hebrewPoints->setText(i18n("Show Hebrew vowel points"));
		m_settings.swords.hebrewPoints->setChecked(CBTConfig::get(CBTConfig::hebrewPoints));
		layout->addWidget(m_settings.swords.hebrewPoints);
	
		m_settings.swords.hebrewCantillation = new QCheckBox(currentTab);
		m_settings.swords.hebrewCantillation->setText(i18n("Show Hebrew cantillation marks"));
		m_settings.swords.hebrewCantillation->setChecked(CBTConfig::get(CBTConfig::hebrewCantillation));		
		layout->addWidget(m_settings.swords.hebrewCantillation);
	
		m_settings.swords.textualVariants = new QCheckBox(currentTab);
		m_settings.swords.textualVariants->setText(i18n("Use textual variants"));
		m_settings.swords.textualVariants->setChecked(CBTConfig::get(CBTConfig::textualVariants));		
		layout->addWidget(m_settings.swords.textualVariants);
			
		layout->addStretch(4);	
	}
}

void COptionsDialog::saveAccelerators(){
//#warning check
// 	m_settings.keys.application.accel->writeSettings();
// 	m_settings.keys.application.keyChooser->save();
 	m_settings.keys.application.keyChooser->commitChanges();
	m_settings.keys.application.accel->writeSettings(); 	

// 	m_settings.keys.general.accel->writeSettings();
//		
// 	m_settings.keys.bible.accel->writeSettings();		
// 	m_settings.keys.bible.keyChooser->save();
 	m_settings.keys.bible.keyChooser->commitChanges();
	m_settings.keys.bible.accel->writeSettings();

//		
// 	m_settings.keys.commentary.accel->writeSettings();
// 	m_settings.keys.commentary.keyChooser->save();
 	m_settings.keys.commentary.keyChooser->commitChanges();
	m_settings.keys.commentary.accel->writeSettings(); 	

		
// 	m_settings.keys.lexicon.accel->writeSettings();
// 	m_settings.keys.lexicon.keyChooser->save();
 	m_settings.keys.lexicon.keyChooser->commitChanges();
	m_settings.keys.lexicon.accel->writeSettings(); 	

// 	m_settings.keys.book.accel->writeSettings(); 	 	
// 	m_settings.keys.book.keyChooser->save();
 	m_settings.keys.book.keyChooser->commitChanges();
	m_settings.keys.book.accel->writeSettings();
}

/** No descriptions */
void COptionsDialog::saveDisplayStyle(){
	CBTConfig::set(CBTConfig::displayStyle, m_settings.displayStyle.styleChooser->currentText());
}

/** No descriptions */
void COptionsDialog::saveFonts(){
	for(QMap<QString, CBTConfig::FontSettingsPair>::Iterator it = m_settings.fonts.fontMap.begin(); it != m_settings.fonts.fontMap.end(); ++it ) {
    const CLanguageMgr::Language* const lang = languageMgr()->languageForTranslatedName(it.key());
    if (!lang->isValid()) { //we probably use a language, for which we have only the abbrev
      CLanguageMgr::Language l(it.key(), it.key(), it.key());
	    CBTConfig::set(&l, it.data());

    }
		else {
    	CBTConfig::set(lang, it.data());
		}
 	}
}

/** No descriptions */
// void COptionsDialog::/*/*savePro*/*/files(){
// }

/** No descriptions */
void COptionsDialog::saveStartup(){
	CBTConfig::set( CBTConfig::logo, m_settings.startup.showLogo->isChecked() );	
 	CBTConfig::set( CBTConfig::tips, m_settings.startup.showTips->isChecked() );				
}

/** No descriptions */
void COptionsDialog::saveSword(){

	for (int i = 0; i < (int)CBTConfig::lastModuleType; ++i) {
		QString moduleDescription = QString::null;
  	
		CBTConfig::modules moduleType = (CBTConfig::modules)(i);
		switch (moduleType) {
			case CBTConfig::standardBible:
				moduleDescription = m_settings.swords.standardBible->currentText();
				break;
			case CBTConfig::standardCommentary:
				moduleDescription = m_settings.swords.standardCommentary->currentText();
				break;
			case CBTConfig::standardLexicon:
				moduleDescription = m_settings.swords.standardLexicon->currentText();
				break;
			case CBTConfig::standardDailyDevotional:
				moduleDescription = m_settings.swords.standardDailyDevotional->currentText();
				break;
			case CBTConfig::standardHebrewStrongsLexicon:
				moduleDescription = m_settings.swords.standardHebrewStrong->currentText();
				break;
			case CBTConfig::standardGreekStrongsLexicon:
				moduleDescription = m_settings.swords.standardGreekStrong->currentText();
				break;
			case CBTConfig::standardHebrewMorphLexicon:
				moduleDescription = m_settings.swords.standardHebrewMorph->currentText();
				break;
			case CBTConfig::standardGreekMorphLexicon:
				moduleDescription = m_settings.swords.standardGreekMorph->currentText();
				break;
		};
		
		CSwordModuleInfo* const module = backend()->findModuleByDescription(moduleDescription);
		CBTConfig::set(moduleType, module);
	}

	
	QString languageAbbrev;
 	
	const QString currentLanguageName = m_settings.fonts.swordLocaleCombo->currentText();
	const CLanguageMgr::Language* const l = CPointers::languageMgr()->languageForTranslatedName( currentLanguageName );
	
	if (l && l->isValid()) {
		languageAbbrev = l->abbrev();
	}
	else { //it can be the lang abbrev like de_abbrev or the Sword description
		list <sword::SWBuf> locales = sword::LocaleMgr::getSystemLocaleMgr()->getAvailableLocales();
		
		for (list <sword::SWBuf>::iterator it = locales.begin(); it != locales.end(); it++) {
			SWLocale* locale = LocaleMgr::getSystemLocaleMgr()->getLocale((*it).c_str());
			Q_ASSERT(locale);
			
			if ( locale && (QString::fromLatin1(locale->getDescription()) == currentLanguageName) ) {
				languageAbbrev = QString::fromLatin1(locale->getName());	//we found the abbrevation for the current language
				break;
			}
		}
		
		if (languageAbbrev.isEmpty()) {
			languageAbbrev = currentLanguageName; //probably a non-standard locale name like de_abbrev
		}
	}
	
 	if (!languageAbbrev.isEmpty()) {
  	CBTConfig::set(CBTConfig::language, languageAbbrev);
 	}

 	CBTConfig::set(CBTConfig::lineBreaks, m_settings.swords.lineBreaks->isChecked());
 	CBTConfig::set(CBTConfig::verseNumbers, m_settings.swords.verseNumbers->isChecked());
 	CBTConfig::set(CBTConfig::headings, m_settings.swords.headings->isChecked());
 	CBTConfig::set(CBTConfig::scriptureReferences, m_settings.swords.scriptureReferences->isChecked());
 	CBTConfig::set(CBTConfig::hebrewPoints, m_settings.swords.hebrewPoints->isChecked());
 	CBTConfig::set(CBTConfig::hebrewCantillation, m_settings.swords.hebrewCantillation->isChecked());
 	CBTConfig::set(CBTConfig::greekAccents, m_settings.swords.greekAccents->isChecked());
 	CBTConfig::set(CBTConfig::textualVariants, m_settings.swords.textualVariants->isChecked());
}

/** This slot is called when the "Use own font for language" bo was clicked. */
void COptionsDialog::useOwnFontClicked( bool isOn ){
  m_settings.fonts.fontChooser->setEnabled(isOn);
  m_settings.fonts.fontMap[ m_settings.fonts.usage->currentText() ].first = isOn;

  if (isOn) { //show font icon
    m_settings.fonts.usage->changeItem(
			SmallIcon("fonts"),
			m_settings.fonts.usage->currentText(),
			m_settings.fonts.usage->currentItem() 
		);
  }
  else {    //don't show
    m_settings.fonts.usage->changeItem(
			m_settings.fonts.usage->currentText(), 
			m_settings.fonts.usage->currentItem() 
		);
  }
}

void COptionsDialog::updateStylePreview() {
	//update the style preview widget
 	using namespace Rendering;
	
	const QString styleName = m_settings.displayStyle.styleChooser->currentText();
	
	CSwordModuleInfo* const module = CBTConfig::get(CBTConfig::standardBible);
	if (!module) {
		return; //WARNING: return already here!
	}
		
 	CTextRendering::KeyTreeItem::Settings settings;
	
	CTextRendering::KeyTree tree;
	tree.append( new CTextRendering::KeyTreeItem("Gen 1:1", "Gen 1:3", module, settings) );
	settings.highlight = true;
	tree.append( new CTextRendering::KeyTreeItem("Joh 3:16", module, settings) );
	settings.highlight = false;
	tree.append( new CTextRendering::KeyTreeItem("Joh 16:4", module, settings) );
	
	CDisplayRendering render;
	m_settings.displayStyle.stylePreview->begin();
	
	const QString oldStyleName = CBTConfig::get(CBTConfig::displayStyle);
	CBTConfig::set(CBTConfig::displayStyle, styleName);
	
	m_settings.displayStyle.stylePreview->write( render.renderKeyTree(tree) );
	
	m_settings.displayStyle.stylePreview->end();

	CBTConfig::set(CBTConfig::displayStyle, oldStyleName);
}
