/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QKeyEvent>
#include <QtWidgets/QAction>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>

#include "accounts/account-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "chat/chat-manager.h"
#include "chat/type/chat-type-contact-set.h"
#include "chat/type/chat-type-contact.h"
#include "configuration/config-file-variant-wrapper.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"
#include "core/application.h"
#include "core/core.h"
#include "gui/actions/base-action-context.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/toolbar.h"
#include "gui/windows/add-buddy-window.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/search-window-actions.h"
#include "icons/kadu-icon.h"
#include "os/generic/window-geometry-manager.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"
#include "protocols/services/search-service.h"
#include "qt/long-validator.h"
#include "status/status-container.h"

#include "search-window.h"

void SearchWindow::createDefaultToolbars(const QDomElement &toolbarsConfig)
{
	QDomElement dockAreaConfig = getDockAreaConfigElement(toolbarsConfig, "search_bottomDockArea");
	QDomElement toolbarConfig = Application::instance()->configuration()->api()->createElement(dockAreaConfig, "ToolBar");

	addToolButton(toolbarConfig, "firstSearchAction", Qt::ToolButtonTextUnderIcon);
	addToolButton(toolbarConfig, "nextResultsAction", Qt::ToolButtonTextUnderIcon);
	addToolButton(toolbarConfig, "stopSearchAction", Qt::ToolButtonTextUnderIcon);
	addToolButton(toolbarConfig, "clearSearchAction", Qt::ToolButtonTextUnderIcon);
	addToolButton(toolbarConfig, "addSearchedAction", Qt::ToolButtonTextUnderIcon);
	addToolButton(toolbarConfig, "chatSearchedAction", Qt::ToolButtonTextUnderIcon);
}

SearchWindow::SearchWindow(QWidget *parent, Buddy buddy) :
		MainWindow(new BaseActionContext(this), "search", parent),
		CurrentSearchService(0), UinEdit(0), FirstNameEdit(0), LastNameEdit(0), NickNameEdit(0),
		StartBirthYearEdit(0), EndBirthYearEdit(0), CityEdit(0),
		GenderComboBox(0), OnlyActiveCheckBox(0), UinRadioButton(0), PersonalDataRadioButton(0),
		ResultsListWidget(0), SearchInProgress(false), DoNotTransferFocus(false)
{
	setWindowRole("kadu-search");

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Search User in Directory"));

	RoleSet roles;
	roles.insert(ContactRole);
	static_cast<BaseActionContext *>(actionContext())->setRoles(roles);

	if (buddy)
	{
		CurrentAccount = BuddyPreferredManager::instance()->preferredAccount(buddy);

		CurrentSearchCriteria.SearchBuddy = buddy;
	}
	else
	{
		// TODO choose proper account
		foreach (const Account &account, AccountManager::instance()->items())
			if (account.protocolHandler() && account.protocolHandler()->isConnected() &&
					account.protocolHandler()->searchService())
			{
				CurrentAccount = account;
				break;
			}

		if (CurrentAccount.isNull())
			CurrentAccount = AccountManager::instance()->defaultAccount();
	}

	if (CurrentAccount.protocolHandler())
		CurrentSearchService = CurrentAccount.protocolHandler()->searchService();

	if (CurrentSearchService)
		connect(CurrentSearchService, SIGNAL(newResults(BuddyList)), this, SLOT(newSearchResults(BuddyList)));

	createGui();

	if (loadOldToolBarsFromConfig("searchDockArea", Qt::BottomToolBarArea))
		writeToolBarsToConfig();
	else
		loadToolBarsFromConfig();

	if (CurrentSearchCriteria.SearchBuddy)
	{
		QVector<Contact> contacts = CurrentSearchCriteria.SearchBuddy.contacts(CurrentAccount);
		Contact contact = contacts.isEmpty() ? Contact::null : contacts.at(0);
		if (contact)
			// it should call uinTyped() slot
			UinEdit->insert(contact.id());
	}

	if (UinEdit->text().isEmpty())
		personalDataTyped();

	new WindowGeometryManager(new ConfigFileVariantWrapper("General", "SearchWindowGeometry"), QRect(0, 50, 800, 350), this);
}

SearchWindow::~SearchWindow()
{
}

void SearchWindow::createGui()
{
	QWidget *centralWidget = new QWidget(this);
	QGridLayout *layout = new QGridLayout(centralWidget);

	QGroupBox *searchCriteriaGroupBox = new QGroupBox(tr("Search Criteria"), centralWidget);
	QButtonGroup *searchCriteriaButtonGroup = new QButtonGroup(searchCriteriaGroupBox);
	QHBoxLayout *searchCriteriaLayout = new QHBoxLayout(searchCriteriaGroupBox);

	UinRadioButton = new QRadioButton(tr("&Uin number"), searchCriteriaGroupBox);
	connect(UinRadioButton, SIGNAL(toggled(bool)), this, SLOT(uinToggled(bool)));
	UinRadioButton->setToolTip(tr("Search for this uin exclusively"));
	searchCriteriaButtonGroup->addButton(UinRadioButton);
	searchCriteriaLayout->addWidget(UinRadioButton);

	PersonalDataRadioButton = new QRadioButton(tr("&Personal data"), searchCriteriaGroupBox);
	connect(PersonalDataRadioButton, SIGNAL(toggled(bool)), this, SLOT(personalDataToggled(bool)));
	PersonalDataRadioButton->setToolTip(tr("Search using the personal data typed above (name, nickname, etc.)"));
	searchCriteriaButtonGroup->addButton(PersonalDataRadioButton);
	searchCriteriaLayout->addWidget(PersonalDataRadioButton);

	layout->addWidget(searchCriteriaGroupBox, 0, 0);

	QGroupBox *uinGroupBox = new QGroupBox(tr("Uin Number"), centralWidget);
	QHBoxLayout *uinLayout = new QHBoxLayout(uinGroupBox);
	QLabel *uinLabel = new QLabel(tr("Uin:"), uinGroupBox);
	UinEdit = new QLineEdit(uinGroupBox);
	// TODO proper validator (this one is taken from GaduIdValidator)
	UinEdit->setValidator(new LongValidator(1LL, 3999999999LL, UinEdit));
	connect(UinEdit, SIGNAL(textChanged(QString)), this, SLOT(uinTyped()));
	connect(UinEdit, SIGNAL(returnPressed()), this, SLOT(firstSearch()));
	uinLayout->addWidget(uinLabel);
	uinLayout->addWidget(UinEdit);

	layout->addWidget(uinGroupBox, 1, 0);

	QGroupBox *personalDataGroupBox = new QGroupBox(tr("Personal Data"), centralWidget);
	QGridLayout *personalDataLayout = new QGridLayout(personalDataGroupBox);

	QLabel *nickLabel = new QLabel(tr("Nick:"), personalDataGroupBox);
	NickNameEdit = new QLineEdit(personalDataGroupBox);
	connect(NickNameEdit, SIGNAL(textChanged(QString)), this, SLOT(personalDataTyped()));
	connect(NickNameEdit, SIGNAL(returnPressed()), this, SLOT(firstSearch()));
	personalDataLayout->addWidget(nickLabel, 0, 0, Qt::AlignRight);
	personalDataLayout->addWidget(NickNameEdit, 0, 1);

	QLabel *genderLabel = new QLabel(tr("Gender:"), personalDataGroupBox);
	GenderComboBox = new QComboBox(personalDataGroupBox);
	GenderComboBox->insertItem(0, QString());
	GenderComboBox->insertItem(1, tr("Male"));
	GenderComboBox->insertItem(2, tr("Female"));
	connect(GenderComboBox, SIGNAL(activated(int)), this, SLOT(personalDataTyped()));
	personalDataLayout->addWidget(genderLabel, 1, 0, Qt::AlignRight);
	personalDataLayout->addWidget(GenderComboBox, 1, 1);

	QLabel *firstNameLabel = new QLabel(tr("First name:"), personalDataGroupBox);
	FirstNameEdit = new QLineEdit(personalDataGroupBox);
	connect(FirstNameEdit, SIGNAL(textChanged(const QString &)), this, SLOT(personalDataTyped()));
	connect(FirstNameEdit, SIGNAL(returnPressed()), this, SLOT(firstSearch()));
	personalDataLayout->addWidget(firstNameLabel, 0, 2, Qt::AlignRight);
	personalDataLayout->addWidget(FirstNameEdit, 0, 3);

	QLabel *lastNameLabel = new QLabel(tr("Last name:"), personalDataGroupBox);
	LastNameEdit = new QLineEdit(personalDataGroupBox);
	connect(LastNameEdit, SIGNAL(textChanged(QString)), this, SLOT(personalDataTyped()));
	connect(LastNameEdit, SIGNAL(returnPressed()), this, SLOT(firstSearch()));
	personalDataLayout->addWidget(lastNameLabel, 1, 2, Qt::AlignRight);
	personalDataLayout->addWidget(LastNameEdit, 1, 3);

	QLabel *startBirthYearLabel = new QLabel(tr("Birth year from:"), personalDataGroupBox);
	StartBirthYearEdit = new QLineEdit(personalDataGroupBox);
	StartBirthYearEdit->setValidator(new QIntValidator(1, 2100, StartBirthYearEdit));
	connect(StartBirthYearEdit, SIGNAL(textChanged(QString)), this, SLOT(endBirthYearTyped()));
	connect(StartBirthYearEdit, SIGNAL(textChanged(QString)), this, SLOT(personalDataTyped()));
	connect(StartBirthYearEdit, SIGNAL(returnPressed()), this, SLOT(firstSearch()));
	personalDataLayout->addWidget(startBirthYearLabel, 2, 0, Qt::AlignRight);
	personalDataLayout->addWidget(StartBirthYearEdit, 2, 1);

	QLabel *endBirthYearLabel = new QLabel(tr("to:"), personalDataGroupBox);
	EndBirthYearEdit = new QLineEdit(personalDataGroupBox);
	EndBirthYearEdit->setEnabled(false);
	EndBirthYearEdit->setValidator(new QIntValidator(1, 2100, EndBirthYearEdit));
	connect(EndBirthYearEdit, SIGNAL(textChanged(QString)), this, SLOT(personalDataTyped()));
	connect(EndBirthYearEdit, SIGNAL(returnPressed()), this, SLOT(firstSearch()));
	personalDataLayout->addWidget(endBirthYearLabel, 3, 0, Qt::AlignRight);
	personalDataLayout->addWidget(EndBirthYearEdit, 3, 1);

	QLabel *cityLabel = new QLabel(tr("City:"), personalDataGroupBox);
	CityEdit = new QLineEdit(personalDataGroupBox);
	connect(CityEdit, SIGNAL(textChanged(QString)), this, SLOT(personalDataTyped()));
	connect(CityEdit, SIGNAL(returnPressed()), this, SLOT(firstSearch()));
	personalDataLayout->addWidget(cityLabel, 2, 2, Qt::AlignRight);
	personalDataLayout->addWidget(CityEdit, 2, 3);

	OnlyActiveCheckBox = new QCheckBox(tr("Search only for active users"), personalDataGroupBox);
	connect(OnlyActiveCheckBox, SIGNAL(clicked()), this, SLOT(personalDataTyped()));
	personalDataLayout->addWidget(OnlyActiveCheckBox, 3, 2, 1, 2);

	layout->addWidget(personalDataGroupBox, 0, 1, 2, 1);
	layout->setColumnStretch(1, 100);

	ResultsListWidget = new QTreeWidget(centralWidget);
	QStringList headers;
	headers << tr("Status") << tr("Uin") << tr("First Name") << tr("City") << tr("Nickname") << tr("Birth Year");
	ResultsListWidget->setHeaderLabels(headers);
	ResultsListWidget->setSortingEnabled(true);
	ResultsListWidget->setAllColumnsShowFocus(true);
	ResultsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	ResultsListWidget->setIndentation(false);
	connect(ResultsListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
	layout->addWidget(ResultsListWidget, 2, 0, 1, -1);
	layout->setRowStretch(2, 100);

	setCentralWidget(centralWidget);

	statusBar()->showMessage(tr("Ready"));
}

void SearchWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QWidget::keyPressEvent(e);
}

QTreeWidgetItem * SearchWindow::selectedItem() const
{
	if (!ResultsListWidget->selectedItems().isEmpty())
		return ResultsListWidget->selectedItems().at(0);
	else if (ResultsListWidget->children().count() == 1)
		return dynamic_cast<QTreeWidgetItem *>(ResultsListWidget->children().at(0));
	else
		return 0;
}

// TODO: return real list
ContactSet SearchWindow::selectedContacts() const
{
	ContactSet result;

	QTreeWidgetItem *selected = selectedItem();

	if (!selected)
		return result;

	QString uin = selected->text(1);
	QString firstName = selected->text(2);
	QString nickName = selected->text(4);

	QString altNick;
	if (!nickName.isEmpty())
		altNick = nickName;
	else if (!firstName.isEmpty())
		altNick = firstName;
	else
		altNick = uin;

	Contact contact = ContactManager::instance()->byId(CurrentAccount, uin, ActionCreateAndAdd);
	Buddy buddy = BuddyManager::instance()->byContact(contact, ActionCreateAndAdd);

	if (buddy.isAnonymous())
	{
		buddy.setFirstName(firstName);
		buddy.setNickName(nickName);
		buddy.setDisplay(altNick);
	}

	result.insert(contact);
	return result;
}

void SearchWindow::addFound()
{
	foreach (const Buddy &buddy, selectedContacts().toBuddySet())
		(new AddBuddyWindow(Core::instance()->kaduWindow(), buddy))->show();
}

void SearchWindow::chatFound()
{
	const ContactSet &contacts = selectedContacts();
	if (!contacts.isEmpty())
	{
		const Chat &chat = 1 == contacts.size()
				? ChatTypeContact::findChat(*contacts.constBegin(), ActionCreateAndAdd)
				: ChatTypeContactSet::findChat(contacts, ActionCreateAndAdd);
		Core::instance()->chatWidgetManager()->openChat(chat, OpenChatActivation::Activate);
	}
}

void SearchWindow::clearResults()
{
	ResultsListWidget->clear();

	setActionEnabled(SearchWindowActions::instance()->AddFound, false);
	setActionEnabled(SearchWindowActions::instance()->ClearResults, false);
	setActionEnabled(SearchWindowActions::instance()->ChatFound, false);
}

void SearchWindow::stopSearch()
{
	CurrentSearchService->stop();

	setActionEnabled(SearchWindowActions::instance()->StopSearch, false);

	if ((PersonalDataRadioButton->isChecked() && !isPersonalDataEmpty()) ||
			(UinRadioButton->isChecked() && !UinEdit->text().isEmpty()))
		setActionEnabled(SearchWindowActions::instance()->FirstSearch, true);

	if (!ResultsListWidget->selectedItems().isEmpty())
	{
		if (PersonalDataRadioButton->isChecked() && !isPersonalDataEmpty())
			setActionEnabled(SearchWindowActions::instance()->NextResults, true);

		setActionEnabled(SearchWindowActions::instance()->AddFound, true);
		setActionEnabled(SearchWindowActions::instance()->ChatFound, true);
	}

	if (ResultsListWidget->topLevelItemCount() > 0)
		setActionEnabled(SearchWindowActions::instance()->ClearResults, true);
}

void SearchWindow::firstSearch()
{
	if (PersonalDataRadioButton->isChecked() && isPersonalDataEmpty())
		return;

	if (!CurrentAccount)
	{
		MessageDialog::show(KaduIcon("dialog-error"), windowTitle(),
				tr("To be able to search you have to set up an account first."), QMessageBox::Ok, this);
		return;
	}

	if (!CurrentSearchService)
	{
		MessageDialog::show(KaduIcon("dialog-error"), windowTitle(),
				tr("We don't offer contacts search feature for your network yet."), QMessageBox::Ok, this);
		return;
	}

	if (!CurrentAccount.protocolHandler() || !CurrentAccount.protocolHandler()->isConnected())
	{
		MessageDialog::show(KaduIcon("dialog-error"), windowTitle(),
				tr("Cannot search contacts in offline mode."), QMessageBox::Ok, this);
		return;
	}

	if (!ResultsListWidget->children().isEmpty())
		clearResults();

	if (SearchInProgress)
		CurrentSearchService->stop();

	CurrentSearchCriteria.clearData();

	if (PersonalDataRadioButton->isChecked())
	{
		CurrentSearchCriteria.reqFirstName(FirstNameEdit->text());
		CurrentSearchCriteria.reqLastName(LastNameEdit->text());
		CurrentSearchCriteria.reqNickName(NickNameEdit->text());
		CurrentSearchCriteria.reqCity(CityEdit->text());
		if (((EndBirthYearEdit->text().isEmpty()) && (!StartBirthYearEdit->text().isEmpty()))
				|| ((EndBirthYearEdit->text().toInt()) < (StartBirthYearEdit->text().toInt())))
			EndBirthYearEdit->setText(StartBirthYearEdit->text());
		CurrentSearchCriteria.reqBirthYear(StartBirthYearEdit->text(), EndBirthYearEdit->text());

		switch (GenderComboBox->currentIndex())
		{
			case 1:
				CurrentSearchCriteria.reqGender(false);
				break;
			case 2:
				CurrentSearchCriteria.reqGender(true);
				break;
		}

		if (OnlyActiveCheckBox->isChecked())
			CurrentSearchCriteria.reqActive();
	}
	else if (UinRadioButton->isChecked())
		CurrentSearchCriteria.reqUin(CurrentAccount, UinEdit->text());

	SearchInProgress = true;

	setActionEnabled(SearchWindowActions::instance()->StopSearch, true);
	setActionEnabled(SearchWindowActions::instance()->FirstSearch, false);
	setActionEnabled(SearchWindowActions::instance()->NextResults, false);
	setActionEnabled(SearchWindowActions::instance()->AddFound, false);
	setActionEnabled(SearchWindowActions::instance()->ChatFound, false);

	CurrentSearchService->searchFirst(CurrentSearchCriteria);

	statusBar()->showMessage(tr("Searching..."));
}

void SearchWindow::nextSearch()
{
	if (!CurrentAccount.protocolHandler() || !CurrentAccount.protocolHandler()->isConnected())
		return;

	SearchInProgress = true;

	setActionEnabled(SearchWindowActions::instance()->StopSearch, true);
	setActionEnabled(SearchWindowActions::instance()->FirstSearch, false);
	setActionEnabled(SearchWindowActions::instance()->NextResults, false);
	setActionEnabled(SearchWindowActions::instance()->AddFound, false);
	setActionEnabled(SearchWindowActions::instance()->ChatFound, false);

	CurrentSearchService->searchNext();

	statusBar()->showMessage(tr("Searching..."));
}

void SearchWindow::newSearchResults(const BuddyList &buddies)
{
	foreach (const Buddy &buddy, buddies)
	{
		QVector<Contact> contacts = buddy.contacts(CurrentAccount);
		Contact contact = contacts.isEmpty() ? Contact::null : contacts.at(0);
		QList<QTreeWidgetItem *> items = ResultsListWidget->findItems(contact.id(), Qt::MatchExactly, 1);
		QTreeWidgetItem *treeItem = items.isEmpty() ? 0 : items.at(0);

		if (treeItem)
		{
			treeItem->setText(1, contact.id());
			treeItem->setText(2, buddy.firstName());
			treeItem->setText(3, buddy.city());
			treeItem->setText(4, buddy.nickName());
			treeItem->setText(5, QString::number(buddy.birthYear()));
		}
		else
		{
			QStringList strings;
			strings << QString() << contact.id() << buddy.firstName()
					<< buddy.city() << buddy.nickName() << QString::number(buddy.birthYear());
			treeItem = new QTreeWidgetItem(ResultsListWidget, strings);
			treeItem->setIcon(0, contact.contactAccount().statusContainer()->statusIcon(contact.currentStatus()).icon());
		}
	}

	statusBar()->showMessage(tr("Done searching"));

	if ((PersonalDataRadioButton->isChecked() && !isPersonalDataEmpty()) ||
			(UinRadioButton->isChecked() && !UinEdit->text().isEmpty()))
		setActionEnabled(SearchWindowActions::instance()->FirstSearch, true);

	setActionEnabled(SearchWindowActions::instance()->StopSearch, false);

	if (buddies.isEmpty())
		MessageDialog::show(KaduIcon("dialog-information"), windowTitle(),
				tr("There were no results of your search."), QMessageBox::Ok, this);
	else
	{
		if (PersonalDataRadioButton->isChecked() && !isPersonalDataEmpty())
			setActionEnabled(SearchWindowActions::instance()->NextResults, true);

		if (ResultsListWidget->topLevelItemCount() > 0)
			setActionEnabled(SearchWindowActions::instance()->ClearResults, true);
	}

	if (!ResultsListWidget->selectedItems().isEmpty())
	{
		setActionEnabled(SearchWindowActions::instance()->AddFound, true);
		setActionEnabled(SearchWindowActions::instance()->ChatFound, true);
	}

	SearchInProgress = false;
}

void SearchWindow::uinTyped()
{
	UinRadioButton->setChecked(true);

	setActionEnabled(SearchWindowActions::instance()->FirstSearch, !UinEdit->text().isEmpty());
}

void SearchWindow::personalDataTyped()
{
	// do not transfer focus if called by a signal from widget (i.e., user typed something)
	if (sender() && sender()->isWidgetType())
	{
		DoNotTransferFocus = true;
		PersonalDataRadioButton->setChecked(true);
		DoNotTransferFocus = false;
	}
	else
		PersonalDataRadioButton->setChecked(true);

	setActionEnabled(SearchWindowActions::instance()->FirstSearch, !isPersonalDataEmpty());
	setActionEnabled(SearchWindowActions::instance()->NextResults, false);
}

void SearchWindow::endBirthYearTyped()
{
	if (StartBirthYearEdit->text().isEmpty())
	{
		EndBirthYearEdit->setEnabled(false);
		EndBirthYearEdit->clear();
	}
	else
		EndBirthYearEdit->setEnabled(true);
}

void SearchWindow::personalDataToggled(bool toggled)
{
	if (!toggled)
		return;

	OnlyActiveCheckBox->setEnabled(true);
	OnlyActiveCheckBox->setChecked(false);
	if (!DoNotTransferFocus)
		NickNameEdit->setFocus();

	setActionEnabled(SearchWindowActions::instance()->FirstSearch, !isPersonalDataEmpty());
}

void SearchWindow::uinToggled(bool toggled)
{
	if (!toggled)
		return;

	OnlyActiveCheckBox->setEnabled(false);
	UinEdit->setFocus();

	setActionEnabled(SearchWindowActions::instance()->FirstSearch, !UinEdit->text().isEmpty());
	setActionEnabled(SearchWindowActions::instance()->NextResults, false);
}

bool SearchWindow::isPersonalDataEmpty() const
{
	return FirstNameEdit->text().isEmpty() &&
			NickNameEdit->text().isEmpty() &&
			StartBirthYearEdit->text().isEmpty() &&
			LastNameEdit->text().isEmpty() &&
			GenderComboBox->currentIndex() == 0 &&
			CityEdit->text().isEmpty();
}

void SearchWindow::selectionChanged()
{
	bool enableActions = !ResultsListWidget->selectedItems().isEmpty();
	setActionEnabled(SearchWindowActions::instance()->AddFound, enableActions);
	setActionEnabled(SearchWindowActions::instance()->ChatFound, enableActions);
}

void SearchWindow::setActionEnabled(ActionDescription *actionDescription, bool enable)
{
	Action *action = actionDescription->action(actionContext());
	if (action)
		action->setEnabled(enable);
}

#include "moc_search-window.cpp"
