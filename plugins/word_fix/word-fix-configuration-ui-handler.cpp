/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "word-fix-configuration-ui-handler.h"

#include "word-fix.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "widgets/configuration/config-group-box.h"
#include "widgets/configuration/configuration-widget.h"
#include "windows/main-configuration-window.h"

#include <QtCore/QMap>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QTreeWidget>

WordFixConfigurationUiHandler::WordFixConfigurationUiHandler(QObject *parent) :
		QObject{parent}
{
}

WordFixConfigurationUiHandler::~WordFixConfigurationUiHandler()
{
}

void WordFixConfigurationUiHandler::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void WordFixConfigurationUiHandler::setWordFix(WordFix *wordFix)
{
	m_wordFix = wordFix;
}

void WordFixConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	auto groupBox = mainConfigurationWindow->widget()->configGroupBox("Chat", "Spelling", "Words fix");

	auto widget = new QWidget(groupBox->widget());

	auto layout = new QGridLayout(widget);
	layout->setSpacing(5);
	layout->setMargin(5);

	m_list = new QTreeWidget(widget);
	layout->addWidget(m_list, 0, 0, 1, 3);

	m_wordEdit = new QLineEdit(widget);
	layout->addWidget(new QLabel(tr("A word to be replaced")), 1, 0);
	layout->addWidget(m_wordEdit, 1, 1);

	m_valueEdit = new QLineEdit(widget);
	layout->addWidget(new QLabel(tr("Value to replace with")), 2, 0);
	layout->addWidget(m_valueEdit, 2, 1);

	auto hbox = new QWidget(widget);
	m_addButton = new QPushButton(tr("Add"), hbox);
	m_changeButton = new QPushButton(tr("Change"), hbox);
	m_deleteButton = new QPushButton(tr("Delete"), hbox);
	auto hlayout = new QHBoxLayout;
	hlayout->addWidget(m_addButton);
	hlayout->addWidget(m_changeButton);
	hlayout->addWidget(m_deleteButton);
	hbox->setLayout(hlayout);
	layout->addWidget(hbox, 3, 1);

	widget->setLayout(layout);
	groupBox->addWidgets(new QLabel(tr(""), groupBox->widget()), widget);

	connect(m_list, SIGNAL(itemSelectionChanged()), this, SLOT(wordSelected()));
	connect(m_changeButton, SIGNAL(clicked()), this, SLOT(changeSelected()));
	connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(deleteSelected()));
	connect(m_addButton, SIGNAL(clicked()), this, SLOT(addNew()));
	connect(m_wordEdit, SIGNAL(returnPressed()), this, SLOT(moveToNewValue()));
	connect(m_valueEdit, SIGNAL(returnPressed()), this, SLOT(addNew()));

	m_changeButton->setEnabled(false);
	m_deleteButton->setEnabled(false);

	m_list->setAllColumnsShowFocus(true);
	m_list->setColumnCount(2);

	QStringList headers;
	headers << tr("Word") << tr("Replace with");
	m_list->setHeaderLabels(headers);

	m_list->setColumnWidth(0, 250);
	m_list->setColumnWidth(1, 246 - m_list->verticalScrollBar()->width());

	QList<QTreeWidgetItem *> items;

	for (QMap<QString, QString>::const_iterator i = m_wordFix->wordsList().constBegin(); i != m_wordFix->wordsList().constEnd(); ++i)
	{
		auto item = new QTreeWidgetItem(m_list);
		item->setText(0, i.key());
		item->setText(1, i.value());
		items.append(item);
	}
	m_list->insertTopLevelItems(0, items);
}

void WordFixConfigurationUiHandler::mainConfigurationWindowDestroyed()
{
}

void WordFixConfigurationUiHandler::mainConfigurationWindowApplied()
{
	m_wordFix->wordsList().clear();
	QTreeWidgetItem* item = m_list->itemAt(0, 0);

	if (item)
	{
		QString wordStr = item->text(0),
				valueStr = item->text(1);

		m_wordFix->wordsList()[wordStr] = valueStr;

		while ((item = m_list->itemBelow(item)))
		{
			wordStr = item->text(0);
			valueStr = item->text(1);
			m_wordFix->wordsList()[wordStr] = valueStr;
		}
	}

	saveList();
}

void WordFixConfigurationUiHandler::saveList()
{
	QStringList list;
	for (QMap<QString, QString>::const_iterator i = m_wordFix->wordsList().constBegin(); i != m_wordFix->wordsList().constEnd(); ++i)
		list.append(i.key() + '\t' + i.value());

	m_configuration->deprecatedApi()->writeEntry("word_fix", "WordFix_list", list.join("\t\t"));
}

void WordFixConfigurationUiHandler::wordSelected()
{
	QList<QTreeWidgetItem *> items = m_list->selectedItems();
	QTreeWidgetItem* item = NULL;

	if (items.isEmpty())
	{
		m_changeButton->setEnabled(false);
		m_deleteButton->setEnabled(false);
		return;
	}
	else
	{
		m_changeButton->setEnabled(true);
		m_deleteButton->setEnabled(true);
	}

	item = items.at(0);
	m_wordEdit->setText(item->text(0));
	m_valueEdit->setText(item->text(1));
}

void WordFixConfigurationUiHandler::changeSelected()
{
	QTreeWidgetItem* item = m_list->currentItem();
	if (!item)
		return;

	QString wordStr = m_wordEdit->text(),
			valueStr = m_valueEdit->text();

	item->setText(0, wordStr);
	item->setText(1, valueStr);

	m_wordEdit->clear();
	m_valueEdit->clear();

	m_changeButton->setEnabled(false);
	m_deleteButton->setEnabled(false);
}

void WordFixConfigurationUiHandler::deleteSelected()
{
	QTreeWidgetItem* item = m_list->currentItem();
	if (!item)
		return;

	QString wordStr = item->text(0);
	delete item;

	m_wordEdit->clear();
	m_valueEdit->clear();

	m_changeButton->setEnabled(false);
	m_deleteButton->setEnabled(false);
	m_list->setCurrentItem(0);
}

void WordFixConfigurationUiHandler::addNew()
{
	QString wordStr = m_wordEdit->text(),
			valueStr = m_valueEdit->text();

	if (wordStr.isEmpty())
		return;

	if (m_list->findItems(wordStr, 0, Qt::MatchExactly).isEmpty())
	{
		QTreeWidgetItem* item = new QTreeWidgetItem(m_list);
		item->setText(0, wordStr);
		item->setText(1, valueStr);
		m_list->sortItems(0, Qt::AscendingOrder);
		m_list->setCurrentItem(item);
	}

	m_wordEdit->clear();
	m_valueEdit->clear();

	m_changeButton->setEnabled(false);
	m_deleteButton->setEnabled(false);
}

void WordFixConfigurationUiHandler::moveToNewValue()
{
	// sprawdz czy podane slowo znajduje sie na liscie i jesli tak to umozliw edycje
	QList<QTreeWidgetItem *> items = m_list->findItems(m_wordEdit->text(), 0, Qt::MatchExactly);
	if (!items.isEmpty()) {
		QTreeWidgetItem *item = items.at(0);
		m_list->setCurrentItem(item);
		m_valueEdit->setText(item->text(1));
		m_changeButton->setEnabled(true);
		m_deleteButton->setEnabled(true);
		m_addButton->setEnabled(false);
	} else {
		m_changeButton->setEnabled(false);
		m_deleteButton->setEnabled(false);
		m_addButton->setEnabled(true);
	}

	m_valueEdit->setFocus();
}
