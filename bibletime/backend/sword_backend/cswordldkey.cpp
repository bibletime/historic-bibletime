/***************************************************************************
                          cswordldkey.cpp  -  description
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

//BibleTime includes
#include "cswordldkey.h"
#include "cswordlexiconmoduleinfo.h"

//Qt includes

//Sword includes
#include <swmodule.h>
#include <swld.h>
#include <utilstr.h>


CSwordLDKey::CSwordLDKey( CSwordModuleInfo* module ) {
	if ( !(m_module = dynamic_cast<CSwordLexiconModuleInfo*>(module)) ) {
		throw EBadModule();
	}		
}

CSwordLDKey::~CSwordLDKey(){
}

/** Sets the key of this instance */
bool CSwordLDKey::setKey( const QString key ){
	SWKey::operator = ((const char*)key.local8Bit());	
	
	m_module->module()->SetKey(*this->clone());			
	
	SWKey::operator = (m_module->module()->KeyText());
	
	return !(bool)error;
}

/** Uses the parameter to returns the next entry afer this key. */
void CSwordLDKey::NextEntry(){
	m_module->module()->SetKey(*this->clone());	//use this key as base for the next one!		
	( *( m_module->module() ) )++;
	setKey(m_module->module()->KeyText());
}

/** Uses the parameter to returns the next entry afer this key. */
void CSwordLDKey::PreviousEntry(){
	m_module->module()->SetKey(*this->clone());	//use this key as base for the next one!		
	( *( m_module->module() ) )--;
	setKey(m_module->module()->KeyText());
}

/** Sets the module of this key. */
void CSwordLDKey::setModule( CSwordModuleInfo* module ){
	const QString oldKey = QString::fromLocal8Bit( (const char*)*this );
	if (module && module->getType() == CSwordModuleInfo::Lexicon) {
		m_module = module;	
		setKey(oldKey);
	}
}

/** Returns the current key as a QString */
const QString CSwordLDKey::getKey() const{
	return QString::fromLocal8Bit( m_module->module()->KeyText() );
}

/** Reimplementation of the cast operator to const char* */
CSwordLDKey::operator const char*(){
	return getKey().local8Bit();
}

/** Returns the rendered text of this entry. */
const QString CSwordLDKey::getRenderedText() const{
	m_module->module()->SetKey(*this->clone());
	return QString::fromLocal8Bit( (const char*)*m_module->module() );
}

/** Returns the stripped down text of this entry. */
const QString CSwordLDKey::getStrippedText() const{
	m_module->module()->SetKey(*this->clone());
	return QString::fromLocal8Bit( m_module->module()->StripText() );
}
