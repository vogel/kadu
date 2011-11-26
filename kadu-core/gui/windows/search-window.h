/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

class BaseActionContext;
class SearchService;

class KADUAPI SearchWindow : public MainWindow
{
	Q_OBJECT

	friend class SearchWindowActions;

	Account CurrentAccount;
	SearchService *CurrentSearchService;

	BuddySearchCriteria CurrentSearchCriteria;

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
	void uinTyped();
	void personalDataTyped();
	void endBirthYearTyped();
	void personalDataToggled(bool toggled);
	void uinToggled(bool toggled);
	void selectionChanged();
	void newSearchResults(const BuddyList &buddies);

protected:
	virtual void keyPressEvent(QKeyEvent *e);

public:
	static void createDefaultToolbars(const QDomElement &toolbarsConfig);

	explicit SearchWindow(QWidget *parent = 0, Buddy buddy = Buddy::null);
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

};

#endif // SEARCH_WINDOW_H
