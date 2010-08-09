#ifndef SIMPLEVIEW_H
#define SIMPLEVIEW_H

#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtCore/QSize>


#include "gui/widgets/buddies-list-widget.h"
#include "gui/widgets/buddy-info-panel.h"
#include "gui/widgets/buddies-list-view.h"
#include "gui/widgets/group-tab-bar.h"
#include "gui/widgets/status-buttons.h"
#include "gui/windows/main-window.h"
#include "gui/windows/kadu-window.h"
#include "os/generic/compositing-aware-object.h"

class QAction;

class SimpleView :
	public QObject, private CompositingAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(SimpleView)

	static SimpleView *Instance;
	QPoint BackupPosition;
	QSize BackupSize;
	QAction *DockAction;
	bool SimpleViewActive;
	KaduWindow *kaduWindow;
	MainWindow *mainWindow;
	BuddiesListView *buddiesListView;
	BuddiesListWidget *buddiesListWidget;
	GroupTabBar *groupTabBar;
	StatusButtons *statusButton;

	SimpleView();
	virtual ~SimpleView();

	virtual void compositingEnabled();
	virtual void compositingDisabled();

public:
	static void createInstance();
	static void destroyInstance();
	static SimpleView *instance(){return Instance; }

public slots:
	void simpleViewToggle();
};

#endif
