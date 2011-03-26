/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>

#include "buddies/buddy-manager.h"
#include "configuration/configuration-file.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "misc/path-conversion.h"
#include "debug.h"

#include "buddy-firewall-data.h"
#include "firewall.h"

#include "firewall-configuration-ui-handler.h"

FirewallConfigurationUiHandler * FirewallConfigurationUiHandler::Instance = 0;

void FirewallConfigurationUiHandler::registerUiHandler()
{
	if (!Instance)
	{
		Instance = new FirewallConfigurationUiHandler();
		MainConfigurationWindow::registerUiFile(dataPath("kadu/plugins/configuration/firewall.ui"));
		MainConfigurationWindow::registerUiHandler(Instance);
	}
}

void FirewallConfigurationUiHandler::unregisterUiHandler()
{
	if (Instance)
	{
		MainConfigurationWindow::unregisterUiHandler(Instance);
		MainConfigurationWindow::unregisterUiFile(dataPath("kadu/plugins/configuration/firewall.ui"));
		delete Instance;
		Instance = 0;
	}
}

FirewallConfigurationUiHandler::FirewallConfigurationUiHandler()
{
}

void FirewallConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	ConfigGroupBox *secureGroupBox = mainConfigurationWindow->widget()->configGroupBox("Firewall", "Safe sending", "Safe sending");

	QWidget *secure = new QWidget(secureGroupBox->widget());
	QGridLayout *secureLayout = new QGridLayout(secure);
	secureLayout->setSpacing(5);
	secureLayout->setMargin(5);

	AllList = new QListWidget(secure);
	QPushButton *moveToSecureList = new QPushButton(tr("Move to 'Secured'"), secure);

	secureLayout->addWidget(new QLabel(tr("All"), secure), 0, 0);
	secureLayout->addWidget(AllList, 1, 0);
	secureLayout->addWidget(moveToSecureList, 2, 0);

	SecureList = new QListWidget(secure);
	QPushButton *moveToAllList = new QPushButton(tr("Move to 'All'"), secure);

	secureLayout->addWidget(new QLabel(tr("Secured"), secure), 0, 1);
	secureLayout->addWidget(SecureList, 1, 1);
	secureLayout->addWidget(moveToAllList, 2, 1);

	connect(moveToSecureList, SIGNAL(clicked()), this, SLOT(allRight()));
	connect(moveToAllList, SIGNAL(clicked()), this, SLOT(allLeft()));
	connect(SecureList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(left(QListWidgetItem *)));
	connect(AllList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(right(QListWidgetItem *)));

	secureGroupBox->addWidgets(0, secure);

	foreach (const Buddy &buddy, BuddyManager::instance()->items())
		if (!buddy.isAnonymous())
		{
			BuddyFirewallData *bfd = 0;
			if (buddy.data())
				bfd = buddy.data()->moduleStorableData<BuddyFirewallData>("firewall-secured-sending", Firewall::instance(), false);

			if (!bfd || !bfd->securedSending())
				AllList->addItem(buddy.display());
			else
				SecureList->addItem(buddy.display());
		}

	AllList->sortItems();
	SecureList->sortItems();

	AllList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	SecureList->setSelectionMode(QAbstractItemView::ExtendedSelection);
/*
Automatic question GUI
*/
	ConfigGroupBox *questionGroupBox = mainConfigurationWindow->widget()->configGroupBox("Firewall", "Unknown chats protection", "Automatic question");

	QWidget *question = new QWidget(questionGroupBox->widget());
	QFormLayout *questionLayout = new QFormLayout(question);
	questionLayout->setSpacing(5);
	questionLayout->setMargin(5);

	QuestionEdit = new QTextEdit(question);
	QuestionEdit->setAcceptRichText(false);
	QuestionEdit->setText(config_file.readEntry("Firewall", "question"));
	QuestionEdit->setToolTip(tr("This message will be send to unknown person."));

	AnswerEdit = new QLineEdit(question);
	AnswerEdit->setText(config_file.readEntry("Firewall", "answer"));
	AnswerEdit->setToolTip(tr("Right answer for question above - you can use regexp."));
	QLabel *label = new QLabel(tr("Answer:"), question);
	label->setToolTip(tr("Right answer for question above - you can use regexp."));

	questionLayout->addRow(new QLabel(tr("Message:"), question), QuestionEdit);
	questionLayout->addRow(label, AnswerEdit);

	questionGroupBox->addWidgets(0, question);
/*
End creating Gui
*/
	connect(mainConfigurationWindow->widget()->widgetById("firewall/write_log"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("firewall/log_file"), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widget()->widgetById("firewall/chats"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->configGroupBox("Firewall", "Unknown chats protection", "Automatic question")->widget(), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widget()->widgetById("firewall/chats"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->configGroupBox("Firewall", "Unknown chats protection", "After right answer")->widget(), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widget()->widgetById("firewall/confirmation"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("firewall/confirmation_text"), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widget()->widgetById("firewall/dos"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("firewall/dos_interval"), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widget()->widgetById("firewall/dos_emoticons"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("firewall/emoticons_max"), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widget()->widgetById("firewall/dos_emoticons"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widget()->widgetById("firewall/emoticons_allow_known"), SLOT(setEnabled(bool)));

	QWidget *safeSending = mainConfigurationWindow->widget()->widgetById("firewall/safe_sending");
	connect(safeSending, SIGNAL(toggled(bool)),
		AllList, SLOT(setEnabled(bool)));
	connect(safeSending, SIGNAL(toggled(bool)),
		SecureList, SLOT(setEnabled(bool)));
	connect(safeSending, SIGNAL(toggled(bool)),
		moveToSecureList, SLOT(setEnabled(bool)));
	connect(safeSending, SIGNAL(toggled(bool)),
		moveToAllList, SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow, SIGNAL(configurationWindowApplied()), this, SLOT(configurationApplied()));
}

void FirewallConfigurationUiHandler::left(QListWidgetItem *item)
{
	kdebugf();

	AllList->addItem(new QListWidgetItem(*item));
	SecureList->removeItemWidget(item);
	delete item;

	AllList->sortItems();

	kdebugf2();
}

void FirewallConfigurationUiHandler::allLeft()
{
	int count = SecureList->count();

	for (int i = count - 1; i >= 0; i--)
		if (SecureList->item(i)->isSelected())
		{
			AllList->addItem(SecureList->item(i)->text());
			delete SecureList->takeItem(i);
		}

	AllList->sortItems();
}

void FirewallConfigurationUiHandler::right(QListWidgetItem *item)
{
	kdebugf();

	SecureList->addItem(new QListWidgetItem(*item));
	AllList->removeItemWidget(item);
	delete item;

	SecureList->sortItems();

	kdebugf2();
}

void FirewallConfigurationUiHandler::allRight()
{
	int count = AllList->count();

	for (int i = count - 1; i >= 0; i--)
		if (AllList->item(i)->isSelected())
		{
			SecureList->addItem(AllList->item(i)->text());
			delete AllList->takeItem(i);
		}

	SecureList->sortItems();
}

void FirewallConfigurationUiHandler::configurationApplied()
{
	int count = SecureList->count();
	for (int i = 0; i < count; i++)
	{
		Buddy buddy = BuddyManager::instance()->byDisplay(SecureList->item(i)->text(), ActionReturnNull);
		if (buddy.isNull() || buddy.isAnonymous())
			continue;

		BuddyFirewallData *bfd = 0;
		if (buddy.data())
			bfd = buddy.data()->moduleStorableData<BuddyFirewallData>("firewall-secured-sending", Firewall::instance(), true);
		if (!bfd)
			continue;

		bfd->setSecuredSending(true);
		bfd->store();
	}

	count = AllList->count();
	for (int i = 0; i < count; i++)
	{
		Buddy buddy = BuddyManager::instance()->byDisplay(AllList->item(i)->text(), ActionReturnNull);
		if (buddy.isNull() || buddy.isAnonymous())
			continue;

		BuddyFirewallData *bfd = 0;
		if (buddy.data())
			bfd = buddy.data()->moduleStorableData<BuddyFirewallData>("firewall-secured-sending", Firewall::instance(), true);
		if (!bfd)
			continue;

		bfd->setSecuredSending(false);
		bfd->store();
	}

	config_file.writeEntry("Firewall", "question", QuestionEdit->toPlainText());
	config_file.writeEntry("Firewall", "answer", AnswerEdit->text());
}
