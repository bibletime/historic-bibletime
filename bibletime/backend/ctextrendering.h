//
// C++ Interface: ctextrendering
//
// Description:
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CTEXTRENDERING_H
#define CTEXTRENDERING_H

//BT includes
#include "backend/cswordbackend.h"

#include "frontend/cbtconfig.h"

#include "util/autoptrvector.h"

//QT includes
#include <qstring.h>
// #include <qptrlist.h>

class CSwordModuleInfo;
class CSwordKey;

/**
 * This class is responsible for text rendering.
 * @author The BibleTime team
*/
namespace Rendering {

class CTextRendering {
public:
	class KeyTreeItem;
	class KeyTree;
	typedef util::AutoPtrVector<KeyTreeItem> KeyTreeItemList;
	
	class KeyTreeItem {
	public:
		struct Settings {
			enum KeyRenderingFace {
				NoKey, //means no key shown at all
				SimpleKey, //means only versenumber or only lexicon entry name
				CompleteShort, //means key like "Gen 1:1"
				CompleteLong //means "Genesis 1:1"
			};

			Settings(const bool highlight = false, KeyRenderingFace keyRendering = SimpleKey) : highlight(highlight), keyRenderingFace(keyRendering) {
			};
			
			bool highlight;
			KeyRenderingFace keyRenderingFace;
		};
		
		KeyTreeItem();
		KeyTreeItem(const KeyTreeItem& i);
		KeyTreeItem(const QString& key, CSwordModuleInfo const * module, const Settings settings);
		KeyTreeItem(const QString& key, const ListCSwordModuleInfo& modules, const Settings settings);
		virtual ~KeyTreeItem();
		
		inline const ListCSwordModuleInfo& modules() const {
			return m_moduleList;
		};
		inline const QString& key() const {
			return m_key;
		};
		inline const Settings& settings() const {
			return m_settings;
		};
		
		inline KeyTree* const childList() const;
		inline const bool hasChildItems() const;

	protected:
		Settings m_settings;
		ListCSwordModuleInfo m_moduleList;
		QString m_key;
		mutable KeyTree* m_childList;
	};
	
	class KeyTree : public KeyTreeItemList {
	public:
		ListCSwordModuleInfo collectModules();
	};

  virtual ~CTextRendering() {
	};

	const QString renderKeyTree( KeyTree& );
	
	const QString renderKeyRange( const QString& start, const QString& stop, const ListCSwordModuleInfo& modules, const QString& hightlightKey = QString::null, const KeyTreeItem::Settings settings = KeyTreeItem::Settings() );
	
	const QString renderSingleKey( const QString& key, const ListCSwordModuleInfo&, const KeyTreeItem::Settings settings = KeyTreeItem::Settings() );
	
protected:
	virtual const QString renderEntry( const KeyTreeItem&, CSwordKey* = 0 ) = 0;
	virtual const QString finishText( const QString&, KeyTree& tree ) = 0;
	virtual void initRendering() = 0;
};

inline CTextRendering::KeyTree* const CTextRendering::KeyTreeItem::childList() const {
	if (!m_childList) {
		m_childList = new KeyTree();
	}
		
	return m_childList;
};

inline const bool CTextRendering::KeyTreeItem::hasChildItems() const {
	if (!m_childList)
		return false;
		
	return m_childList->isEmpty();
}
 
}

#endif
