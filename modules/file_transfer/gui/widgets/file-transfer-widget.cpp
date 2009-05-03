/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QFileInfo>
#include <QtCore/QTimer>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>

#include "contacts/contact.h"
#include "file-transfer/file-transfer.h"
#include "file-transfer/file-transfer-manager.h"

#include "debug.h"
#include "icons-manager.h"
#include "message_box.h"

#include "file-transfer-widget.h"

FileTransferWidget::FileTransferWidget(FileTransfer *ft, QWidget *parent)
	: QFrame(parent), CurrentTransfer(ft), UpdateTimer(0)
{
	kdebugf();

	connect(CurrentTransfer, SIGNAL(statusChanged()), this, SLOT(fileTransferStatusChanged()));
	connect(CurrentTransfer, SIGNAL(destroyed(QObject *)), this, SLOT(fileTransferDestroyed(QObject *)));

	LastTransferredSize = CurrentTransfer->transferredSize();

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
	}
}

void FileTransferWidget::fileTransferDestroyed(QObject *)
{
	disconnect(CurrentTransfer, SIGNAL(statusChanged()), this, SLOT(fileTransferStatusChanged()));
	disconnect(CurrentTransfer, SIGNAL(destroyed(QObject *)), this, SLOT(fileTransferDestroyed(QObject *)));

	CurrentTransfer = 0;
	deleteLater();
}

void FileTransferWidget::createGui()
{
	setBackgroundRole(QPalette::Base);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	setMinimumSize(QSize(100, 100));

	setFrameStyle(QFrame::Box | QFrame::Sunken);
	setLineWidth(1);

	QGridLayout *layout = new QGridLayout(this);
	layout->setMargin(10);
	layout->setColumnStretch(0, 1);
	layout->setColumnStretch(1, 20);
	layout->setColumnStretch(2, 20);

	QLabel *icon = new QLabel(this);
	icon->setBackgroundRole(QPalette::Base);
	layout->addWidget(icon, 0, 0, 3, 1);

	DescriptionLabel = new QLabel(this);
	DescriptionLabel->setBackgroundRole(QPalette::Base);
	DescriptionLabel->setScaledContents(true);
	layout->addWidget(DescriptionLabel, 0, 1, 1, 2);

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
	connect(deleteThis, SIGNAL(clicked()), this, SLOT(removeTransfer()));

	buttons_layout->addWidget(PauseButton);
	buttons_layout->addWidget(ContinueButton);
	buttons_layout->addWidget(deleteThis);
	buttons->setLayout(buttons_layout);
 	layout->addWidget(buttons, 2, 2, Qt::AlignRight);

	Contact contact = CurrentTransfer->contact();

	QString fileName = QFileInfo(CurrentTransfer->localFileName()).fileName();

	if (FileTransfer::TypeSend == CurrentTransfer->transferType())
	{
		icon->setPixmap(IconsManager::instance()->loadPixmap("FileTransferSend"));
		DescriptionLabel->setText(tr("<b>File</b> %1 <b>to</b> %2").arg(fileName).arg(contact.display()));
	}
	else
	{
		icon->setPixmap(IconsManager::instance()->loadPixmap("FileTransferReceive"));
		DescriptionLabel->setText(tr("<b>File</b> %1 <b>from</b> %2").arg(fileName).arg(contact.display()));
	}
}

void FileTransferWidget::removeTransfer()
{
	kdebugf();

	if (!CurrentTransfer)
		return;

	if (FileTransfer::StatusFinished != CurrentTransfer->transferStatus())
		if (!MessageBox::ask(tr("Are you sure you want to remove this transfer?"), QString::null, this))
			return;
		else
			CurrentTransfer->stop();

	FileTransferManager::instance()->removeFileTransfer(CurrentTransfer);

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
	if (FileTransfer::StatusTransfer == CurrentTransfer->transferStatus())
	{
		if (!UpdateTimer)
		{
			UpdateTimer = new QTimer(this);
			connect(UpdateTimer, SIGNAL(timeout()), this, SLOT(fileTransferUpdate()));
			UpdateTimer->setSingleShot(false);
			UpdateTimer->start(2500);
		}
	}
	else
	{
		if (UpdateTimer)
		{
			delete UpdateTimer;
			UpdateTimer = 0;
		}
	}

	fileTransferUpdate();
}

void FileTransferWidget::fileTransferUpdate()
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

	if (FileTransfer::StatusFinished != CurrentTransfer->transferStatus())
		ProgressBar->setValue(CurrentTransfer->percent());
	else
		ProgressBar->setValue(100);

	unsigned long speed = 0;

	if (LastUpdateTime.isValid())
	{
		QDateTime now = QDateTime::currentDateTime();
		int timeDiff = now.toTime_t() - LastUpdateTime.toTime_t();
		if (0 < timeDiff)
			speed = (CurrentTransfer->transferredSize() - LastTransferredSize) / 1024;
	}

	LastUpdateTime = QDateTime::currentDateTime();
	LastTransferredSize = CurrentTransfer->transferredSize();

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

		case FileTransfer::StatusWaitingForAccept:
			StatusLabel->setText(tr("<b>Wait for accept</b>"));
			break;

		case FileTransfer::StatusTransfer:
			// TOdO: 0.6.6
			StatusLabel->setText(tr("<b>Transfer</b>: %1 kB/s").arg(QString::number(speed)));
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
