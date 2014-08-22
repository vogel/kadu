/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QFile>
#include <QtCore/QRegExp>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "core/core.h"
#include "formatted-string/formatted-string-html-visitor.h"
#include "formatted-string/formatted-string.h"
#include "gui/widgets/chat-widget/chat-widget-repository.h"
#include "gui/widgets/chat-widget/chat-widget.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/custom-input.h"
#include "misc/paths-provider.h"
#include "debug.h"

#include "word-fix-formatted-string-visitor.h"

#include "word-fix.h"


WordFix::WordFix(QObject *parent) :
		ConfigurationUiHandler(parent),
		changeButton{},
		deleteButton{},
		addButton{},
		wordEdit{},
		valueEdit{},
		list{}
{
}

WordFix::~WordFix()
{
}

void WordFix::setChatWidgetRepository(ChatWidgetRepository *chatWidgetRepository)
{
	this->chatWidgetRepository = chatWidgetRepository;

	if (this->chatWidgetRepository)
	{
		connect(this->chatWidgetRepository.data(), SIGNAL(chatWidgetAdded(ChatWidget *)),
			this, SLOT(chatWidgetAdded(ChatWidget *)));
		connect(this->chatWidgetRepository.data(), SIGNAL(chatWidgetRemoved(ChatWidget*)),
			this, SLOT(chatWidgetRemoved(ChatWidget *)));

		for (auto chatWidget : this->chatWidgetRepository.data())
			chatWidgetAdded(chatWidget);
	}
}

bool WordFix::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)
	kdebugf();

	ExtractBody.setPattern("<body[^>]*>.*</body>");

	// Loading list
	QString data = Application::instance()->configuration()->deprecatedApi()->readEntry("word_fix", "WordFix_list");
	if (data.isEmpty())
	{
		QFile defList(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/data/word_fix/wf_default_list.data"));
		if (defList.open(QIODevice::ReadOnly))
		{
			QTextStream s(&defList);
			QStringList pair;
			while (!s.atEnd())
			{
				pair = s.readLine().split('|');
				if (pair.isEmpty())
					continue;

				// TODO why we are not checking if there are actually at least 2 items?
				wordsList[pair.at(0)] = pair.at(1);
			}
			defList.close();
		}
		else
		{
			kdebug("Can't open file: %s\n", qPrintable((defList.fileName())));
		}
	}
	else
	{
		QStringList list = data.split("\t\t");
		for (int i = 0; i < list.count(); i++)
		{
			if (!list.at(i).isEmpty())
			{
				QStringList sp = list.at(i).split('\t');
				wordsList[sp.at(0)] = sp.at(1);
			}
		}
	}

	kdebugf2();

	kdebugf();
	MainConfigurationWindow::registerUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/word_fix.ui"));
	MainConfigurationWindow::registerUiHandler(this);

	setChatWidgetRepository(Core::instance()->chatWidgetRepository());

	kdebugf2();
	return true;
}

void WordFix::done()
{
	kdebugf();
	MainConfigurationWindow::unregisterUiHandler(this);
	MainConfigurationWindow::unregisterUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/word_fix.ui"));
	kdebugf2();

	kdebugf();

	if (chatWidgetRepository)
	{
		disconnect(chatWidgetRepository.data(), 0, this, 0);

		for (auto chatWidget : chatWidgetRepository.data())
			chatWidgetRemoved(chatWidget);
	}

	kdebugf2();
}

void WordFix::chatWidgetAdded(ChatWidget *chatWidget)
{
	connect(chatWidget, SIGNAL(messageSendRequested(ChatWidget*)), this, SLOT(sendRequest(ChatWidget*)));
}

void WordFix::chatWidgetRemoved(ChatWidget *chatWidget)
{
	disconnect(chatWidget, 0, this, 0);
}

void WordFix::sendRequest(ChatWidget* chat)
{
	if (!Application::instance()->configuration()->deprecatedApi()->readBoolEntry("PowerKadu", "enable_word_fix", false))
		return;

	auto formattedString = chat->edit()->formattedString();
	WordFixFormattedStringVisitor fixVisitor(wordsList);
	formattedString->accept(&fixVisitor);

	auto fixedString = fixVisitor.result();
	FormattedStringHtmlVisitor htmlVisitor;
	fixedString->accept(&htmlVisitor);

	chat->edit()->setHtml(htmlVisitor.result());
}

void WordFix::wordSelected()
{
	kdebugf();

	QList<QTreeWidgetItem *> items = list->selectedItems();
	QTreeWidgetItem* item = NULL;

	if (items.isEmpty())
	{
		changeButton->setEnabled(false);
		deleteButton->setEnabled(false);
		return;
	}
	else
	{
		changeButton->setEnabled(true);
		deleteButton->setEnabled(true);
	}

	item = items.at(0);
	wordEdit->setText(item->text(0));
	valueEdit->setText(item->text(1));

	kdebugf2();
}

void WordFix::changeSelected()
{
	kdebugf();

	QTreeWidgetItem* item = list->currentItem();
	if (!item)
		return;

	QString wordStr = wordEdit->text(),
			valueStr = valueEdit->text();

	item->setText(0, wordStr);
	item->setText(1, valueStr);

	wordEdit->clear();
	valueEdit->clear();

	changeButton->setEnabled(false);
	deleteButton->setEnabled(false);
	kdebugf2();
}

void WordFix::deleteSelected()
{
	kdebugf();

	QTreeWidgetItem* item = list->currentItem();
	if (!item)
		return;

	QString wordStr = item->text(0);
	delete item;

	wordEdit->clear();
	valueEdit->clear();

	changeButton->setEnabled(false);
	deleteButton->setEnabled(false);
	list->setCurrentItem(0);

	kdebugf2();
}

void WordFix::addNew()
{
	kdebugf();

	QString wordStr = wordEdit->text(),
			valueStr = valueEdit->text();

	if (wordStr.isEmpty())
		return;

	if (list->findItems(wordStr, 0, Qt::MatchExactly).isEmpty())
	{
		QTreeWidgetItem* item = new QTreeWidgetItem(list);
		item->setText(0, wordStr);
		item->setText(1, valueStr);
		list->sortItems(0, Qt::AscendingOrder);
		list->setCurrentItem(item);
	}

	wordEdit->clear();
	valueEdit->clear();

	changeButton->setEnabled(false);
	deleteButton->setEnabled(false);

	kdebugf2();
}

void WordFix::moveToNewValue()
{
	kdebugf();

	// sprawdz czy podane slowo znajduje sie na liscie i jesli tak to umozliw edycje
	QList<QTreeWidgetItem *> items = list->findItems(wordEdit->text(), 0, Qt::MatchExactly);
	if (!items.isEmpty()) {
		QTreeWidgetItem *item = items.at(0);
		list->setCurrentItem(item);
		valueEdit->setText(item->text(1));
		changeButton->setEnabled(true);
		deleteButton->setEnabled(true);
		addButton->setEnabled(false);
	} else {
		changeButton->setEnabled(false);
		deleteButton->setEnabled(false);
		addButton->setEnabled(true);
	}

	valueEdit->setFocus();
	kdebugf2();
}

void WordFix::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	kdebugf();

	ConfigGroupBox *groupBox = mainConfigurationWindow->widget()->
		configGroupBox("Chat", "Spelling", "Words fix");

	QWidget *widget = new QWidget(groupBox->widget());

	QGridLayout *layout = new QGridLayout(widget);
	layout->setSpacing(5);
	layout->setMargin(5);

	list = new QTreeWidget(widget);
	layout->addWidget(list, 0, 0, 1, 3);

	wordEdit = new QLineEdit(widget);
	layout->addWidget(new QLabel(tr("A word to be replaced")), 1, 0);
	layout->addWidget(wordEdit, 1, 1);

	valueEdit = new QLineEdit(widget);
	layout->addWidget(new QLabel(tr("Value to replace with")), 2, 0);
	layout->addWidget(valueEdit, 2, 1);

	QWidget *hbox = new QWidget(widget);
	addButton = new QPushButton(tr("Add"), hbox);
	changeButton = new QPushButton(tr("Change"), hbox);
	deleteButton = new QPushButton(tr("Delete"), hbox);
	QHBoxLayout *hlayout = new QHBoxLayout;
	hlayout->addWidget(addButton);
	hlayout->addWidget(changeButton);
	hlayout->addWidget(deleteButton);
	hbox->setLayout(hlayout);
	layout->addWidget(hbox, 3, 1);

	widget->setLayout(layout);
	groupBox->addWidgets(new QLabel(tr(""), groupBox->widget()), widget);

	connect(list, SIGNAL(itemSelectionChanged()), this, SLOT(wordSelected()));
	connect(changeButton, SIGNAL(clicked()), this, SLOT(changeSelected()));
	connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteSelected()));
	connect(addButton, SIGNAL(clicked()), this, SLOT(addNew()));
	connect(wordEdit, SIGNAL(returnPressed()), this, SLOT(moveToNewValue()));
	connect(valueEdit, SIGNAL(returnPressed()), this, SLOT(addNew()));

	changeButton->setEnabled(false);
	deleteButton->setEnabled(false);

	list->setAllColumnsShowFocus(true);
	list->setColumnCount(2);

	QStringList headers;
	headers << tr("Word") << tr("Replace with");
	list->setHeaderLabels(headers);

	list->setColumnWidth(0, 250);
	list->setColumnWidth(1, 246 - list->verticalScrollBar()->width());

	QList<QTreeWidgetItem *> items;

	for (QMap<QString, QString>::const_iterator i = wordsList.constBegin(); i != wordsList.constEnd(); ++i)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem(list);
		item->setText(0, i.key());
		item->setText(1, i.value());
		items.append(item);
	}
	list->insertTopLevelItems(0, items);

	connect(mainConfigurationWindow, SIGNAL(configurationWindowApplied()), this, SLOT(configurationApplied()));

	kdebugf2();
}

void WordFix::configurationApplied()
{
	kdebugf();

	wordsList.clear();
	QTreeWidgetItem* item = list->itemAt(0, 0);

	if (item)
	{
		QString wordStr = item->text(0),
				valueStr = item->text(1);

		wordsList[wordStr] = valueStr;

		while ((item = list->itemBelow(item)))
		{
			wordStr = item->text(0);
			valueStr = item->text(1);
			wordsList[wordStr] = valueStr;
		}
	}

	saveList();

	kdebugf2();
}

void WordFix::saveList()
{
	kdebugf();

	QStringList list;
	for (QMap<QString, QString>::const_iterator i = wordsList.constBegin(); i != wordsList.constEnd(); ++i)
		list.append(i.key() + '\t' + i.value());

	Application::instance()->configuration()->deprecatedApi()->writeEntry("word_fix", "WordFix_list", list.join("\t\t"));

	kdebugf2();
}

Q_EXPORT_PLUGIN2(word_fix, WordFix)

#include "moc_word-fix.cpp"
