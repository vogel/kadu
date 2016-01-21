/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/actions/action-context-provider.h"
#include "gui/actions/action-context.h"
#include "gui/actions/action-description.h"
#include "os/generic/desktop-aware-object.h"
#include "exports.h"

#include <QtCore/QPointer>
#include <QtWidgets/QMainWindow>
#include <QtXml/QDomElement>
#include <injeqt/injeqt.h>

class QContextMenuEvent;

class Actions;
class Buddy;
class BuddySet;
class Chat;
class ConfigurationManager;
class Configuration;
class Contact;
class ContactSet;
class InjectedFactory;
class StatusContainer;
class TalkableProxyModel;
class ToolBar;

class KADUAPI MainWindow : public QMainWindow, public ActionContextProvider, DesktopAwareObject
{
	Q_OBJECT

public:
	static MainWindow * findMainWindow(QWidget *widget);

	MainWindow(ActionContext *context, const QString &windowName, QWidget *parent);
	virtual ~MainWindow();

	const QString & windowName() { return WindowName; }

	virtual QMenu * createPopupMenu() { return 0; }

	virtual bool supportsActionType(ActionDescription::ActionType type) = 0;
	virtual TalkableProxyModel * talkableProxyModel() = 0;

	Contact contact();
	Buddy buddy();

	bool hasAction(const QString &actionName, ToolBar *exclude = 0);

	// ActionContextProvider implementation
	ActionContext * actionContext() override;

public slots:
	void addTopToolbar();
	void addBottomToolbar();
	void addLeftToolbar();
	void addRightToolbar();

protected:
	InjectedFactory * injectedFactory() const;

	void loadToolBarsFromConfig();
	void loadToolBarsFromConfig(Qt::ToolBarArea area);

	bool loadOldToolBarsFromConfig(const QString &configName, Qt::ToolBarArea area);

	void writeToolBarsToConfig();
	void writeToolBarsToConfig(Qt::ToolBarArea area);

	QDomElement getDockAreaConfigElement(Qt::ToolBarArea area);

	static QDomElement getToolbarsConfigElement(Configuration *configuration);
	static QDomElement getDockAreaConfigElement(Configuration *configuration, QDomElement toolbarsConfig, const QString &name);
	static void addToolButton(Configuration *configuration, QDomElement toolbarConfig, const QString &actionName, Qt::ToolButtonStyle style = Qt::ToolButtonIconOnly);
	static QDomElement findExistingToolbarOnArea(Configuration *configuration, const QString &areaName);
	static QDomElement findExistingToolbar(Configuration *configuration, const QString &prefix);

	void setTransparency(bool enable);
	void setBlur(bool enable);
	virtual void showEvent (QShowEvent * event);

	virtual void contextMenuEvent(QContextMenuEvent *event);
	Configuration * configuration() const;

protected slots:
	void refreshToolBars();

private:
	QPointer<Actions> m_actions;
	QPointer<ConfigurationManager> m_configurationManager;
	QPointer<Configuration> m_configuration;
	QPointer<InjectedFactory> m_injectedFactory;

	QString WindowName;
	bool TransparencyEnabled;
	bool BlurEnabled;

	ActionContext *Context;

	ToolBar * newToolbar(QWidget *parent);

	void loadToolBarsFromConfigNode(QDomElement dockareaConfig, Qt::ToolBarArea area);

private slots:
	INJEQT_SET void setActions(Actions *actions);
	INJEQT_SET void setConfigurationManager(ConfigurationManager *configurationManager);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_INIT void init();

	void actionLoadedOrUnloaded(ActionDescription *action);
	void toolbarUpdated();
	void toolbarRemoved(ToolBar *toolBar);

};
