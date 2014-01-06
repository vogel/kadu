/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
	QTimer Timer;
	QList<ChatWidget *> NewChats;
	QList<ChatWidget *> DetachedChats;
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

	void updateTabName(ChatWidget *chatWidget);
	void updateTabIcon(ChatWidget *chatWidget);

	/**
	* Zmienne konfiguracyjne.
	*/
	bool ConfigConferencesInTabs;
	bool ConfigTabsBelowChats;
	bool ConfigDefaultTabs;
	int ConfigMinTabs;
	bool ConfigBlinkChatTitle;
	bool ConfigShowNewMessagesNum;

	ChatWidget * chatWidgetWithUnreadMessage() const;

	QString defaultChatTitle(const Chat &chat) const;
	QString shortChatTitle(const QString &chatTitle) const;

private slots:
	void onTimer();
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

	void updateTabTextAndTooltip(int index, const QString &text, const QString &tooltip);

	bool acceptChatWidget(ChatWidget *chatWidget) const;
	void addChatWidget(ChatWidget *chatWidget);
	void removeChatWidget(ChatWidget *chatWidget);

public slots:
	void onDestroyingChat(ChatWidget *chatWidget);
	void onIconChanged();
	void onTitleChanged(ChatWidget *chatWidget, const QString &newTitle);

	void onTabChange(int index);

	void onNewTab(QAction *sender, bool toggled);

	void onTabAttach(QAction *sender, bool toggled);

	void attachToTabsActionCreated(Action *action);

	void closeChat();
	void unreadMessagesCountChanged(ChatWidget *chatWidget);

signals:
	void chatWidgetActivated(ChatWidget *chatWidget);
	void chatWidgetAcceptanceChanged(ChatWidget *chatWidget);

};
