/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef SEARCH_WINDOW_H
#define SEARCH_WINDOW_H

#include "buddies/buddy-list.h"
#include "buddies/buddy-search-criteria.h"
#include "buddies/buddy-set.h"
#include "chat/chat.h"
#include "gui/actions/action.h"
#include "gui/windows/main-window.h"

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QTreeWidget;
class QTreeWidgetItem;
class SearchService;

// TODO: a better name
class SearchActionsSlots : public QObject
{
	Q_OBJECT

public slots:
	void firstSearchActionCreated(Action *action);
	void nextResultsActionCreated(Action *action);
	void stopSearchActionCreated(Action *action);
	void clearResultsActionCreated(Action *action);
	void actionsFoundActionCreated(Action *action);

	void firstSearchActionActivated(QAction *sender, bool toggled);
	void nextResultsActionActivated(QAction *sender, bool toggled);
	void stopSearchActionActivated(QAction *sender, bool toggled);
	void clearResultsActionActivated(QAction *sender, bool toggled);
	void addFoundActionActivated(QAction *sender, bool toggled);
	void chatFoundActionActivated(QAction *sender, bool toggled);

};

class KADUAPI SearchWindow : public MainWindow
{
	Q_OBJECT

	friend class SearchActionsSlots;
	static SearchActionsSlots *searchActionsSlot;

	static ActionDescription *firstSearchAction;
	static ActionDescription *nextResultsAction;
	static ActionDescription *stopSearchAction;
	static ActionDescription *clearResultsAction;
	static ActionDescription *addFoundAction;
	static ActionDescription *chatFoundAction;

	SearchService *CurrentSearchService;

	QCheckBox *only_active;
	QLineEdit *e_uin;
	QLineEdit *e_name;
	QLineEdit *e_nick;
	QLineEdit *e_byrFrom;
	QLineEdit *e_byrTo;
	QLineEdit *e_surname;
	QComboBox *c_gender;
	QLineEdit *e_city;
	QTreeWidget *results;
	QLabel *progress;
	QRadioButton *r_uin;
	QRadioButton *r_pers;
	Account CurrentAccount;
	quint32 seq;
	BuddySet selectedUsers;

	BuddySearchCriteria CurrentSearchCriteria;

	bool searchhidden;
	bool searching;
	bool workaround; // TODO: remove

	bool isPersonalDataEmpty() const;

	ContactSet selected();

	QTreeWidgetItem * selectedItem();

	void setActionState(ActionDescription *action, bool toogle);

private slots:
	void uinTyped(void);
	void personalDataTyped(void);
	void byrFromDataTyped(void);
	void persClicked();
	void uinClicked();
	void selectionChanged();

protected:

	virtual void closeEvent(QCloseEvent *e);
	virtual void resizeEvent(QResizeEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);

public:

	SearchWindow(QWidget *parent=0, Buddy buddy = Buddy::null);
	~SearchWindow(void);

	static void createDefaultToolbars(QDomElement parentConfig);

	static void initModule();
	static void closeModule();

	virtual bool supportsActionType(ActionDescription::ActionType type) { return type & ActionDescription::TypeSearch; }
	virtual BuddiesListView* buddiesListView() { return 0; }
	virtual StatusContainer* statusContainer() { return 0; }
	virtual ContactSet contacts() { return ContactSet(); }
	virtual BuddySet buddies() { return BuddySet(); }
	virtual Chat chat() { return Chat::null; }
	virtual bool hasContactSelected() { return true; } // we really have contacts now

	void nextSearch();
	void stopSearch();
	void clearResults();

	void addFound();
	void chatFound();

public slots:

	void newSearchResults(BuddyList buddies);
	void firstSearch();
};

#endif

