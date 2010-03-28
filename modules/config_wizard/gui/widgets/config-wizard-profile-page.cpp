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

#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>
#include <QtGui/QTextBrowser>

#include "config-wizard-profile-page.h"

ConfigWizardProfilePage::ConfigWizardProfilePage(QWidget *parent) :
		QWizardPage(parent)
{
	createGui();
}

ConfigWizardProfilePage::~ConfigWizardProfilePage()
{
}

void ConfigWizardProfilePage::createGui()
{
	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	mainLayout->setSpacing(5);

	QTextBrowser *descriptionPane = new QTextBrowser(this);
	descriptionPane->setText(tr("<h3>Welcome to Kadu Instant Messenger</h3>"
		"<p>This wizard will help you to configure the basic settings of Kadu.</p>"
		"<p>Please choose a preffered language and create a nickname</p>"));

	mainLayout->addWidget(descriptionPane, 2);

	QWidget *formWidget = new QWidget(this);
	QFormLayout *formLayout = new QFormLayout(formWidget);

	mainLayout->addWidget(formWidget, 5);

	formLayout->addRow(new QLabel(tr("<h3>Profile setup</h3>"), this));
}
