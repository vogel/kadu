/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef CONFIG_WIZARD_SET_UP_ACCOUNT_PAGE_H
#define CONFIG_WIZARD_SET_UP_ACCOUNT_PAGE_H

#include <QtCore/QWeakPointer>

#include "accounts/account.h"
#include "gui/widgets/config-wizard-page.h"

class ModalConfigurationWidget;

class ConfigWizardSetUpAccountPage : public ConfigWizardPage
{
	Q_OBJECT

	QWeakPointer<ModalConfigurationWidget> AccountWidget;
	bool AccountSuccessfullyCreated;

	void createGui();

private slots:
	void accountCreated(Account account);

public:
	explicit ConfigWizardSetUpAccountPage(QWidget *parent = 0);
	virtual ~ConfigWizardSetUpAccountPage();

	virtual bool isComplete() const;

	virtual void initializePage();
	virtual void cleanupPage();
	virtual bool validatePage();

};

#endif // CONFIG_WIZARD_SET_UP_ACCOUNT_PAGE_H
