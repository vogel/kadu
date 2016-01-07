/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QRect>
#include <injeqt/injector.h>

#include "configuration/configuration-aware-object.h"
#include "os/generic/compositing-aware-object.h"

class QAction;
class QString;
class QWidget;

class Configuration;
class Docking;
class GroupTabBar;
class KaduWindow;
class MainWindow;
class StatusButtons;
class TalkableTreeView;

class SimpleView : public QObject, private ConfigurationAwareObject, CompositingAwareObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit SimpleView(QObject *parent = nullptr);
	virtual ~SimpleView();

public slots:
	void simpleViewToggle(bool activate);

private:
	QPointer<Configuration> m_configuration;
	QPointer<Docking> m_docking;

	bool KeepSize;
	bool NoScrollBar;
	bool SimpleViewActive;
	bool Borderless;

	QRect DiffRect;
	QString BuddiesListViewStyle;

	QAction *DockAction;
	KaduWindow *KaduWindowHandle;
	MainWindow *MainWindowHandle;
	GroupTabBar *GroupTabBarHandle;
	StatusButtons *StatusButtonsHandle;
	TalkableTreeView *TalkableTreeViewHandle;

	virtual void compositingEnabled();
	virtual void compositingDisabled();
	virtual void configurationUpdated();

private slots:
	INJEQT_INIT void init();
	INJEQT_DONE void done();
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setDocking(Docking *docking);

};
