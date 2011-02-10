/*
 * %kadu copyright begin%
 * Copyright 2010 Przemysław Rudy (prudy1@o2.pl)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SIMPLEVIEW_H
#define SIMPLEVIEW_H

#include <QtCore/QObject>
#include <QtCore/QRect>

#include "configuration/configuration-aware-object.h"
#include "gui/widgets/group-tab-bar.h"
#include "os/generic/compositing-aware-object.h"

class QAction;
class QString;
class QWidget;

class BuddiesListWidget;
class KaduWindow;
class MainWindow;
class StatusButtons;

class SimpleView :
	public QObject, private ConfigurationAwareObject, CompositingAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(SimpleView)

	static SimpleView *Instance;

	bool KeepSize;
	bool NoScrollBar;
	bool SimpleViewActive;
	bool Borderless;

	QRect DiffRect;
	QString BuddiesListViewStyle;

	QAction *DockAction;
	KaduWindow *KaduWindowHandle;
	MainWindow *MainWindowHandle;
	BuddiesListWidget *BuddiesListWidgetHandle;
	GroupTabBar *GroupTabBarHandle;
	StatusButtons *StatusButtonsHandle;

	SimpleView();
	virtual ~SimpleView();

	virtual void compositingEnabled();
	virtual void compositingDisabled();
	virtual void configurationUpdated();

public:
	static void createInstance();
	static void destroyInstance();
	static SimpleView *instance(){return Instance; }

public slots:
	void simpleViewToggle(bool activate);
};

#endif
