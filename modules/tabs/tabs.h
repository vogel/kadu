/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

class QMenu;
class Action;
class ActionDescription;

class TabsManager : public ConfigurationUiHandler, ConfigurationAwareObject, StorableObject
{
	Q_OBJECT

		void createDefaultConfiguration();

		ActionDescription *openInNewTabActionDescription;
		ActionDescription *attachToTabsActionDescription;
		TabWidget *tabdialog;
		QTimer timer;
		QList<ChatWidget *> chatsWithNewMessages, newchats, detachedchats;
		bool no_tabs, autoswith, force_tabs;

		int target_tabs;
		void insertTab(ChatWidget *chat);
		void makePopupMenu();
		int menuitem;
		ChatWidget *selectedchat;
		QMenu *menu;
		Action *action;

		void repaintTabs();

		QString formatTabName(ChatWidget * chatWidget);

		void refreshTab(int tabIndex, ChatWidget * chatWidget);

		/**
		* Zmienne konfiguracyjne.
		*/
		bool config_conferencesInTabs;
		bool config_tabsBelowChats;
		bool config_autoTabChange;
		bool config_defaultTabs;
		int config_minTabs;
		bool config_closeButtonOnTab;
		bool config_blinkChatTitle;
		bool config_showNewMessagesNum;

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
		TabsManager(bool firstload);
		virtual ~TabsManager();

		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

		bool detachChat(ChatWidget *chat);

		virtual void store();

		virtual StorableObject * storageParent() { return 0; }
		virtual QString storageNodeName() { return QLatin1String("ModuleTabs"); }

	public slots:
		void onNewChat(ChatWidget *chat, bool &handled);
		void onDestroyingChat(ChatWidget *chat);

		void onOpenChat(ChatWidget *chat);
		void onTitleChanged(Chat chatChanged, const QString &newTitle);

		void onTabChange(int index);

		void onNewTab(QAction *sender, bool toggled);

		void openTabWith(QStringList altnicks, int index);

		void onTabAttach(QAction *sender, bool toggled);

		void attachToTabsActionCreated(Action *action);

		void closeChat();

	signals:
		void chatWidgetActivated(ChatWidget *);
};

extern TabsManager *tabs_manager;
#endif
