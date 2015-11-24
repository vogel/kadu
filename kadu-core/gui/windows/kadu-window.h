/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010, 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef KADU_WINDOW_H
#define KADU_WINDOW_H

#include <QtCore/QMap>
#include <QtCore/QPair>

#include "buddies/buddy-list.h"
#include "chat/chat.h"
#include "gui/windows/main-window.h"
#include "os/generic/compositing-aware-object.h"
#include "talkable/talkable.h"
#include "exports.h"

class QSplitter;
class QMenu;
class QMenuBar;
class QVBoxLayout;

class ActionDescription;
class BuddyInfoPanel;
class ProxyActionContext;
class KaduWindowActions;
class RosterWidget;
class StatusButtons;
class TalkableTreeView;

class KADUAPI KaduWindow : public MainWindow, private ConfigurationAwareObject, CompositingAwareObject
{
	Q_OBJECT

public:
	enum MenuType
	{
		MenuKadu,
		MenuBuddies,
		MenuTools,
		MenuHelp
	};

private:
	bool Docked; // TODO: 0.11.0 it is a hack
	QSplitter *Split;

	KaduWindowActions *Actions;

	BuddyInfoPanel *InfoPanel;
	QMenu *KaduMenu;
	QMenu *ContactsMenu;
	QMenu *ToolsMenu;
	QMenu *HelpMenu;

	QWidget *MainWidget;
	QVBoxLayout *MainLayout;

	RosterWidget *Roster;
	StatusButtons *ChangeStatusButtons;
	QPoint LastPositionBeforeStatusMenuHide;

#ifdef Q_OS_WIN
	QWidget *HiddenParent;
#endif
	QWidget *WindowParent;

	bool CompositingEnabled;

	ProxyActionContext *Context;

	void createGui();

	void createMenu();
	void createKaduMenu();
	void createContactsMenu();
	void createToolsMenu();
	void createHelpMenu();

#ifdef Q_OS_WIN
	void hideWindowFromTaskbar();
#endif

	void storeConfiguration();

	virtual void compositingEnabled();
	virtual void compositingDisabled();

private slots:
#ifdef Q_OS_WIN
	void setHiddenParent();
#endif

protected:
	virtual void closeEvent(QCloseEvent *);
	virtual void keyPressEvent(QKeyEvent *);
	virtual void changeEvent(QEvent *event);

	virtual bool supportsActionType(ActionDescription::ActionType type);

	virtual void configurationUpdated();

public:
	static void createDefaultToolbars(QDomElement parentConfig);

	KaduWindow();
	virtual ~KaduWindow();

	virtual TalkableTreeView * talkableTreeView();
	virtual TalkableProxyModel * talkableProxyModel();

	KaduWindowActions * kaduWindowActions() const { return Actions; }

	void setDocked(bool);
	bool docked() { return Docked; }

	BuddyInfoPanel * infoPanel() { return InfoPanel; }

public slots:
	void talkableActivatedSlot(const Talkable &talkable);

signals:
	void keyPressed(QKeyEvent *e);
	void parentChanged(QWidget *oldParent);

	void talkableActivated(const Talkable &talkable);

};

#endif // KADU_WINDOW_H
