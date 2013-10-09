/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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
#include <QtGui/QHBoxLayout>
#include <QtGui/QTextBrowser>

#include "config-wizard-page.h"

ConfigWizardPage::ConfigWizardPage(QWidget *parent) :
		QWizardPage(parent)
{
	createGui();
}

ConfigWizardPage::~ConfigWizardPage()
{
}

void ConfigWizardPage::createGui()
{
	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	mainLayout->setSpacing(5);

	DescriptionPane = new QTextBrowser(this);
	DescriptionPane->setFocusPolicy(Qt::ClickFocus);
#ifdef Q_OS_MAC
	DescriptionPane->setMinimumWidth(150);
#endif
	mainLayout->addWidget(DescriptionPane, 2);

	QWidget *formWidget = new QWidget(this);
	FormLayout = new QFormLayout(formWidget);

	mainLayout->addWidget(formWidget, 5);
}

void ConfigWizardPage::acceptPage()
{
}

void ConfigWizardPage::rejectPage()
{
}

void ConfigWizardPage::setDescription(const QString &description)
{
	DescriptionPane->setText(description);
}

#include "moc_config-wizard-page.cpp"
