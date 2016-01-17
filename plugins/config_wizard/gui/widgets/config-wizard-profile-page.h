/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/widgets/config-wizard-page.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Configuration;
class Myself;

class QComboBox;
class QLineEdit;

class ConfigWizardProfilePage : public ConfigWizardPage
{
	Q_OBJECT

	QComboBox *LanguagesCombo;
	QLineEdit *NickNameEdit;

	void createGui();
	void setLanguages();

public:
	explicit ConfigWizardProfilePage(QWidget *parent = 0);
	virtual ~ConfigWizardProfilePage();

	virtual void initializePage();
	virtual void acceptPage();

private:
	QPointer<Configuration> m_configuration;
	QPointer<Myself> m_myself;

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setMyself(Myself *myself);
	INJEQT_INIT void init();

};
