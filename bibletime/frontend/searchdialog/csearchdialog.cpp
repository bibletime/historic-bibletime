/***************************************************************************
                          csearchdialog.cpp  -  description
                             -------------------
    begin                : Die Jul 30 2002
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

#include "csearchdialog.h"

#include "resource.h"
#include "whatsthisdef.h"
#include "tooltipdef.h"

#include "backend/cswordkey.h"
#include "backend/cswordversekey.h"

//Qt includes
#include <qhbox.h>
#include <qptrlist.h>
#include <qpainter.h>
#include <qwhatsthis.h>
#include <qlayout.h>

//KDE includes
#include <kapplication.h>
#include <klocale.h>

#define SPACE_BETWEEN_PARTS 5
#define RIGHT_BORDER 15
#define LEFT_BORDER 15
#define LOWER_BORDER 10
#define UPPER_BORDER 10

#define ITEM_TEXT_SIZE 8
#define LABEL_TEXT_SIZE 6

//used for the shift between the bars
#define BAR_DELTAX 4
#define BAR_DELTAY 2
#define BAR_WIDTH 2+2*BAR_DELTAX  //should be equal or bigger than the label font size
//used for the text below the bars
#define BAR_LOWER_BORDER 100

#define LEGEND_INNER_BORDER 5
#define LEGEND_DELTAY 4
#define LEGEND_WIDTH 85

CSearchDialog::CSearchDialog(QWidget *parent)
  : KDialogBase(Tabbed, i18n("Search dialog"), Close | User1 | User2, User1, parent, "CSearchDialog", false, true, i18n("Search"), i18n("Interrupt")) {

	setIcon(MODULE_SEARCH_ICON_SMALL);
	m_searcher.connectPercentUpdate(this, SLOT(percentUpdate()));
	m_searcher.connectFinished(this, SLOT(searchFinished()));

  initView();
  initConnections();
}

CSearchDialog::~CSearchDialog(){
}

/** Starts the search with the set modules and the set search text. */
void CSearchDialog::startSearch(){
  qWarning("CSearchDialog::startSearch()");
  m_searchResultPage->reset();
 	enableButton(User1,false);
	enableButton(User2,true);
  m_interruptedSearch = false;

  int searchFlags = m_searchOptionsPage->searchFlags();	
  m_searchOptionsPage->prepareSearch();

	m_searcher.setModules( modules() );
	m_searcher.setSearchedText(m_searchOptionsPage->searchText());
	m_searcher.setSearchOptions(searchFlags);
	
  m_searcher.startSearchThread();
}

/** Starts the search with the given module list and given search text. */
void CSearchDialog::startSearch( const ListCSwordModuleInfo modules, const QString& searchText){
  m_searchResultPage->reset();
  m_searchOptionsPage->reset();
  setModules(modules);
  setSearchText(searchText);

  startSearch();
}

/** Returns the list of used modules. */
const ListCSwordModuleInfo CSearchDialog::modules(){
  return m_searchOptionsPage->modules();
}

/** Sets the list of modules for the search. */
void CSearchDialog::setModules( const ListCSwordModuleInfo modules ){
  m_searchOptionsPage->setModules(modules);
}

/** Returns the search text which is set currently. */
const QString CSearchDialog::searchText(){
  return m_searchOptionsPage->searchText();
}

/** Returns the search text which is used for the search. */
void CSearchDialog::setSearchText( const QString searchText ){
  m_searchOptionsPage->setSearchText(searchText);
}

/** Initializes this object. */
void CSearchDialog::initView(){
  QHBox* box = addHBoxPage(i18n("Options"));
  m_index.optionsPage = pageIndex(box);
  m_searchOptionsPage = new CSearchOptionsPage(box);

  box = addHBoxPage(i18n("Search result"));
  m_index.resultPage = pageIndex(box);
  m_searchResultPage = new CSearchResultPage(box);
}

/** Updates the percentage bars. */
void CSearchDialog::percentUpdate(){
  updateProgress();
}

/** Updates the percentage bars. */
void CSearchDialog::searchFinished(){
//  qWarning("CSearchDialog::searchFinished()");
 	m_searchOptionsPage->setOverallProgress(100);
 	m_searchOptionsPage->setCurrentModuleProgress(100);

 	if ( !m_interruptedSearch && m_searcher.foundItems() ){
 		m_searchResultPage->setSearchResult(modules());
    showPage(m_index.resultPage);
 	}
 	else
 		m_searchResultPage->reset();

 	enableButton(User2,false);
 	enableButton(User1,true); 	
}

/** Initializes the signal slot connections */
void CSearchDialog::initConnections(){
  connect(this, SIGNAL(user1Clicked()), SLOT(startSearch()));
  connect(this, SIGNAL(user2Clicked()), SLOT(interruptSearch()));
}

/** Updates the progress. */
void CSearchDialog::updateProgress(){
  KApplication::kApplication()->processEvents();
  m_searchOptionsPage->setOverallProgress(m_searcher.getPercent(CSwordModuleSearch::allModules));
  m_searchOptionsPage->setCurrentModuleProgress(m_searcher.getPercent(CSwordModuleSearch::currentModule));
}

/** Interrupts the search. */
void CSearchDialog::interruptSearch(){
  m_interruptedSearch = true;
  m_searcher.interruptSearch();
  m_searchOptionsPage->prepareSearch();
}

/** Resets the parts to the default. */
void CSearchDialog::reset(){
  m_searchOptionsPage->reset();
  m_searchResultPage->reset();
  showPage(m_index.optionsPage);
}

/****************************/
/****************************/
/****************************/

CModuleChooser::ModuleCheckBoxItem::ModuleCheckBoxItem(CTreeFolder* item, CSwordModuleInfo* module) : CModuleItem(item, module) {

};

CModuleChooser::ModuleCheckBoxItem::~ModuleCheckBoxItem() {

};

/****************************/
/****************************/
/****************************/

CModuleChooser::CModuleChooser(QWidget* parent) : CMainIndex(parent) {

};

CModuleChooser::~CModuleChooser() {

};

/** Reimplementation to add our special checkbox items to the list. */
void CModuleChooser::addGroup(const CItemBase::Type type, const QString language){
  if (type == CItemBase::Module) {
//    CItemBase* i = new ModuleCheckBoxItem(this, );
  }
  else if (type == CItemBase::BookmarkFolder) {

  }
  else
    CMainIndex::addGroup(type, language);
}

/****************************/

CModuleChooserDialog::CModuleChooserDialog( QWidget* parentDialog, ListCSwordModuleInfo modules ) : KDialogBase(Plain, i18n("Choose modules ..."), Close, Close, parentDialog, "CModuleChooser", false, true) {
  initView();
  initConnections();
};

CModuleChooserDialog::~CModuleChooserDialog() {

};

/** Initializes the view of this dialog */
void CModuleChooserDialog::initView(){
  QFrame* page = plainPage();
  QHBoxLayout* layout = new QHBoxLayout(page);

  m_moduleChooser = new CModuleChooser(page);
  m_moduleChooser->initTree();

  layout->addWidget(m_moduleChooser);
}

/** Initializes the connections of this dialog. */
void CModuleChooserDialog::initConnections(){

}

/****************************/

CRangeChooserDialog::CRangeChooserDialog( QWidget* parentDialog ) : KDialog(parentDialog) {

};

CRangeChooserDialog::~CRangeChooserDialog() {

};


/****************************/

CSearchAnalysisDialog::CSearchAnalysisDialog( ListCSwordModuleInfo modules, QWidget* parentDialog ) : KDialogBase(Plain, i18n("Search analysis"), Close, Close, parentDialog, 0, true){
  initView();
  initConnections();
  m_analysis->reset();
  m_analysis->analyse(modules);
};

CSearchAnalysisDialog::~CSearchAnalysisDialog() {

};

/** Initializes this dialog. */
void CSearchAnalysisDialog::initView(){
  QHBoxLayout* layout = new QHBoxLayout(plainPage());
  m_analysis = new CSearchAnalysis(plainPage());
  m_analysisView = new CSearchAnalysisView(m_analysis, plainPage());
  m_analysisView->show();
  layout->addWidget(m_analysisView);

  resize(600,400);
}

/** Initializes the widgets SIGNAL and SLOT connections,. */
void CSearchAnalysisDialog::initConnections(){
}

/****************************/
/*  CSearchAnalysis         */
/****************************/

CSearchAnalysis::CSearchAnalysis(QObject *parent, const char *name )
	: QCanvas(parent,name) {

  m_scaleFactor = 0.0;
  m_legend = 0;
	setBackgroundColor(Qt::white);	
	m_canvasItemList.resize(67);
	m_canvasItemList.setAutoDelete(true);
	resize(1,1);
	connect(this, SIGNAL(resized()), SLOT(slotResized()));
}

CSearchAnalysis::~CSearchAnalysis(){
}

QDict<CSearchAnalysisItem>* CSearchAnalysis::getSearchAnalysisItemList(){
	// Returns pointer to the search analysis items
	return &m_canvasItemList;
}

/** Starts the analysis of the search result. This should be called only once because QCanvas handles the updates automatically. */
void CSearchAnalysis::analyse(ListCSwordModuleInfo modules){
	qDebug("void CSearchAnalysis::analyse()");
  /**
	* Steps of analysing our search result;
	*	-Create the items for all available books ("Genesis" - "Revelation")
	* -Iterate through all modules we analyse
	*		-Go through all books of this module
	*			-Find out how many times we found the book
	*			-Set the count to the items which belongs to the book
	*/
  setModules(modules);

	m_lastPosList.clear();		
	const int numberOfModules = m_moduleList.count();
	if (!numberOfModules)
		return;	
	m_legend = new CSearchAnalysisLegendItem(this, &m_moduleList);	
	m_legend->setX(LEFT_BORDER);
	m_legend->setY(UPPER_BORDER);
	m_legend->setSize(LEGEND_WIDTH,
	           LEGEND_INNER_BORDER*2 + ITEM_TEXT_SIZE*numberOfModules + LEGEND_DELTAY*(numberOfModules-1));
  m_legend->show();	

  int xPos = LEFT_BORDER + m_legend->width() + SPACE_BETWEEN_PARTS;			
	int moduleIndex = 0;	
	m_maxCount = 0;	
	int count = 0;
	CSwordVerseKey key(0);	
	key.key("Genesis 1:1");	
	
	CSearchAnalysisItem* analysisItem = m_canvasItemList[key.book()];
	bool ok = true;
	while (ok && analysisItem) {
		for (moduleIndex = 0,m_moduleList.first(); m_moduleList.current(); m_moduleList.next(),++moduleIndex) {
			KApplication::kApplication()->processEvents(10);
			if (!m_lastPosList.contains(m_moduleList.current()))
				m_lastPosList.insert(m_moduleList.current(),0);
			analysisItem->setCountForModule(moduleIndex, (count = getCount(key.book(),m_moduleList.current())));
			m_maxCount = (count > m_maxCount) ? count : m_maxCount;
		}
		analysisItem->setX(xPos);
		analysisItem->setY(UPPER_BORDER);
		analysisItem->show();
		
		xPos += (int)analysisItem->width() + SPACE_BETWEEN_PARTS;
		ok = key.next(CSwordVerseKey::UseBook);		
   	analysisItem = m_canvasItemList[key.book()];
	}
	resize(xPos+BAR_WIDTH+(m_moduleList.count()-1)*BAR_DELTAX+RIGHT_BORDER, height() );	
	slotResized();
}

/** Sets te module list used for the analysis. */
void CSearchAnalysis::setModules(ListCSwordModuleInfo modules){
//	ASSERT(modules);
	m_moduleList.clear();
	for (modules.first(); modules.current(); modules.next())
		if ( modules.current()->type() == CSwordModuleInfo::Bible || modules.current()->type() == CSwordModuleInfo::Commentary )//a Bible or an commentary
			m_moduleList.append(modules.current());

	m_canvasItemList.clear();
	CSearchAnalysisItem* analysisItem = 0;	
	CSwordVerseKey key(0);	
	key.key("Genesis 1:1");
	do {
   	analysisItem = new CSearchAnalysisItem(this, m_moduleList.count(), key.book(), &m_scaleFactor, &m_moduleList);
   	analysisItem->hide();
		m_canvasItemList.insert(key.book(), analysisItem);
	} while (key.next(CSwordVerseKey::UseBook));
	update();
}

/** Sets back the items and deletes things to cleanup */
void CSearchAnalysis::reset(){
	m_scaleFactor = 0.0;
  QDictIterator<CSearchAnalysisItem> it( m_canvasItemList ); // iterator for items
	while ( it.current() ) {
		it.current()->hide();
		++it;
	}	
	m_lastPosList.clear();	
	
	if (m_legend)
		m_legend->hide();
	delete m_legend;
	m_legend = 0;	
	update();
}

/** No descriptions */
void CSearchAnalysis::slotResized(){
	m_scaleFactor = (double)( (double)(height()-UPPER_BORDER-LOWER_BORDER-BAR_LOWER_BORDER-(m_moduleList.count()-1)*BAR_DELTAY)
	                                    /(double)m_maxCount);	
	QDictIterator<CSearchAnalysisItem> it( m_canvasItemList );
	while ( it.current() ) {
		it.current()->setSize(BAR_WIDTH + (m_moduleList.count()-1)*BAR_DELTAX, height()-UPPER_BORDER-LOWER_BORDER);
		it.current()->setY(UPPER_BORDER);
  	++it;
	}
	update();
}

/** This function returns a color for each module */
QColor CSearchAnalysis::getColor(int index){
  switch (index){
    case  0: return Qt::red;
    case 	1: return Qt::darkGreen;
    case  2: return Qt::blue;
    case  3: return Qt::cyan;
    case  4: return Qt::magenta;
    case  5: return Qt::darkRed;
    case  6: return Qt::darkGray;
    case  7: return Qt::black;
    case  8: return Qt::darkCyan;
    case  9: return Qt::darkMagenta;
    default: return Qt::red;
  }
}

/** Returns the count of the book in the module */
const unsigned int CSearchAnalysis::getCount( const QString book, CSwordModuleInfo* module ){
	ListKey& result = module->searchResult();
	const int length = book.length();	
	unsigned int i = m_lastPosList[module];
	unsigned int count = 0;
	const unsigned int resultCount = result.Count();
	while (i < resultCount) {
		if ( strncmp(book.local8Bit(), (const char*)*result.GetElement(i), length) )		
			break;
		i++;
		++count;		
	}
	m_lastPosList.contains(module) ? m_lastPosList.replace(module,i) : m_lastPosList.insert(module,i);
//	m_lastPosList.replace(module,i);
	return count;
}


//------------------------------------------------------------------
//------------------------------------------------------------------

CSearchAnalysisItem::CSearchAnalysisItem(QCanvas *parent, const int moduleCount, const QString &bookname, double *scaleFactor, ListCSwordModuleInfo* modules)
	: QCanvasRectangle(parent),
	m_moduleList( modules ),
	m_scaleFactor(scaleFactor),
	m_bookName(bookname),
	m_moduleCount(moduleCount), 	
	m_bufferPixmap(0)
{	
 	m_resultCountArray.resize(m_moduleCount);
 	int index = 0;
 	for (index = 0; index < m_moduleCount; ++index)
 		m_resultCountArray[index] = 0;
}

CSearchAnalysisItem::~CSearchAnalysisItem() {
//	qWarning("CSearchAnalysisItem::~CSearchAnalysisItem()");
//	if (m_bufferPixmap)
		delete m_bufferPixmap;
}

/** Sets the resultcount of this item for the given module */
void CSearchAnalysisItem::setCountForModule( const int moduleIndex, const int count) {
	m_resultCountArray[moduleIndex] = count;
}

/** Returns the resultcount of this item for the given module */
int CSearchAnalysisItem::getCountForModule( const int moduleIndex) {
	return m_resultCountArray[moduleIndex];
}

/** Reimplementation. Draws the content of this item. */
void CSearchAnalysisItem::draw(QPainter& painter) {
	QFont f = painter.font();
	f.setPointSize(ITEM_TEXT_SIZE);
	painter.setFont(f);
	
	setPen(QPen(black,1));
	setBrush(Qt::red);
  /**	
	* We have to paint so many bars as we have modules available (we use m_moduleCount)
	* We paint inside the area which is given by height and widt of this rectangle item
	*/	
	int index = 0;	
	int drawn = 0;
	int Value = 0;
	
	//find out the biggest value
	for (index=0;index < m_moduleCount; index++)
	  if (m_resultCountArray[index] > Value)
	    Value = m_resultCountArray[index];
	
	while (drawn < m_moduleCount){
    for (index=0; index < m_moduleCount; index++){
      if (m_resultCountArray[index] == Value){
    		QPoint p1(x()+(m_moduleCount-drawn-1)*BAR_DELTAX,
    		          height()+y()-BAR_LOWER_BORDER-(m_moduleCount-drawn)*BAR_DELTAY);
    		QPoint p2(p1.x() + BAR_WIDTH,
    		          p1.y() - (!m_resultCountArray[index] ? 0 : ((m_resultCountArray[index])*(*m_scaleFactor))) );
    		QRect r(p1, p2);
    		painter.fillRect(r, QBrush(CSearchAnalysis::getColor(index)) );
    		painter.drawRect(r);
    		drawn++;
      }
    }
    //finds the next smaller value
 	  int newValue = 0;
 		for (index=0;index < m_moduleCount; index++)
   	  if (m_resultCountArray[index] < Value && m_resultCountArray[index] >= newValue)
   	    newValue = m_resultCountArray[index];
   	Value = newValue;
	}		
	if (!m_bufferPixmap) {
		m_bufferPixmap = new QPixmap();
		m_bufferPixmap->resize(width(),BAR_LOWER_BORDER);
		m_bufferPixmap->fill();		
		QPainter p(m_bufferPixmap);				
		f = p.font();
		f.setPointSize(ITEM_TEXT_SIZE);
		p.setFont(f);		
		p.rotate(90);
		p.drawText(QPoint(5,0), m_bookName);
	}
	painter.drawPixmap(QPoint(x(),height()+y()-BAR_LOWER_BORDER), *m_bufferPixmap);
}

/** Returns the width of this item. */
int CSearchAnalysisItem::width(){
	return m_moduleCount*(m_moduleCount>1 ? BAR_DELTAX : 0) + BAR_WIDTH;	
}

/** Returns the tooltip for this item. */
const QString CSearchAnalysisItem::getToolTip(){
	QString ret = QString::fromLatin1("<CENTER><B>%1</B></CENTER><HR>").arg(m_bookName);
	ret.append("<TABLE CELLPADDING=\"3\" WIDTH=\"100%\" ALIGN=\"center\">");
	for (int i = 0; i < m_moduleCount; ++i) {
		CSwordModuleInfo* info = m_moduleList->at(i);
		const QColor c = CSearchAnalysis::getColor(i);
		ret.append(
			QString::fromLatin1("<TR BGCOLOR=\"white\"><TD><B><FONT COLOR=\"#%1\">%2</FONT></B></TD><TD>%3</TD></TR>")
				.arg(QString().sprintf("%02X%02X%02X",c.red(),c.green(),c.blue()))
				.arg(info ? info->name() : QString::null)
				.arg(m_resultCountArray[i])
		);
	}
	ret.append("</TABLE>");			
	return ret;
}

//------------------------------------------------------------------
//------------------------------------------------------------------

CSearchAnalysisView::CSearchAnalysisView(QCanvas* canvas, QWidget* parent)
	: QCanvasView(canvas, parent) {
	QWhatsThis::add(this, WT_SD_ANALYSIS_VIEW);
	setFocusPolicy(QWidget::WheelFocus);
	m_toolTip = new ToolTip(this);	
	resize(sizeHint());

}

/** Returns the sizeHint for this view */
QSize CSearchAnalysisView::sizeHint(){
	if ( parentWidget() )
		return parentWidget()->sizeHint();
	return QCanvasView::sizeHint();
}

/** No descriptions */
void CSearchAnalysisView::resizeEvent( QResizeEvent* e){
	QCanvasView::resizeEvent(e);
	canvas()->resize( canvas()->width(), viewport()->height() );
}

CSearchAnalysisView::ToolTip::ToolTip(QWidget* parent) : QToolTip(parent) {
}

void CSearchAnalysisView::ToolTip::maybeTip(const QPoint& p) {
	CSearchAnalysisView* view = dynamic_cast<CSearchAnalysisView*>(parentWidget());
	if (!view)
		return;
	QPoint point(p);
	point = view->viewport()->mapFrom(view, point);
	CSearchAnalysisItem* i = view->itemAt( view->viewportToContents(point) );
	if (!i)
		return;
				
	//get type of item and display correct text
	QString text = i->getToolTip();
	if (text.isEmpty())
		return;
	
	QPoint p1 = view->viewport()->mapTo(view, view->contentsToViewport(i->rect().topLeft()));
	p1.setY(0);	
	QPoint p2 = view->viewport()->mapTo(view, view->contentsToViewport(i->rect().bottomRight()));
	p2.setY(view->height());	
	QRect r = QRect( p1, p2 );	
	if (r.contains(p))
		tip(r, text);
}


/** Returns the item at position p. If there no item at that point return 0. */
CSearchAnalysisItem* CSearchAnalysisView::itemAt( const QPoint& p ){
	QCanvasItemList l = canvas()->collisions(p);
	if (!l.count())
		return 0;
	return dynamic_cast<CSearchAnalysisItem*>(l.first());	
}

//------------------------------------------------------------------
//------------------------------------------------------------------

CSearchAnalysisLegendItem::CSearchAnalysisLegendItem(QCanvas *parent, ListCSwordModuleInfo *list )
	: QCanvasRectangle(parent) {
	m_moduleList = list;
}

/** Reimplementation. Draws the content of this item. */
void CSearchAnalysisLegendItem::draw (QPainter& painter) {	
 	painter.save();
 		
	setPen( QPen(black,2) );
	setBrush( Qt::white );
  //the outer rectangle
  QPoint p1(x(),y());
  QPoint p2(x()+width(), y()+height());
  QRect r(p1, p2);
  r.normalize();
 	painter.drawRect(r);
 	
  QFont f = painter.font();
  f.setPointSize(ITEM_TEXT_SIZE);
  painter.setFont(f);
 	
 	for (unsigned int index=0; index < m_moduleList->count(); index++){
 	  // the module color indicators
 	  QPoint p1(x()+LEGEND_INNER_BORDER, y() + LEGEND_INNER_BORDER + index*(LEGEND_DELTAY + ITEM_TEXT_SIZE));
 	  QPoint p2( p1.x() + ITEM_TEXT_SIZE, p1.y() + ITEM_TEXT_SIZE);
 	  QRect r(p1,p2);
 		painter.fillRect(r, QBrush(CSearchAnalysis::getColor(index)) );
 		r.normalize();
 		painter.drawRect(r);
 		
 		QPoint p3( p2.x() + LEGEND_INNER_BORDER, p2.y() );
  	painter.drawText(p3, m_moduleList->at(index)->name() );
 	}
  painter.restore();
}
