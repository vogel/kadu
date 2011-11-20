/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
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

#include <QtGui/QApplication>
#include <QtGui/QCloseEvent>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>

#include "gui/widgets/progress-label.h"

#include "progress-window.h"

ProgressWindow::ProgressWindow(QWidget *parent) :
		QDialog(parent), DesktopAwareObject(this), CanClose(false)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowRole("kadu-progress");
	createGui();
}

ProgressWindow::~ProgressWindow()
{
}

void ProgressWindow::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	Progress = new ProgressLabel(tr("Plase wait. New Gadu-Gadu account is being registered."), this);
	mainLayout->addWidget(Progress);

	Container = new QWidget(this);
	new QVBoxLayout(Container);
	mainLayout->addWidget(Container);

	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	CloseButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCloseButton), tr("Close"));
	CloseButton->setEnabled(false);
	connect(CloseButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	buttons->addButton(CloseButton, QDialogButtonBox::DestructiveRole);

	mainLayout->addWidget(buttons);

	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
}

void ProgressWindow::enableClosing(bool enable)
{
	CanClose = enable;
	CloseButton->setEnabled(enable);

	if (enable)
	{
		CloseButton->setDefault(true);
		CloseButton->setFocus();
	}
}

void ProgressWindow::closeEvent(QCloseEvent *e)
{
	if (!CanClose)
		e->ignore();
	else
		QDialog::closeEvent(e);
}

void ProgressWindow::setState(ProgressIcon::ProgressState state, const QString &text, bool enableClosingInProgress)
{
	if (ProgressIcon::StateInProgress != state)
		CloseButton->setText(tr("Close"));
	else
		CloseButton->setText(tr("Cancel"));

	enableClosing(enableClosingInProgress || (ProgressIcon::StateInProgress != state));
	Progress->setState(state, text);
}

void ProgressWindow::setText(const QString &text)
{
	Progress->setText(text);
}
