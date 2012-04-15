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
#include "gui/windows/buddy-data-window.h"

#include "history-buddy-data-window-addons.h"

HistoryBuddyDataWindowAddons::HistoryBuddyDataWindowAddons(QObject *parent) :
		QObject(parent)
{
	configurationUpdated();
	triggerAllBuddyDataWindowsCreated();
}

HistoryBuddyDataWindowAddons::~HistoryBuddyDataWindowAddons()
{
	triggerAllBuddyDataWindowsDestroyed();
}

void HistoryBuddyDataWindowAddons::save()
{
	BuddyDataWindow *buddyDataWindow = qobject_cast<BuddyDataWindow *>(sender());
	Q_ASSERT(buddyDataWindow);
	Q_ASSERT(StoreHistoryCheckBoxes.contains(buddyDataWindow));

	if (!buddyDataWindow->buddy())
		return;

	if (StoreHistoryCheckBoxes.value(buddyDataWindow)->isChecked())
		buddyDataWindow->buddy().removeProperty("history:StoreHistory");
	else
		buddyDataWindow->buddy().addProperty("history:StoreHistory", false, CustomProperties::Storable);
}

void HistoryBuddyDataWindowAddons::configurationUpdated()
{
	StoreHistory = config_file.readBoolEntry("History", "SaveChats", true);

	QList<QCheckBox *> checkBoxes = StoreHistoryCheckBoxes.values();
	foreach (QCheckBox *checkBox, checkBoxes)
		checkBox->setEnabled(StoreHistory);
}

void HistoryBuddyDataWindowAddons::buddyDataWindowCreated(BuddyDataWindow *buddyDataWindow)
{
	Q_ASSERT(!StoreHistoryCheckBoxes.contains(buddyDataWindow));

	QWidget *optionsTab = buddyDataWindow->optionsTab();
	QVBoxLayout *optionsLayout = static_cast<QVBoxLayout *>(optionsTab->layout());

	QCheckBox *historyCheckBox = new QCheckBox(tr("Store history"), optionsTab);
	// insert before final stretch
	optionsLayout->insertWidget(optionsLayout->count() - 1, historyCheckBox);

	historyCheckBox->setChecked(buddyDataWindow->buddy().property("history:StoreHistory", true).toBool());
	historyCheckBox->setEnabled(StoreHistory);

	StoreHistoryCheckBoxes.insert(buddyDataWindow, historyCheckBox);

	connect(buddyDataWindow, SIGNAL(save()), this, SLOT(save()));
}

void HistoryBuddyDataWindowAddons::buddyDataWindowDestroyed(BuddyDataWindow *buddyDataWindow)
{
	Q_ASSERT(StoreHistoryCheckBoxes.contains(buddyDataWindow));

	QCheckBox *historyCheckBox = StoreHistoryCheckBoxes.value(buddyDataWindow);
	delete historyCheckBox;

	StoreHistoryCheckBoxes.remove(buddyDataWindow);

	disconnect(buddyDataWindow, 0, this, 0);
}
