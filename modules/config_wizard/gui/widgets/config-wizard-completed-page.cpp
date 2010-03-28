/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>
#include <QtGui/QTextBrowser>

#include "modules.h"
#include "languages-manager.h"

#include "config-wizard-completed-page.h"

ConfigWizardCompletedPage::ConfigWizardCompletedPage(QWidget *parent) :
		QWizardPage(parent)
{
	createGui();
}

ConfigWizardCompletedPage::~ConfigWizardCompletedPage()
{
}

void ConfigWizardCompletedPage::createGui()
{
	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	mainLayout->setSpacing(5);

	QTextBrowser *descriptionPane = new QTextBrowser(this);
	descriptionPane->setText(tr("<p>Confirm your wizard setting to start using Kadu.</p>"));

	mainLayout->addWidget(descriptionPane, 2);

	QWidget *formWidget = new QWidget(this);
	QFormLayout *formLayout = new QFormLayout(formWidget);

	mainLayout->addWidget(formWidget, 5);

	formLayout->addRow(new QLabel(tr("<h3>Configuration Wizard Completed/h3>"), this));

	formLayout->addRow(new QLabel(tr("You are now done setting up Kadu! Click Finish to begin."), this));

	ConfigureAccouuntsCheckBox = new QCheckBox(tr("Go to Accounts Setting after closing this window"), this);

	formLayout->addRow("", ConfigureAccouuntsCheckBox);
}
