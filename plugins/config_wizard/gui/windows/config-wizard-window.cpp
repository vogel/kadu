/*
 * %kadu copyright begin%
 * Copyright 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "plugin/plugin-injected-factory.h"

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
}

ConfigWizardWindow::~ConfigWizardWindow()
{
}

void ConfigWizardWindow::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
	m_pluginInjectedFactory = pluginInjectedFactory;
}

void ConfigWizardWindow::setProtocolsManager(ProtocolsManager *protocolsManager)
{
	m_protocolsManager = protocolsManager;
}

void ConfigWizardWindow::init()
{
	setWindowRole("kadu-wizard");
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Kadu Wizard"));

	#ifdef Q_OS_WIN
		// NOTE: Workaround for bug #1912.
		// TODO: Remove this as soon as QTBUG-10478 is fixed in
		// a release we bundle in the Windows build.
		setWizardStyle(QWizard::ModernStyle);
	#endif
	setMinimumSize(500, 500);

	setPage(ProfilePage, m_pluginInjectedFactory->makeInjected<ConfigWizardProfilePage>(this));
	setPage(ChooseNetworkPage, m_pluginInjectedFactory->makeInjected<ConfigWizardChooseNetworkPage>(this));
	setPage(SetUpAccountPage, m_pluginInjectedFactory->makeInjected<ConfigWizardSetUpAccountPage>(this));
	setPage(CompletedPage, m_pluginInjectedFactory->makeInjected<ConfigWizardCompletedPage>(this));

	connect(this, SIGNAL(accepted()), this, SLOT(acceptedSlot()));
	connect(this, SIGNAL(rejected()), this, SLOT(rejectedSlot()));
}

void ConfigWizardWindow::setPage(int id, ConfigWizardPage *page)
{
	ConfigWizardPages.append(page);

	QWizard::setPage(id, page);
}

bool ConfigWizardWindow::goToChooseNetwork() const
{
	return m_protocolsManager->count() > 0;
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

#include "moc_config-wizard-window.cpp"
