/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004, 2005 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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
#include "protocols/protocol-factory.h"
#include "protocols/protocols-manager.h"

#include "config-wizard-window.h"

/**
 * @ingroup config_wizard
 * @{
 */

ConfigWizardWindow::ConfigWizardWindow(QWidget *parent) :
		QWizard(parent)
{
	kdebugf();

	setWindowRole("kadu-wizard");
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Kadu Wizard"));

#ifdef Q_OS_MAC
	/* MacOSX has it's own QWizard style which requires much more space
	 * than the other ones so we're forcing the ClassicStyle to unify
	 * the window sizes and look. Mac users will love us for that.
	 */
	setWizardStyle(QWizard::ClassicStyle);
#else
	#ifdef Q_WS_WIN
		// NOTE: Workaround for bug #1912.
		// TODO: Remove this as soon as QTBUG-10478 is fixed in
		// a release we bundle in the Windows build.
		setWizardStyle(QWizard::ModernStyle);
	#endif
	setMinimumSize(500, 500);
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

bool ConfigWizardWindow::goToChooseNetwork() const
{
	return ProtocolsManager::instance()->count() > 0;
}

bool ConfigWizardWindow::goToAccountSetUp() const
{
	if (field("choose-network.ignore").toBool())
		return false;

	ProtocolFactory *pf = field("choose-network.protocol-factory").value<ProtocolFactory *>();
	if (!pf)
		return false;

	if (field("choose-network.new").toBool() && !pf->canRegister())
		return false;

	return true;
}

int ConfigWizardWindow::nextId() const
{
	switch (currentId())
	{
		case ProfilePage:
			return goToChooseNetwork()
					? ChooseNetworkPage
					: CompletedPage;
		case ChooseNetworkPage:
			return goToAccountSetUp()
					? SetUpAccountPage
					: CompletedPage;
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
