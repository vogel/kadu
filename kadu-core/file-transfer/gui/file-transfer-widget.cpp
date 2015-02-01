/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "file-transfer-widget.h"

#include "accounts/account.h"
#include "buddies/buddy.h"
#include "contacts/contact.h"
#include "file-transfer/file-transfer-error.h"
#include "file-transfer/file-transfer-handler.h"
#include "file-transfer/file-transfer-manager.h"
#include "file-transfer/file-transfer-status.h"
#include "file-transfer/file-transfer-type.h"
#include "file-transfer/file-transfer.h"
#include "gui/windows/message-dialog.h"
#include "icons/kadu-icon.h"
#include "identities/identity.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>
#include <QtCore/QTimer>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>

FileTransferWidget::FileTransferWidget(FileTransfer transfer, QWidget *parent) :
		QFrame{parent},
		m_transfer{std::move(transfer)},
		m_speed{0}
{
	createGui();

	m_lastTransferredSize = m_transfer.transferredSize();
	connect(m_transfer, SIGNAL(updated()), this, SLOT(fileTransferUpdate()), Qt::QueuedConnection);
	fileTransferUpdate();

	show();
}

FileTransferWidget::~FileTransferWidget()
{
	disconnect(m_transfer, 0, this, 0);
}

FileTransfer FileTransferWidget::fileTransfer() const
{
	return m_transfer;
}

void FileTransferWidget::createGui()
{
	setBackgroundRole(QPalette::Base);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	setMinimumSize(QSize(100, 100));

	setFrameStyle(QFrame::Box | QFrame::Sunken);
	setLineWidth(1);

	auto layout = new QGridLayout{this};
	layout->setMargin(10);
	layout->setColumnStretch(0, 1);
	layout->setColumnStretch(1, 20);
	layout->setColumnStretch(2, 20);
	layout->setSizeConstraint(QLayout::SetMinimumSize);

	auto icon = new QLabel{this};
	icon->setBackgroundRole(QPalette::Base);
	layout->addWidget(icon, 0, 0, 3, 1, Qt::AlignTop);

	m_descriptionLabel = new QLabel{this};
	m_descriptionLabel->setBackgroundRole(QPalette::Base);
	layout->addWidget(m_descriptionLabel.get(), 0, 1, 1, 2);

	m_progressBar = new QProgressBar{this};
	m_progressBar->setMinimum(0);
	m_progressBar->setMaximum(100);
	m_progressBar->setBackgroundRole(QPalette::Base);
	layout->addWidget(m_progressBar.get(), 1, 1, 1, 2);

	m_statusLabel = new QLabel{this};
	m_statusLabel->setBackgroundRole(QPalette::Base);
	layout->addWidget(m_statusLabel.get(), 2, 1);

	auto buttons = new QWidget{this};
	auto buttonsLayout = new QHBoxLayout{buttons};
	buttons->setBackgroundRole(QPalette::Base);
	buttonsLayout->setSpacing(2);

	m_stopButton = new QPushButton{tr("Stop"), this};
	m_stopButton->hide();
	connect(m_stopButton.get(), SIGNAL(clicked()), this, SLOT(stopTransfer()));

	m_startButton = new QPushButton{tr("Start"), this};
	m_startButton->hide();
	connect(m_startButton.get(), SIGNAL(clicked()), this, SLOT(startTransfer()));

	auto deleteThis = new QPushButton{tr("Remove"), this};
	connect(deleteThis, SIGNAL(clicked()), this, SLOT(removeTransfer()));

	buttonsLayout->addWidget(m_startButton.get());
	buttonsLayout->addWidget(m_stopButton.get());
	buttonsLayout->addWidget(deleteThis);
 	layout->addWidget(buttons, 2, 2, Qt::AlignRight);

	auto contact = m_transfer.peer();
	auto account = contact.contactAccount();

	auto fileName = QFileInfo{m_transfer.localFileName()}.fileName();
	if (fileName.isEmpty())
		fileName = m_transfer.remoteFileName();

	if (FileTransferType::Outgoing == m_transfer.transferType())
	{
		icon->setPixmap(KaduIcon("kadu_icons/transfer-send").icon().pixmap(64, 64));
		m_descriptionLabel->setText(tr("File <b>%1</b><br /> to <b>%2</b><br />on account <b>%3</b>")
				.arg(fileName).arg(contact.display(true)).arg(account.accountIdentity().name()));
	}
	else
	{
		icon->setPixmap(KaduIcon("kadu_icons/transfer-receive").icon().pixmap(64, 64));
		m_descriptionLabel->setText(tr("File <b>%1</b><br /> from <b>%2</b><br />on account <b>%3</b>")
				.arg(fileName).arg(contact.display(true)).arg(account.accountIdentity().name()));
	}
}

void FileTransferWidget::startTransfer()
{
	if (!m_transfer.handler())
		m_transfer.createHandler();
	if (FileTransferType::Outgoing == m_transfer.transferType() && m_transfer.handler())
		m_transfer.handler()->send();
}

void FileTransferWidget::stopTransfer()
{
	if (m_transfer.handler())
		m_transfer.handler()->stop();
}

void FileTransferWidget::removeTransfer()
{
	if (!m_transfer)
		return;

	if (FileTransferStatus::Finished != m_transfer.transferStatus())
	{
		auto dialog = MessageDialog::create(KaduIcon(), tr("Kadu"), tr("Are you sure you want to remove this transfer?"), this);
		dialog->addButton(QMessageBox::Yes, tr("Remove"));
		dialog->addButton(QMessageBox::No, tr("Cancel"));

		if (!dialog->ask())
			return;
		else
			if (m_transfer.handler())
				m_transfer.handler()->stop();
	}

	FileTransferManager::instance()->removeItem(m_transfer);

	deleteLater();
}

void FileTransferWidget::fileTransferUpdate()
{
	if (!m_transfer)
	{
		m_statusLabel->setText(tr("<b>Not connected</b>"));
		m_stopButton->hide();
		m_startButton->hide();
		return;
	}

	if (FileTransferError::NoError != m_transfer.transferError())
	{
		m_statusLabel->setText(tr("<b>Error</b>"));
		m_stopButton->hide();

		if (FileTransferType::Outgoing == m_transfer.transferType())
			m_startButton->show();
		return;
	}

	if (FileTransferStatus::Finished != m_transfer.transferStatus())
		m_progressBar->setValue(static_cast<int>(m_transfer.percent()));
	else
		m_progressBar->setValue(100);

	if (FileTransferStatus::Transfer == m_transfer.transferStatus())
	{
		if (m_lastUpdateTime.isValid())
		{
			auto now = QDateTime::currentDateTime();
			auto timeDiff = now.toTime_t() - m_lastUpdateTime.toTime_t();
			if (0 < timeDiff)
			{
				m_speed = ((m_transfer.transferredSize() - m_lastTransferredSize) / 1024) / timeDiff;
				m_lastUpdateTime = QDateTime::currentDateTime();
				m_lastTransferredSize = m_transfer.transferredSize();
			}
		}
		else
		{
			m_speed = 0;
			m_lastUpdateTime = QDateTime::currentDateTime();
			m_lastTransferredSize = m_transfer.transferredSize();
		}
	}

	switch (m_transfer.transferStatus())
	{
		case FileTransferStatus::NotConnected:
			m_statusLabel->setText(tr("<b>Not connected</b>"));
			m_stopButton->hide();
			if (FileTransferType::Outgoing == m_transfer.transferType())
				m_startButton->show();
			break;

		case FileTransferStatus::WaitingForConnection:
			m_statusLabel->setText(tr("<b>Wait for connection</b>"));
			m_startButton->hide();
			break;

		case FileTransferStatus::WaitingForAccept:
			m_statusLabel->setText(tr("<b>Wait for accept</b>"));
			m_startButton->hide();
			break;

		case FileTransferStatus::Transfer:
			m_statusLabel->setText(tr("<b>Transfer</b>: %1 kB/s").arg(QString::number(m_speed)));
			m_stopButton->show();
			m_startButton->hide();
			break;

		case FileTransferStatus::Finished:
			m_statusLabel->setText(tr("<b>Finished</b>"));
			m_stopButton->hide();
			m_startButton->hide();
			break;

		case FileTransferStatus::Rejected:
			m_statusLabel->setText(tr("<b>Rejected</b>"));
			m_stopButton->hide();
			m_startButton->hide();
			break;

		default:
			m_stopButton->hide();
			m_startButton->hide();
	}

	QCoreApplication::processEvents();
}

#include "moc_file-transfer-widget.cpp"
