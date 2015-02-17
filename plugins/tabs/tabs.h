/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/widgets/tab-widget.h"

class QAction;
class QMenu;

class Action;
class ActionDescription;
class ChatWidget;
class ChatWidgetRepository;
class ChatWidgetSetTitle;

enum class OpenChatActivation;

class TabsManager : public ConfigurationUiHandler, ConfigurationAwareObject, StorableObject
{
	Q_OBJECT

	// just for fun, this code is so bad already
	// that one more friend class wont do a difference
	friend class TabWidget;

	QPointer<ChatWidgetRepository> m_chatWidgetRepository;

	void createDefaultConfiguration();

	ActionDescription *OpenInNewTabActionDescription;
	ActionDescription *AttachToTabsActionDescription;
	TabWidget *TabDialog;
	QList<Chat> DetachedChats;
	QList<Chat> ClosedChats;

	int TargetTabs;
	void insertTab(ChatWidget *chatWidget);
	void makePopupMenu();
	ChatWidget *SelectedChat;
	QMenu *Menu;
	QAction *DetachTabMenuAction;
	QAction *CloseTabMenuAction;
	QAction *CloseOtherTabsMenuAction;
	QAction *ReopenClosedTabMenuAction;
	ChatWidgetSetTitle *Title;

	void updateTabTitle(ChatWidget *chatWidget);

	/**
	* Zmienne konfiguracyjne.
	*/
	bool ConfigTabsBelowChats;
	bool ConfigDefaultTabs;

	void setConfiguration(ChatWidget *chatWidget);

private slots:
	void onContextMenu(QWidget *w, const QPoint &pos);
	void onMenuActionDetach();
	void onMenuActionDetachAll();
	void onMenuActionClose();
	void onMenuActionCloseAll();
	void onMenuActionCloseAllButActive();
	void reopenClosedChat();

protected:
	virtual void configurationUpdated();
	virtual void load();
	virtual void store();
	virtual bool shouldStore();

public:
	explicit TabsManager(QObject *parent = 0);
	virtual ~TabsManager();

	TabWidget * tabWidget() { return TabDialog; }

	void openStoredChatTabs();
	void storeOpenedChatTabs();

	void setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository);

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	void detachChat(ChatWidget *chatWidget);

	virtual StorableObject * storageParent() { return 0; }
	virtual QString storageNodeName() { return QLatin1String("ModuleTabs"); }

	bool acceptChat(Chat chat) const;
	ChatWidget * addChat(Chat chat, OpenChatActivation activation);
	void removeChat(Chat chat);

public slots:
	void onDestroyingChat(ChatWidget *chatWidget);
	void onTitleChanged(ChatWidget *chatWidget);
	void onTitleChanged();

	void onTabChange(int index);

	void onNewTab(QAction *sender, bool toggled);

	void onTabAttach(QAction *sender, bool toggled);

	void attachToTabsActionCreated(Action *action);

	void closeChat();

signals:
	void chatWidgetActivated(ChatWidget *chatWidget);
	void chatAcceptanceChanged(Chat chat);

};
