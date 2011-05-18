/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef TABS_TABS_H
#define TABS_TABS_H

/*
 * autor
 * Michal Podsiadlik
 * michal at gov.one.pl
 */

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include "chat/chat-manager.h"

#include "configuration/configuration-aware-object.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/misc.h"
#include "storage/storable-object.h"

#include "tabwidget.h"

class QAction;
class QMenu;
class Action;
class ActionDescription;

class TabsManager : public ConfigurationUiHandler, ConfigurationAwareObject, StorableObject
{
	Q_OBJECT

	void createDefaultConfiguration();

	ActionDescription *OpenInNewTabActionDescription;
	ActionDescription *AttachToTabsActionDescription;
	TabWidget *TabDialog;
	QTimer Timer;
	QList<ChatWidget *> ChatsWithNewMessages;
	QList<ChatWidget *> NewChats;
	QList<ChatWidget *> DetachedChats;
	bool NoTabs;
	bool ForceTabs;

	int TargetTabs;
	void insertTab(ChatWidget *chat);
	void makePopupMenu();
	ChatWidget *SelectedChat;
	QMenu *Menu;
	QAction *DetachTabMenuAction;
	QAction *CloseTabMenuAction;

	QString formatTabName(ChatWidget *chatWidget);

	/**
	* Zmienne konfiguracyjne.
	*/
	bool ConfigConferencesInTabs;
	bool ConfigTabsBelowChats;
	bool ConfigDefaultTabs;
	int ConfigMinTabs;
	bool ConfigBlinkChatTitle;
	bool ConfigShowNewMessagesNum;

private slots:
	void onTimer();
	void onContextMenu(QWidget *w, const QPoint &pos);
	void onMenuActionDetach();
	void onMenuActionDetachAll();
	void onMenuActionClose();
	void onMenuActionCloseAll();
	void onMessageReceived(Chat chat);

protected:
	virtual void configurationUpdated();
	virtual void load();

public:
	explicit TabsManager(QObject *parent = 0);
	virtual ~TabsManager();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	bool detachChat(ChatWidget *chat);

	virtual void store();

	virtual StorableObject * storageParent() { return 0; }
	virtual QString storageNodeName() { return QLatin1String("ModuleTabs"); }

public slots:
	void onNewChat(ChatWidget *chat, bool &handled);
	void onDestroyingChat(ChatWidget *chat);
	void onOpenChat(ChatWidget *chat, bool activate);
	void onIconChanged();
	void onTitleChanged(ChatWidget *chatChanged, const QString &newTitle);

	void onTabChange(int index);

	void onNewTab(QAction *sender, bool toggled);

	void openTabWith(QStringList altnicks, int index);

	void onTabAttach(QAction *sender, bool toggled);

	void attachToTabsActionCreated(Action *action);

	void closeChat();

signals:
	void chatWidgetActivated(ChatWidget *);
};

#endif // TABS_TABS_H
