/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QFileInfo>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>

#include "contacts/contact.h"
#include "file-transfer/file-transfer.h"

#include "debug.h"
#include "icons_manager.h"
#include "message_box.h"

#include "file-transfer-widget.h"

FileTransferWidget::FileTransferWidget(FileTransfer *ft, QWidget *parent)
	: QFrame(parent), CurrentTransfer(ft)
{
	kdebugf();

	printf("transfer: %p\n", CurrentTransfer);

	connect(CurrentTransfer, SIGNAL(statusChanged()), this, SLOT(fileTransferStatusChanged()));
	connect(CurrentTransfer, SIGNAL(destroyed(QObject *)), this, SLOT(fileTransferDestroyed(QObject *)));

	createGui();
	fileTransferStatusChanged();

	show();
}

FileTransferWidget::~FileTransferWidget()
{
	kdebugf();

	if (CurrentTransfer)
	{
		disconnect(CurrentTransfer, SIGNAL(statusChanged()), this, SLOT(fileTransferStatusChanged()));
		disconnect(CurrentTransfer, SIGNAL(destroyed(QObject *)), this, SLOT(fileTransferDestroyed(QObject *)));

		delete CurrentTransfer;
		CurrentTransfer = 0;
	}
}

void FileTransferWidget::fileTransferDestroyed(QObject *)
{
	disconnect(CurrentTransfer, SIGNAL(statusChanged()), this, SLOT(fileTransferStatusChanged()));
	disconnect(CurrentTransfer, SIGNAL(destroyed(QObject *)), this, SLOT(fileTransferDestroyed(QObject *)));

	CurrentTransfer = 0;
}

void FileTransferWidget::createGui()
{
	setBackgroundRole(QPalette::Base);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

	setMinimumSize(QSize(100, 100));

	setFrameStyle(QFrame::Box | QFrame::Sunken);
	setLineWidth(1);

	QGridLayout *layout = new QGridLayout(this, 3, 4, 2);
	layout->setMargin(10);
	layout->setColumnStretch(0, 1);
	layout->setColumnStretch(1, 20);
	layout->setColumnStretch(2, 20);

	QLabel *icon = new QLabel(this);
	icon->setBackgroundRole(QPalette::Base);
	layout->addWidget(icon, 0, 2, 0, 0);

	DescriptionLabel = new QLabel(this);
	DescriptionLabel->setBackgroundRole(QPalette::Base);
	DescriptionLabel->setScaledContents(true);
	layout->addWidget(DescriptionLabel, 0, 0, 1, 2);

	ProgressBar = new QProgressBar;
	ProgressBar->setMinimum(0);
	ProgressBar->setMaximum(100);
	ProgressBar->setBackgroundRole(QPalette::Base);
	layout->addWidget(ProgressBar, 1, 1, 1, 2);

	StatusLabel = new QLabel(this);
	StatusLabel->setBackgroundRole(QPalette::Base);
	layout->addWidget(StatusLabel, 2, 1);

	QWidget *buttons = new QWidget;
	QHBoxLayout *buttons_layout = new QHBoxLayout;
	buttons->setBackgroundRole(QPalette::Base);
	buttons_layout->setSpacing(2);

	PauseButton = new QPushButton(tr("Pause"), this);
	PauseButton->hide();
	connect(PauseButton, SIGNAL(clicked()), this, SLOT(pauseTransfer()));

	ContinueButton = new QPushButton(tr("Continue"), this);
	ContinueButton->hide();
	connect(ContinueButton, SIGNAL(clicked()), this, SLOT(continueTransfer()));

	QPushButton *deleteThis = new QPushButton(tr("Remove"), this);
	connect(deleteThis, SIGNAL(clicked()), this, SLOT(remove()));

	buttons_layout->addWidget(PauseButton);
	buttons_layout->addWidget(ContinueButton);
	buttons_layout->addWidget(deleteThis);
	buttons->setLayout(buttons_layout);
 	layout->addWidget(buttons, 2, 2, Qt::AlignRight);

	Contact contact = CurrentTransfer->contact();

	QString fileName = QFileInfo(CurrentTransfer->localFileName()).fileName();

	if (FileTransfer::TypeSend == CurrentTransfer->transferType())
	{
		icon->setPixmap(icons_manager->loadPixmap("FileTransferSend"));
		DescriptionLabel->setText(tr("<b>File</b> %1 <b>to</b> %2").arg(fileName).arg(contact.display()));
	}
	else
	{
		icon->setPixmap(icons_manager->loadPixmap("FileTransferReceive"));
		DescriptionLabel->setText(tr("<b>File</b> %1 <b>from</b> %2").arg(fileName).arg(contact.display()));
	}
}

void FileTransferWidget::remove()
{
	kdebugf();

	if (!CurrentTransfer)
		return;

	if (FileTransfer::StatusFinished != CurrentTransfer->transferStatus())
		if (!MessageBox::ask(tr("Are you sure you want to remove this transfer?")))
			return;
		else
			CurrentTransfer->stop();

	delete CurrentTransfer;
	CurrentTransfer = 0;

	// it will destroy widget too, see FileTransferWidget::fileTransferDestroying
	deleteLater();
}

void FileTransferWidget::pauseTransfer()
{
	kdebugf();

	if (CurrentTransfer)
		CurrentTransfer->pause();
}

void FileTransferWidget::continueTransfer()
{
	kdebugf();

	if (CurrentTransfer)
		CurrentTransfer->restore();
}

void FileTransferWidget::fileTransferStatusChanged()
{
	if (!CurrentTransfer)
	{
		StatusLabel->setText(tr("<b>Not connected</b>"));
		PauseButton->hide();
		ContinueButton->show();
		return;
	}

	if (FileTransfer::ErrorOk != CurrentTransfer->transferError())
	{
		StatusLabel->setText(tr("<b>Error</b>"));
		PauseButton->hide();
		ContinueButton->show();
		return;
	}

	ProgressBar->setValue(CurrentTransfer->percent());

	switch (CurrentTransfer->transferStatus())
	{
		case FileTransfer::StatusNotConnected:
			StatusLabel->setText(tr("<b>Not connected</b>"));
			PauseButton->hide();
			ContinueButton->show();
			break;

		case FileTransfer::StatusWaitingForConnection:
			StatusLabel->setText(tr("<b>Wait for connection</b>"));
			break;

		case FileTransfer::StatusTransfer:
			// TOdO: 0.6.6
			StatusLabel->setText(tr("<b>Transfer</b>: %1 kB/s").arg(0 /*QString::number(ft->speed())*/));
			PauseButton->show();
			ContinueButton->hide();
			break;

		case FileTransfer::StatusFinished:
			StatusLabel->setText(tr("<b>Finished</b>"));
			PauseButton->hide();
			ContinueButton->hide();
			break;

		case FileTransfer::StatusRejected:
			StatusLabel->setText(tr("<b>Rejected</b>"));
			PauseButton->hide();
			ContinueButton->hide();
			break;

		default:
			PauseButton->hide();
			ContinueButton->hide();
	}
}
