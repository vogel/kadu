/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy-list.h"
#include "buddies/buddy-search-criteria.h"
#include "buddies/buddy-set.h"
#include "chat/chat.h"
#include "gui/actions/action.h"
#include "gui/windows/main-window.h"
#include "misc/memory.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class AccountManager;
class Actions;
class AddFoundBuddyAction;
class BaseActionContext;
class BuddyManager;
class BuddyPreferredManager;
class ChatFoundAction;
class ChatManager;
class ChatStorage;
class ChatWidgetManager;
class ClearResultsAction;
class ContactManager;
class FirstSearchAction;
class IconsManager;
class InjectedFactory;
class KaduWindowService;
class NextResultsAction;
class SearchService;
class StopSearchAction;

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QTreeWidget;
class QTreeWidgetItem;

class KADUAPI SearchWindow : public MainWindow
{
	Q_OBJECT

public:
	static void createDefaultToolbars(Configuration *configuration, const QDomElement &toolbarsConfig);

	explicit SearchWindow(QWidget *parent = nullptr, Buddy buddy = Buddy::null);
	virtual ~SearchWindow();

	virtual bool supportsActionType(ActionDescription::ActionType type) { return (type & ActionDescription::TypeSearch); }
	virtual TalkableProxyModel * talkableProxyModel() { return 0; }

	void nextSearch();
	void stopSearch();
	void clearResults();

	void addFound();
	void chatFound();

public slots:
	void firstSearch();

protected:
	virtual void keyPressEvent(QKeyEvent *e);

private:
	friend class AddFoundBuddyAction;
	friend class ChatFoundAction;
	friend class ClearResultsAction;
	friend class FirstSearchAction;
	friend class NextResultsAction;
	friend class StopSearchAction;

	QPointer<AccountManager> m_accountManager;
	QPointer<Actions> m_actions;
	QPointer<AddFoundBuddyAction> m_addFoundBuddyAction;
	QPointer<BuddyManager> m_buddyManager;
	QPointer<BuddyPreferredManager> m_buddyPreferredManager;
	QPointer<ChatFoundAction> m_chatFoundAction;
	QPointer<ChatManager> m_chatManager;
	QPointer<ChatStorage> m_chatStorage;
	QPointer<ChatWidgetManager> m_chatWidgetManager;
	QPointer<ClearResultsAction> m_clearResultsAction;
	QPointer<ContactManager> m_contactManager;
	QPointer<FirstSearchAction> m_firstSearchAction;
	QPointer<IconsManager> m_iconsManager;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<KaduWindowService> m_kaduWindowService;
	QPointer<NextResultsAction> m_nextResultsAction;
	QPointer<StopSearchAction> m_stopSearchAction;

	Buddy m_buddy;
	Account CurrentAccount;
	SearchService *CurrentSearchService;

	owned_qptr<BuddySearchCriteria> CurrentSearchCriteria;

	QLineEdit *UinEdit;
	QLineEdit *FirstNameEdit;
	QLineEdit *LastNameEdit;
	QLineEdit *NickNameEdit;
	QLineEdit *StartBirthYearEdit;
	QLineEdit *EndBirthYearEdit;
	QLineEdit *CityEdit;
	QComboBox *GenderComboBox;
	QCheckBox *OnlyActiveCheckBox;
	QRadioButton *UinRadioButton;
	QRadioButton *PersonalDataRadioButton;
	QTreeWidget *ResultsListWidget;

	bool SearchInProgress;
	bool DoNotTransferFocus; // TODO: remove

	void createGui();

	QTreeWidgetItem * selectedItem() const;
	ContactSet selectedContacts() const;

	bool isPersonalDataEmpty() const;

	void setActionEnabled(ActionDescription *actionDescription, bool enable);

private slots:
	INJEQT_SET void setAccountManager(AccountManager *accountManager);
	INJEQT_SET void setActions(Actions *actions);
	INJEQT_SET void setAddFoundBuddyAction(AddFoundBuddyAction *addFoundBuddyAction);
	INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
	INJEQT_SET void setBuddyPreferredManager(BuddyPreferredManager *buddyPreferredManager);
	INJEQT_SET void setChatFoundAction(ChatFoundAction *chatFoundAction);
	INJEQT_SET void setChatManager(ChatManager *chatManager);
	INJEQT_SET void setChatStorage(ChatStorage *chatStorage);
	INJEQT_SET void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);
	INJEQT_SET void setClearResultsAction(ClearResultsAction *clearResultsAction);
	INJEQT_SET void setContactManager(ContactManager *contactManager);
	INJEQT_SET void setFirstSearchAction(FirstSearchAction *firstSearchAction);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setKaduWindowService(KaduWindowService *kaduWindowService);
	INJEQT_SET void setNextResultsAction(NextResultsAction *nextResultsAction);
	INJEQT_SET void setStopSearchAction(StopSearchAction *stopSearchAction);
	INJEQT_INIT void init();

	void uinTyped();
	void personalDataTyped();
	void endBirthYearTyped();
	void personalDataToggled(bool toggled);
	void uinToggled(bool toggled);
	void selectionChanged();
	void newSearchResults(const BuddyList &buddies);

};
