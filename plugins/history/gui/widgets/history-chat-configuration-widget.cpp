/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "gui/widgets/simple-configuration-value-state-notifier.h"

#include "history-chat-configuration-widget.h"

HistoryChatConfigurationWidget::HistoryChatConfigurationWidget(const Chat &chat, QWidget *parent) :
		ChatConfigurationWidget(chat, parent), StateNotifier(new SimpleConfigurationValueStateNotifier(this))
{
	setWindowTitle(tr("History"));

	createGui();
	configurationUpdated();
	loadValues();
}

HistoryChatConfigurationWidget::~HistoryChatConfigurationWidget()
{
}

void HistoryChatConfigurationWidget::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	StoreHistoryCheckBox = new QCheckBox(tr("Store history"));

	connect(StoreHistoryCheckBox, SIGNAL(stateChanged(int)), this, SLOT(updateState()));

	layout->addWidget(StoreHistoryCheckBox);
	layout->addStretch(100);
}

void HistoryChatConfigurationWidget::configurationUpdated()
{
	GlobalStoreHistory = Application::instance()->configuration()->deprecatedApi()->readBoolEntry("History", "SaveChats", true);
	StoreHistoryCheckBox->setEnabled(GlobalStoreHistory);
}

void HistoryChatConfigurationWidget::loadValues()
{
	StoreHistoryCheckBox->setChecked(chat().property("history:StoreHistory", true).toBool());
	StoreHistoryCheckBox->setEnabled(GlobalStoreHistory);
}

void HistoryChatConfigurationWidget::updateState()
{
	if (StoreHistoryCheckBox->isChecked() == chat().property("history:StoreHistory", true).toBool())
		StateNotifier->setState(StateNotChanged);
	else
		StateNotifier->setState(StateChangedDataValid);
}

const ConfigurationValueStateNotifier * HistoryChatConfigurationWidget::stateNotifier() const
{
	return StateNotifier;
}

void HistoryChatConfigurationWidget::apply()
{
	if (StoreHistoryCheckBox->isChecked())
		chat().removeProperty("history:StoreHistory");
	else
		chat().addProperty("history:StoreHistory", false, CustomProperties::Storable);

	updateState();
}

void HistoryChatConfigurationWidget::cancel()
{
	loadValues();
}
