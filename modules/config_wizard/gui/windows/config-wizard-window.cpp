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

#include <QtCore/QVariant>

#include "debug.h"

#include "gui/widgets/config-wizard-choose-network-page.h"
#include "gui/widgets/config-wizard-completed-page.h"
#include "gui/widgets/config-wizard-profile-page.h"
#include "gui/widgets/config-wizard-set-up-account-page.h"

#include "config-wizard-window.h"

/**
 * @ingroup config_wizard
 * @{
 */

ConfigWizardWindow::ConfigWizardWindow(QWidget *parent) :
		QWizard(parent)
{
	kdebugf();

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Kadu Wizard"));

	setOption(IndependentPages, true);

#ifdef Q_OS_MAC
	/* MacOSX has it's own QWizard style which requires much more space
	 * than the other ones so we're forcing the ClassicStyle to unify
	 * the window sizes and look. Mac users will love us for that.
	 */
	setWizardStyle(QWizard::ClassicStyle);
#else
	setMinimumSize(710, 300);
#endif

	setPage(ProfilePage, new ConfigWizardProfilePage(this));
	setPage(ChooseNetworkPage, new ConfigWizardChooseNetworkPage(this));
	setPage(SetUpAccountPage, new ConfigWizardSetUpAccountPage(this));
	setPage(CompletedPage, new ConfigWizardCompletedPage(this));

	connect(this, SIGNAL(accepted()), this, SLOT(acceptedSlot()));
	connect(this, SIGNAL(rejected()), this, SLOT(rejectedSlot()));

	kdebugf2();
}

ConfigWizardWindow::~ConfigWizardWindow()
{
	kdebugf();
	kdebugf2();
}

void ConfigWizardWindow::setPage(int id, ConfigWizardPage *page)
{
	ConfigWizardPages.append(page);

	QWizard::setPage(id, page);
}

int ConfigWizardWindow::nextId() const
{
	switch (currentId())
	{
		case ProfilePage:
			return ChooseNetworkPage;
		case ChooseNetworkPage:
		{
			if (field("choose-network.ignore").toBool())
				return CompletedPage;
			else
				return SetUpAccountPage;
		}
		case SetUpAccountPage:
			return CompletedPage;
		case CompletedPage:
		default:
			return -1;
	}
}

void ConfigWizardWindow::acceptedSlot()
{
	foreach (ConfigWizardPage *page, ConfigWizardPages)
		page->acceptPage();
}

void ConfigWizardWindow::rejectedSlot()
{
	foreach (ConfigWizardPage *page, ConfigWizardPages)
		page->rejectPage();
}

/** @} */
