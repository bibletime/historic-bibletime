//
// C++ Implementation: ctextrendering
//
// Description:
//
//
// Author: The BibleTime team <info@bibletime.info>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ctextrendering.h"

//BibleTime includes
#include "backend/cswordkey.h"
#include "backend/cswordversekey.h"
#include "backend/cswordmoduleinfo.h"
#include "backend/cdisplaytemplatemgr.h"
#include "backend/creferencemanager.h"


#include "util/scoped_resource.h"


CTextRendering::KeyTreeItem::KeyTreeItem(const QString& key, CSwordModuleInfo const * mod, const Settings settings ) 
	: m_key(key),
		m_childList( 0 ),
		m_settings(settings)
{
	m_moduleList.append( mod );
}

CTextRendering::KeyTreeItem::KeyTreeItem(const QString& key, ListCSwordModuleInfo& mods, const Settings settings ) 
	: m_key(key),
		m_moduleList( mods ),
		m_childList( 0 ),
		m_settings(settings)
{
	
}

CTextRendering::KeyTreeItem::KeyTreeItem()
	: m_key(QString::null),
		m_childList( 0 )
{
}


CTextRendering::KeyTreeItem::~KeyTreeItem() {
	delete m_childList;
}


ListCSwordModuleInfo CTextRendering::KeyTree::collectModules() {
	//collect all modules which are available and used by child items
	ListCSwordModuleInfo modules;
	KeyTree::const_iterator it;
	for (it = begin(); it != end(); ++it) {
		ListCSwordModuleInfo childMods = (*it).modules();
		for (ListCSwordModuleInfo::const_iterator c_it = childMods.begin(); c_it != childMods.end(); ++c_it) {
			if (!modules.contains(*c_it)) {
				modules.append(*c_it);
			}
		}
	}
	
	return modules;
}

CTextRendering::CTextRendering() {
}


CTextRendering::~CTextRendering() {

}

const QString CTextRendering::renderKeyTree( KeyTree& tree ) {
	QString ret;
	
	for (KeyTree::const_iterator it = tree.begin(); it != tree.end(); ++it) {
		ret += renderEntry( *it );	
	}
	
	return finishText(ret, tree);
}

const QString CTextRendering::renderSingleKey( const QString& key, ListCSwordModuleInfo moduleList ) {
	KeyTree tree;
	KeyTreeItem::Settings settings;
	tree += KeyTreeItem(key, moduleList, settings);
	
	return renderKeyTree(tree);
}


CDisplayRendering::CDisplayRendering(CSwordBackend::DisplayOptions displayOptions, CSwordBackend::FilterOptions filterOptions) 
	: m_displayOptions(displayOptions),
		m_filterOptions(filterOptions)
{

}

CDisplayRendering::~CDisplayRendering() {

}

const QString CDisplayRendering::renderEntry( const KeyTreeItem& i ) {
	ListCSwordModuleInfo modules = i.modules();	
	util::scoped_ptr<CSwordKey> key( CSwordKey::createInstance(modules.first()) );
  QString renderedText = (modules.count() > 1) ? QString::fromLatin1("<tr>") : QString::null;

	qWarning("render entry for %s", i.key().latin1());
	// Only insert the table stuff if we are displaying parallel.
  // Otherwise, strip out he table stuff -> the whole chapter will be rendered in one cell!

  //declarations out of the loop for optimization
  QString entry;
  QString keyText;
  bool isRTL;

  for (CSwordModuleInfo* m = modules.first(); m; m = modules.next()) {
    key->module(m);
    key->key( i.key() );
    keyText = key->key();
    isRTL = (m->textDirection() == CSwordModuleInfo::RightToLeft);
		entry = QString::null;

		key->renderedText();
		int pvHeading = 0;
		do { //add sectiontitle before we add the versenumber
			QString preverseHeading = QString::fromUtf8(
m->module()->getEntryAttributes()["Heading"]["Preverse"][QString::number(pvHeading++).latin1()].c_str());
			if (!preverseHeading.isEmpty()) {
				entry += QString::fromLatin1("<div %1 class=\"sectiontitle\">%1</div>")
					.arg(m->language().isValid() 
						? QString::fromLatin1("lang=\"%1\"").arg(m->language().abbrev()) 
						: QString::null
					)
					.arg(preverseHeading);
			}
			else {
				break;
			}
		} while (true);

		entry += QString::fromLatin1("<%1 %2 %3 dir=\"%4\">") //linebreaks = div, without = span
    	.arg(m_displayOptions.lineBreaks ? QString::fromLatin1("div") : QString::fromLatin1("span"))
			.arg((modules.count() == 1) 
				? (i.settings().highlight ? QString::fromLatin1("class=\"currententry\"") : QString::fromLatin1("class=\"entry\"")) 
				: "") //insert only the class if we're not in a td
			.arg(m->language().isValid() 
				? QString::fromLatin1("lang=\"%1\"").arg(m->language().abbrev()) 
				: QString::null
			)
			.arg(isRTL ? QString::fromLatin1("rtl") : QString::fromLatin1("ltr"));

		entry += QString::fromLatin1("<span dir=\"%1\" class=\"entryname\">%2</span>")
			.arg(isRTL ? QString::fromLatin1("rtl") : QString::fromLatin1("ltr"))
			.arg( m_displayOptions.verseNumbers 
				? entryLink(i, m)
				: QString::null
			);
		
		entry += key->renderedText();
		
		entry +=  (m_displayOptions.lineBreaks 
			? QString::fromLatin1("</div>") 
			: QString::fromLatin1("</span>"));
		
  	if (modules.count() == 1) {
			renderedText += entry;
		}
  	else {
	    renderedText += QString::fromLatin1("<td class=\"%1\" %2 dir=\"%3\">%4</td>")
 				.arg(i.settings().highlight ? QString::fromLatin1("currententry") : QString::fromLatin1("entry"))
				.arg(m->language().isValid() 
					? QString::fromLatin1("lang=\"%1\"").arg(m->language().abbrev()) 
					: QString::null
				)
				.arg(isRTL ? QString::fromLatin1("rtl") : QString::fromLatin1("ltr"))
				.arg(entry);
		}
	}
 	if (modules.count() > 1) {
		renderedText += QString::fromLatin1("</tr>");
	}
  return renderedText;	
}

const QString CDisplayRendering::finishText( const QString& oldText, KeyTree& tree ) {
	ListCSwordModuleInfo modules = tree.collectModules();
	
	QString text;
	if (modules.count() > 1) {
		QString header;

		for (CSwordModuleInfo* m = modules.first(); m; m = modules.next()) {
			header += QString::fromLatin1("<th width=\"%1%\">%2</th>")
					.arg(100 / modules.count())
					.arg(m->name());
		}
		text = "<table><tr>" + header + "</tr>" + oldText + "</table>";
	}
	else {
		text = oldText;
	}

	QString langAbbrev = 
		((modules.count() == 1) && modules.first()->language().isValid())
		?	modules.first()->language().abbrev() 
		: "unknown";
	
	CDisplayTemplateMgr tMgr;
	return tMgr.fillTemplate(CBTConfig::get(CBTConfig::displayStyle), "title", text, CSwordModuleInfo::Bible, langAbbrev);
}


/*!
    \fn CDisplayRendering::entryLink( KeyTreeItem& item )
 */
const QString CDisplayRendering::entryLink( const KeyTreeItem& item, CSwordModuleInfo*  module ) {
//	ListCSwordModuleInfo mods = item.modules();	
//	CSwordModuleInfo* module = mods.first();
	Q_ASSERT(module);	
	
	QString linkText;
	
	if (module && module->type() == CSwordModuleInfo::Bible) {
		CSwordVerseKey vk(module);
		vk = item.key();
		
		linkText = QString::number(vk.Verse());
	}
	else {
		linkText = item.key();
	}
	
  if (linkText.isEmpty()) {
    return QString::fromLatin1("<a name=\"%1\"/>").arg(item.key());
  }
  else {
    return QString::fromLatin1("<a name=\"%1\" href=\"%2\">%3</a>")
      .arg(item.key())
      .arg(
				CReferenceManager::encodeHyperlink(module->name(), 
				item.key(), 
				CReferenceManager::typeFromModule(module->type()) )
			)
      .arg(linkText);
  }
	
	return QString::null;
}