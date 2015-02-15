/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QKeyEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>

#include "accounts/filter/have-multilogon-filter.h"
#include "configuration/config-file-variant-wrapper.h"
#include "gui/widgets/accounts-combo-box.h"
#include "icons/icons-manager.h"
#include "model/roles.h"
#include "multilogon/model/multilogon-model.h"
#include "multilogon/multilogon-session.h"
#include "os/generic/window-geometry-manager.h"
#include "protocols/protocol.h"
#include "protocols/services/multilogon-service.h"
#include "activate.h"

#include "multilogon-window.h"

MultilogonWindow *MultilogonWindow::Instance = 0;

MultilogonWindow::MultilogonWindow(QWidget *parent) :
		QWidget(parent), DesktopAwareObject(this)
{
	setWindowRole("kadu-multilogon");

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Multilogon window"));

	createGui();

	new WindowGeometryManager(new ConfigFileVariantWrapper("General", "MultilogonWindowGeometry"), QRect(0, 50, 450, 300), this);
}

MultilogonWindow::~MultilogonWindow()
{
	Instance = 0;
}

MultilogonWindow * MultilogonWindow::instance()
{
	if (!Instance)
		Instance = new MultilogonWindow();

	return Instance;
}

void MultilogonWindow::show()
{
	QWidget::show();

	_activateWindow(this);
}

void MultilogonWindow::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	QWidget *selectAccountWidget = new QWidget(this);
	QHBoxLayout *selectAccountLayout = new QHBoxLayout(selectAccountWidget);

	selectAccountLayout->addWidget(new QLabel(tr("Account:"), selectAccountWidget));
	selectAccountLayout->setMargin(0);

	Accounts = new AccountsComboBox(true, AccountsComboBox::NotVisibleWithOneRowSourceModel, selectAccountWidget);
	Accounts->addFilter(new HaveMultilogonFilter(Accounts));
	Accounts->setIncludeIdInDisplay(true);
	selectAccountLayout->addWidget(Accounts);
	selectAccountLayout->addStretch(1);

	connect(Accounts, SIGNAL(currentIndexChanged(int)), this, SLOT(accountChanged()));

	layout->addWidget(selectAccountWidget);

	SessionsTable = new QTableView(this);
	SessionsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	SessionsTable->setSelectionMode(QAbstractItemView::SingleSelection);
	SessionsTable->setSortingEnabled(true);
	SessionsTable->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	SessionsTable->horizontalHeader()->setStretchLastSection(true);
	layout->addWidget(SessionsTable);

	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	KillSessionButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCloseButton),
			tr("Disconnect session"), buttons);
	QPushButton *closeButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton),
			tr("Close"), buttons);

	KillSessionButton->setEnabled(false);
	connect(KillSessionButton, SIGNAL(clicked()), this, SLOT(killSession()));
	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

	buttons->addButton(KillSessionButton, QDialogButtonBox::DestructiveRole);
	buttons->addButton(closeButton, QDialogButtonBox::RejectRole);

	layout->addSpacing(16);
	layout->addWidget(buttons);

	accountChanged();
}

void MultilogonWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QWidget::keyPressEvent(e);
}

MultilogonService * MultilogonWindow::multilogonService()
{
	Protocol *protocol = Accounts->currentAccount().protocolHandler();
	if (!protocol)
		return 0;

	return protocol->multilogonService();
}

MultilogonSession * MultilogonWindow::multilogonSession()
{
	QItemSelectionModel *selectionModel = SessionsTable->selectionModel();
	if (!selectionModel)
		return 0;

	QModelIndex index = selectionModel->currentIndex();
	return index.data(MultilogonSessionRole).value<MultilogonSession *>();
}

void MultilogonWindow::accountChanged()
{
	delete SessionsTable->model();

	MultilogonService *service = multilogonService();
	if (!service)
		return;

	SessionsTable->setModel(new MultilogonModel(service, this));

	connect(SessionsTable->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
			this, SLOT(selectionChanged()));
}

void MultilogonWindow::selectionChanged()
{
	KillSessionButton->setEnabled(0 != multilogonSession());
}

void MultilogonWindow::killSession()
{
	MultilogonService *service = multilogonService();
	if (!service)
		return;

	MultilogonSession *session = multilogonSession();
	if (session)
		service->killSession(session);
}

#include "moc_multilogon-window.cpp"
