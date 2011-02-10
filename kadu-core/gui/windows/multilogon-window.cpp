/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QTableView>

#include "accounts/filter/have-multilogon-filter.h"
#include "gui/widgets/accounts-combo-box.h"
#include "misc/misc.h"
#include "activate.h"
#include "icons-manager.h"

#include "multilogon-window.h"

MultilogonWindow *MultilogonWindow::Instance = 0;

MultilogonWindow::MultilogonWindow(QWidget *parent) :
		QWidget(parent)
{
	setWindowRole("kadu-multilogon");

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Multilogon window"));

	createGui();

	loadWindowGeometry(this, "General", "MultilogonWindowGeometry", 0, 50, 700, 500);
}

MultilogonWindow::~MultilogonWindow()
{
	saveWindowGeometry(this, "General", "MultilogonWindowGeometry");

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

	AccountsComboBox *accounts = new AccountsComboBox(true, selectAccountWidget);
	accounts->addFilter(new HaveMultilogonFilter(accounts));
	accounts->setIncludeIdInDisplay(true);
	selectAccountLayout->addWidget(accounts);

	layout->addWidget(selectAccountWidget);

	layout->addWidget(new QTableView(this));

	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	QPushButton *killSessionButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCloseButton),
			tr("Disconnect session"), buttons);
	QPushButton *closeButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton),
			tr("Close"), buttons);

	buttons->addButton(killSessionButton, QDialogButtonBox::DestructiveRole);
	buttons->addButton(closeButton, QDialogButtonBox::RejectRole);

	layout->addWidget(buttons);
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
