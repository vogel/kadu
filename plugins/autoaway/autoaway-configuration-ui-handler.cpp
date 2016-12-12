/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "autoaway-configuration-ui-handler.h"

#include "widgets/configuration/configuration-widget.h"
#include "windows/main-configuration-window.h"

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>

AutoawayConfigurationUiHandler::AutoawayConfigurationUiHandler(QObject *parent) :
		QObject{parent}
{
}

AutoawayConfigurationUiHandler::~AutoawayConfigurationUiHandler()
{
}

void AutoawayConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	m_autoAwaySpinBox = static_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("autoaway/autoAway"));
	m_autoExtendedAwaySpinBox = static_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("autoaway/autoExtendedAway"));
	m_autoInvisibleSpinBox = static_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("autoaway/autoInvisible"));
	m_autoOfflineSpinBox = static_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("autoaway/autoOffline"));
	m_autoRefreshSpinBox = static_cast<QSpinBox *>(mainConfigurationWindow->widget()->widgetById("autoaway/autoRefresh"));

	m_descriptionTextLineEdit = static_cast<QLineEdit *>(mainConfigurationWindow->widget()->widgetById("autoaway/descriptionText"));

	connect(m_autoAwaySpinBox, SIGNAL(valueChanged(int)), this, SLOT(autoAwaySpinBoxValueChanged(int)));
	connect(m_autoExtendedAwaySpinBox, SIGNAL(valueChanged(int)), this, SLOT(autoExtendedAwaySpinBoxValueChanged(int)));
	connect(m_autoInvisibleSpinBox, SIGNAL(valueChanged(int)), this, SLOT(autoInvisibleSpinBoxValueChanged(int)));
	connect(m_autoOfflineSpinBox, SIGNAL(valueChanged(int)), this, SLOT(autoOfflineSpinBoxValueChanged(int)));

	connect(mainConfigurationWindow->widget()->widgetById("autoaway/descriptionChange"), SIGNAL(activated(int)), this, SLOT(descriptionChangeChanged(int)));

	m_autoRefreshSpinBox->setSpecialValueText(tr("Don't refresh"));
}

void AutoawayConfigurationUiHandler::mainConfigurationWindowDestroyed()
{
}

void AutoawayConfigurationUiHandler::mainConfigurationWindowApplied()
{
}

void AutoawayConfigurationUiHandler::autoAwaySpinBoxValueChanged(int value)
{
	if (m_autoInvisibleSpinBox->value() < value)
		m_autoInvisibleSpinBox->setValue(value);
	if (m_autoExtendedAwaySpinBox->value() < value)
		m_autoExtendedAwaySpinBox->setValue(value);
}

void AutoawayConfigurationUiHandler::autoExtendedAwaySpinBoxValueChanged(int value)
{
	if (m_autoInvisibleSpinBox->value() < value)
		m_autoInvisibleSpinBox->setValue(value);
	if (m_autoAwaySpinBox->value() > value)
		m_autoAwaySpinBox->setValue(value);
}

void AutoawayConfigurationUiHandler::autoInvisibleSpinBoxValueChanged(int value)
{
	if (m_autoAwaySpinBox->value() > value)
		m_autoAwaySpinBox->setValue(value);
	if (m_autoExtendedAwaySpinBox->value() > value)
		m_autoExtendedAwaySpinBox->setValue(value);
	if (m_autoOfflineSpinBox->value() < value)
		m_autoOfflineSpinBox->setValue(value);
}

void AutoawayConfigurationUiHandler::autoOfflineSpinBoxValueChanged(int value)
{
	if (m_autoInvisibleSpinBox->value() > value)
		m_autoInvisibleSpinBox->setValue(value);
}

void AutoawayConfigurationUiHandler::descriptionChangeChanged(int index)
{
	m_descriptionTextLineEdit->setEnabled(index != 0);
	m_autoRefreshSpinBox->setEnabled(index != 0);
}

#include "moc_autoaway-configuration-ui-handler.cpp"
