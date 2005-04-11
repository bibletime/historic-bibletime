/********* Read the file LICENSE for license details. *********/

#ifndef CHTMLWRITEWINDOW_H
#define CHTMLWRITEWINDOW_H

//BibleTime includes
#include "cplainwritewindow.h"

//Qt includes
#include <qwidget.h>

class KAction;

/** The WYSIWYG implementation of the editor.
  * @author The BibleTime team
  */
class CHTMLWriteWindow : public CPlainWriteWindow  {
   Q_OBJECT
public:
	CHTMLWriteWindow(ListCSwordModuleInfo modules, CMDIArea* parent, const char *name = 0);
	virtual ~CHTMLWriteWindow();

  /**
  * Store the settings of this window in the given CProfileWindow object.
  */
  virtual void storeProfileSettings( CProfileWindow* );
  /**
  * Store the settings of this window in the given profile window.
  */
  virtual void applyProfileSettings( CProfileWindow* );

	virtual const bool syncAllowed() const;

protected: // Protected methods
  /**
  * Initialize the state of this widget.
  */
  virtual void initView();
  virtual void initConnections();
  virtual void initToolbars();
  virtual const CDisplayWindow::WriteWindowType writeWindowType() {
    return CDisplayWindow::HTMLWindow;
  };

protected slots:
  /**
  * Is called when the current text was changed.
  */
  virtual void textChanged();
  /**
  * Loads the original text from the module.
  */
  virtual void restoreText();

private:
  struct {
    KAction* saveText;
    KAction* restoreText;
    KAction* deleteEntry;
		KToggleAction* syncWindow;
  } m_actions;
};

#endif
