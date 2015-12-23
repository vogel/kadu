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

#pragma once

#include "configuration/gui/configuration-ui-handler.h"

class QLineEdit;
class QSpinBox;

class AutoawayConfigurationUiHandler : public QObject, public ConfigurationUiHandler
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit AutoawayConfigurationUiHandler(QObject *parent = nullptr);
	virtual ~AutoawayConfigurationUiHandler();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow) override;
	virtual void mainConfigurationWindowDestroyed() override;
	virtual void mainConfigurationWindowApplied() override;

private:
	QSpinBox *m_autoAwaySpinBox;
	QSpinBox *m_autoExtendedAwaySpinBox;
	QSpinBox *m_autoInvisibleSpinBox;
	QSpinBox *m_autoOfflineSpinBox;
	QSpinBox *m_autoRefreshSpinBox;
	QLineEdit *m_descriptionTextLineEdit;

private slots:
	void autoAwaySpinBoxValueChanged(int value);
	void autoExtendedAwaySpinBoxValueChanged(int value);
	void autoInvisibleSpinBoxValueChanged(int value);
	void autoOfflineSpinBoxValueChanged(int value);
	void descriptionChangeChanged(int index);

};
