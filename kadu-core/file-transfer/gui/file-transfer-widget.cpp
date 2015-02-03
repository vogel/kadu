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

#include "avatars/avatar.h"
#include "accounts/account.h"
#include "buddies/buddy.h"
#include "contacts/contact.h"
#include "file-transfer/file-transfer-error.h"
#include "file-transfer/file-transfer-handler.h"
#include "file-transfer/file-transfer-manager.h"
#include "file-transfer/file-transfer-status.h"
#include "file-transfer/file-transfer-type.h"
#include "file-transfer/file-transfer.h"
#include "gui/widgets/contact-avatar-display.h"
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
		QWidget{parent},
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
	setAutoFillBackground(true);
	setMinimumSize(QSize(100, 50));
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	auto mainLayout = new QVBoxLayout{this};
	mainLayout->setMargin(10);
	mainLayout->setSizeConstraint(QLayout::SetMinimumSize);

	auto topLayout = new QHBoxLayout{this};
	topLayout->setMargin(0);

	auto statusLayout = new QVBoxLayout{this};
	statusLayout->setMargin(0);

	auto buttonsLayout = new QVBoxLayout{this};
	buttonsLayout->setMargin(0);
	buttonsLayout->setSpacing(2);

	auto bottomLayout = new QHBoxLayout{this};
	bottomLayout->setMargin(0);

	auto avatar = new ContactAvatarDisplay{m_transfer.peer(), QSize{48, 48}, this};

	m_descriptionLabel = new QLabel{this};
	auto fileName = QFileInfo{m_transfer.localFileName()}.fileName();
	if (fileName.isEmpty())
		fileName = m_transfer.remoteFileName();
	m_descriptionLabel->setText(QString{"File: <b>%1</b>"}.arg(fileName));

	m_statusLabel = new QLabel{this};

	m_stopButton = new QPushButton{tr("Stop"), this};
	m_stopButton->hide();
	connect(m_stopButton.get(), SIGNAL(clicked()), this, SLOT(stopTransfer()));

	m_startButton = new QPushButton{tr("Start"), this};
	m_startButton->hide();
	connect(m_startButton.get(), SIGNAL(clicked()), this, SLOT(startTransfer()));

	auto deleteThis = new QPushButton{tr("Remove"), this};
	connect(deleteThis, SIGNAL(clicked()), this, SLOT(removeTransfer()));

	auto icon = new QLabel{this};
	auto iconName = FileTransferType::Outgoing == m_transfer.transferType()
		? "kadu_icons/transfer-send"
		: "kadu_icons/transfer-receive";
	icon->setPixmap(KaduIcon{iconName}.icon().pixmap(22, 22));

	m_progressBar = new QProgressBar{this};
	m_progressBar->setMinimum(0);
	m_progressBar->setMaximum(100);

	mainLayout->addLayout(topLayout);
	mainLayout->addLayout(bottomLayout);

	topLayout->addWidget(avatar, 1, Qt::AlignTop | Qt::AlignLeft);
	topLayout->addLayout(statusLayout, 100);
	topLayout->addLayout(buttonsLayout, 1);

	statusLayout->addWidget(m_descriptionLabel.get(), Qt::AlignLeft);
	statusLayout->addWidget(m_statusLabel.get());
	statusLayout->addStretch(100);

	buttonsLayout->addWidget(m_startButton.get());
	buttonsLayout->addWidget(m_stopButton.get());
	buttonsLayout->addWidget(deleteThis);
	buttonsLayout->addStretch(100);

	bottomLayout->addWidget(icon);
	bottomLayout->addWidget(m_progressBar.get());
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
