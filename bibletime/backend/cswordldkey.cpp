/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//BibleTime includes
#include "cswordldkey.h"
#include "cswordlexiconmoduleinfo.h"

//Qt includes
#include <qtextcodec.h>

//Sword includes
#include <swmodule.h>
#include <swld.h>
#include <utilstr.h>


CSwordLDKey::CSwordLDKey( CSwordModuleInfo* module ) {
	if ((m_module = dynamic_cast<CSwordLexiconModuleInfo*>(module))) {
		//    *(m_module->module()) = TOP;
	}

	SWKey::operator = (" ");
}

/** No descriptions */
CSwordLDKey::CSwordLDKey( const CSwordLDKey &k ) : CSwordKey(k), SWKey((const char*)k) {}

/** No descriptions */
CSwordLDKey::CSwordLDKey( const SWKey *k, CSwordModuleInfo* module) : CSwordKey(module), SWKey(*k) {}

/** Clones this object by copying the members. */
CSwordLDKey* CSwordLDKey::copy() const {
	return new CSwordLDKey(*this);
}

/** Sets the module of this key. */
CSwordModuleInfo* const CSwordLDKey::module(CSwordModuleInfo* const newModule) {
	if (newModule && newModule->type() == CSwordModuleInfo::Lexicon) {
		const QString oldKey = key();
		m_module = newModule;
		key(oldKey);
	}

	return m_module;
}

/** Sets the key of this instance */
const QString CSwordLDKey::key() const {
	
	if (!m_module || m_module->isUnicode()) {
		return QString::fromUtf8((const char*)*this);
	} else {
		QTextCodec *codec = QTextCodec::codecForName("CP1252");
		return codec->toUnicode((const char*)*this);
	}
}

const char * CSwordLDKey::rawKey() const {
	return (const char*)*this;
}

const bool CSwordLDKey::key( const QString& newKey ) {
	
	Q_ASSERT(m_module);

	if (!m_module || m_module->isUnicode()) {
		return key((const char*)newKey.utf8());
	} else {
		QTextCodec *codec = QTextCodec::codecForName("CP1252");
		return key((const char*)codec->fromUnicode(newKey));
	}
}


/** Sets the key of this instance */
const bool CSwordLDKey::key( const char* newKey ) {
	Q_ASSERT(newKey);

	if (newKey) {
		SWKey::operator = (newKey); //set the key

		m_module->module()->SetKey(this);
		//   m_module->module()->getKey()->setText( (const char*)key().utf8() );
		m_module->snap();
		//   SWKey::operator = (m_module->module()->KeyText());
	}

	return !Error();
}

/** Uses the parameter to returns the next entry afer this key. */
CSwordLDKey* CSwordLDKey::NextEntry() {
	m_module->module()->SetKey(this); //use this key as base for the next one!

	m_module->module()->setSkipConsecutiveLinks(true);
	( *( m_module->module() ) )++;
	m_module->module()->setSkipConsecutiveLinks(false);

	key(m_module->module()->KeyText());
	SWKey::operator = (m_module->module()->KeyText());

	return this;
}

/** Uses the parameter to returns the next entry afer this key. */
CSwordLDKey* CSwordLDKey::PreviousEntry() {
	m_module->module()->SetKey(this); //use this key as base for the next one!

	m_module->module()->setSkipConsecutiveLinks(true);
	( *( m_module->module() ) )--;
	m_module->module()->setSkipConsecutiveLinks(false);

	SWKey::operator = (m_module->module()->KeyText());

	return this;
}

/** Assignment operator for more ease of use of this class. */
CSwordLDKey& CSwordLDKey::operator = (const QString& keyname ) {
	 key(keyname);
	 return *this;
}
