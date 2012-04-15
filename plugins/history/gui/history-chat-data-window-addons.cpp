/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QCheckBox>
#include <QtGui/QVBoxLayout>

#include "configuration/configuration-file.h"
#include "gui/windows/chat-data-window.h"

#include "history-chat-data-window-addons.h"

HistoryChatDataWindowAddons::HistoryChatDataWindowAddons(QObject *parent) :
		QObject(parent)
{
	configurationUpdated();
	triggerAllChatDataWindowsCreated();
}

HistoryChatDataWindowAddons::~HistoryChatDataWindowAddons()
{
	triggerAllChatDataWindowsDestroyed();
}

void HistoryChatDataWindowAddons::save()
{
	ChatDataWindow *chatDataWindow = qobject_cast<ChatDataWindow *>(sender());
	Q_ASSERT(chatDataWindow);
	Q_ASSERT(StoreHistoryCheckBoxes.contains(chatDataWindow));

	if (StoreHistoryCheckBoxes.value(chatDataWindow)->isChecked())
		chatDataWindow->chat().removeProperty("history:StoreHistory");
	else
		chatDataWindow->chat().addProperty("history:StoreHistory", false, CustomProperties::Storable);
}

void HistoryChatDataWindowAddons::configurationUpdated()
{
	StoreHistory = config_file.readBoolEntry("History", "SaveChats", true);

	QList<QCheckBox *> checkBoxes = StoreHistoryCheckBoxes.values();
	foreach (QCheckBox *checkBox, checkBoxes)
		checkBox->setEnabled(StoreHistory);
}

void HistoryChatDataWindowAddons::chatDataWindowCreated(ChatDataWindow *chatDataWindow)
{
	Q_ASSERT(!StoreHistoryCheckBoxes.contains(chatDataWindow));

	QVBoxLayout *layout = static_cast<QVBoxLayout *>(chatDataWindow->generalTab()->layout());

	QCheckBox *historyCheckBox = new QCheckBox(tr("Store history"), chatDataWindow);
	layout->insertWidget(3, historyCheckBox);

	historyCheckBox->setChecked(chatDataWindow->chat().property("history:StoreHistory", true).toBool());
	historyCheckBox->setEnabled(StoreHistory);

	StoreHistoryCheckBoxes.insert(chatDataWindow, historyCheckBox);

	connect(chatDataWindow, SIGNAL(save()), this, SLOT(save()));
}

void HistoryChatDataWindowAddons::chatDataWindowDestroyed(ChatDataWindow *chatDataWindow)
{
	Q_ASSERT(StoreHistoryCheckBoxes.contains(chatDataWindow));

	QCheckBox *historyCheckBox = StoreHistoryCheckBoxes.value(chatDataWindow);
	delete historyCheckBox;

	StoreHistoryCheckBoxes.remove(chatDataWindow);

	disconnect(chatDataWindow, 0, this, 0);
}
