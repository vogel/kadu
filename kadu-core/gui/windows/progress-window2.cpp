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
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>

#include "gui/windows/message-dialog.h"
#include "icons/kadu-icon.h"

#include "progress-window2.h"

ProgressWindow2::ProgressWindow2(const QString &label, QWidget *parent) :
		QDialog(parent), Label(label), Finished(false), Cancellable(false)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowRole("kadu-progress");
	createGui();

	setFixedWidth(width());
}

ProgressWindow2::~ProgressWindow2()
{
}

void ProgressWindow2::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->setMargin(12);
	mainLayout->setSpacing(16);

	QLabel *label = new QLabel(this);
	label->setText(QString("<b>%1</b>").arg(Label));
	label->setWordWrap(true);

	QWidget *progressWidget = new QWidget(this);
	QHBoxLayout *progressLayout = new QHBoxLayout(progressWidget);
	progressLayout->setMargin(0);

	ProgressBar = new QProgressBar(this);
	ProgressBar->setMinimum(0);
	ProgressBar->setMaximum(0);

	progressLayout->addWidget(new QLabel(tr("Progress:"), progressWidget));
	progressLayout->addWidget(ProgressBar);

	TextListWidget = new QListWidget(this);
	TextListWidget->setMinimumHeight(200);
	TextListWidget->hide();

	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	CloseButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCloseButton), tr("Close"));
	CloseButton->setEnabled(false);
	connect(CloseButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	ShowDetailsButton = new QPushButton(tr("Show details >>>"));
	connect(ShowDetailsButton, SIGNAL(clicked(bool)), this, SLOT(showDetailsClicked()));

	buttons->addButton(CloseButton, QDialogButtonBox::DestructiveRole);
	buttons->addButton(ShowDetailsButton, QDialogButtonBox::ActionRole);

	mainLayout->addWidget(label);
	mainLayout->addWidget(progressWidget);
	mainLayout->addWidget(TextListWidget);
	mainLayout->addStretch(1);
	mainLayout->addWidget(buttons);
}

void ProgressWindow2::setCancellable(bool cancellable)
{
	if (Cancellable == cancellable)
		return;

	Cancellable = cancellable;

	CloseButton->setEnabled(Cancellable || Finished);
	CloseButton->setDefault(Cancellable || Finished);
}

void ProgressWindow2::closeEvent(QCloseEvent *closeEvent)
{
	if (Finished)
	{
		QDialog::closeEvent(closeEvent);
		return;
	}

	if (Cancellable)
	{
		emit canceled();
		QDialog::closeEvent(closeEvent);
		return;
	}

	closeEvent->ignore();
}

void ProgressWindow2::keyPressEvent(QKeyEvent *keyEvent)
{
	if (Qt::Key_Escape != keyEvent->key() || Finished)
	{
		QDialog::keyPressEvent(keyEvent);
		return;
	}

	if (Cancellable)
	{
		emit canceled();
		QDialog::keyPressEvent(keyEvent);
		return;
	}

	keyEvent->ignore();
}

void ProgressWindow2::showDetailsClicked()
{
	ShowDetailsButton->setEnabled(false);
	TextListWidget->show();
}

void ProgressWindow2::addProgressEntry(const QString &entryIcon, const QString &entryMessage)
{
	QListWidgetItem *item = new QListWidgetItem(KaduIcon(entryIcon).icon(), entryMessage, TextListWidget);
	TextListWidget->addItem(item);
}

void ProgressWindow2::setProgressValue(int value, int maximum)
{
	ProgressBar->setMaximum(maximum);
	ProgressBar->setValue(value);
}

void ProgressWindow2::progressFinished(bool ok, const QString &entryIcon, const QString &entryMessage)
{
	Finished = true;
	CloseButton->setEnabled(true);
	CloseButton->setDefault(true);
	CloseButton->setFocus();

	ProgressBar->setMaximum(10);

	if (ok)
		ProgressBar->setValue(10);
	else
		ProgressBar->setValue(0);

	if (!entryMessage.isEmpty())
		addProgressEntry(entryIcon, entryMessage);

	qApp->alert(this);

	if (!ok && !entryMessage.isEmpty())
		MessageDialog::exec(KaduIcon(entryIcon), Label, entryMessage);
}
