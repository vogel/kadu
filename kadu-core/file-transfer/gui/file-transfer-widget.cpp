/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy.h"
#include "contacts/contact.h"
#include "file-transfer/file-transfer-direction.h"
#include "file-transfer/file-transfer-manager.h"
#include "file-transfer/file-transfer-status.h"
#include "file-transfer/file-transfer-type.h"
#include "file-transfer/file-transfer.h"
#include "file-transfer/outgoing-file-transfer-handler.h"
#include "gui/widgets/contact-avatar-display.h"
#include "gui/windows/message-dialog.h"
#include "icons/kadu-icon.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtGui/QDesktopServices>
#include <QtWidgets/QAction>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMenu>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>

FileTransferWidget::FileTransferWidget(FileTransferManager *manager, FileTransfer transfer, QWidget *parent) :
		QWidget{parent},
		m_manager{manager},
		m_transfer{std::move(transfer)},
		m_speed{0}
{
	createGui();

	m_lastTransferredSize = m_transfer.transferredSize();
	connect(m_transfer, SIGNAL(updated()), this, SLOT(update()), Qt::QueuedConnection);
	update();

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

	auto mainLayout = new QGridLayout{this};
	mainLayout->setMargin(6);
	mainLayout->setSpacing(6);
	mainLayout->setSizeConstraint(QLayout::SetMinimumSize);

	auto descriptionLayout = new QGridLayout{this};
	descriptionLayout->setMargin(0);
	descriptionLayout->setSpacing(2);

	auto buttonsLayout = new QHBoxLayout{this};
	buttonsLayout->setMargin(0);
	buttonsLayout->setSpacing(2);

	auto avatar = new ContactAvatarDisplay{m_transfer.peer(), QSize{48, 48}, this};

	m_fileNameLabel = new QLabel{this};
	m_statusLabel = new QLabel{this};

	m_acceptButton = new QPushButton{tr("Accept"), this};
	connect(m_acceptButton.get(), SIGNAL(clicked()), this, SLOT(accept()));

	m_rejectButton = new QPushButton{tr("Reject"), this};
	connect(m_rejectButton.get(), SIGNAL(clicked()), this, SLOT(reject()));

	m_saveButton = new QPushButton{tr("Save"), this};
	connect(m_saveButton.get(), SIGNAL(clicked()), this, SLOT(save()));

	m_sendButton = new QPushButton{tr("Send"), this};
	connect(m_sendButton.get(), SIGNAL(clicked()), this, SLOT(send()));

	m_stopButton = new QPushButton{tr("Stop"), this};
	connect(m_stopButton.get(), SIGNAL(clicked()), this, SLOT(stop()));

	m_openButton = new QPushButton{tr("Open"), this};
	auto openMenu = new QMenu{m_openButton.get()};
	m_openFileAction = new QAction{tr("Open file"), openMenu};
	connect(m_openFileAction.get(), SIGNAL(triggered(bool)), this, SLOT(openFile()));
	m_openFolderAction = new QAction{tr("Open folder"), openMenu};
	connect(m_openFolderAction.get(), SIGNAL(triggered(bool)), this, SLOT(openFolder()));
	openMenu->addAction(m_openFileAction.get());
	openMenu->addAction(m_openFolderAction.get());
	m_openButton->setMenu(openMenu);

	m_removeButton = new QToolButton{this};
	m_removeButton->setAutoRaise(true);
	m_removeButton->setFixedSize({22, 22});
	m_removeButton->setIcon(KaduIcon("kadu_icons/tab-remove").icon());
	m_removeButton->setToolTip(tr("Remove"));
	connect(m_removeButton.get(), SIGNAL(clicked()), this, SLOT(remove()));

	auto icon = new QLabel{this};
	auto iconName = FileTransferDirection::Outgoing == m_transfer.transferDirection()
		? "kadu_icons/transfer-send"
		: "kadu_icons/transfer-receive";
	icon->setPixmap(KaduIcon{iconName}.icon().pixmap(22, 22));

	m_progressBar = new QProgressBar{this};
	m_progressBar->setMinimum(0);
	m_progressBar->setMaximum(100);
	m_progressBar->setMaximumHeight(8);
	m_progressBar->setTextVisible(false);

	mainLayout->addWidget(avatar, 0, 0, 2, 1, Qt::AlignTop | Qt::AlignLeft);
	mainLayout->addLayout(descriptionLayout, 0, 1, 2, 1, Qt::AlignTop | Qt::AlignLeft);
	mainLayout->addWidget(m_removeButton.get(), 0, 2, Qt::AlignTop | Qt::AlignRight);
	mainLayout->addLayout(buttonsLayout, 1, 2, Qt::AlignTop | Qt::AlignLeft);
	mainLayout->addWidget(m_progressBar.get(), 2, 0, 1, 3);

	mainLayout->setColumnStretch(0, 1);
	mainLayout->setColumnStretch(1, 100);
	mainLayout->setColumnStretch(2, 1);

	descriptionLayout->addWidget(icon, 0, 0, Qt::AlignHCenter | Qt::AlignLeft);
	descriptionLayout->addWidget(m_fileNameLabel.get(), 0, 1, Qt::AlignHCenter | Qt::AlignLeft);
	descriptionLayout->addWidget(m_statusLabel.get(), 1, 0, 1, 2, Qt::AlignTop | Qt::AlignLeft);

	descriptionLayout->setColumnStretch(0, 1);
	descriptionLayout->setColumnStretch(1, 100);

	buttonsLayout->addStretch(100);
	buttonsLayout->addWidget(m_acceptButton.get(), 1, Qt::AlignBottom);
	buttonsLayout->addWidget(m_rejectButton.get(), 1, Qt::AlignBottom);
	buttonsLayout->addWidget(m_saveButton.get(), 1, Qt::AlignBottom);
	buttonsLayout->addWidget(m_sendButton.get(), 1, Qt::AlignBottom);
	buttonsLayout->addWidget(m_stopButton.get(), 1, Qt::AlignBottom);
	buttonsLayout->addWidget(m_openButton.get(), 1, Qt::AlignBottom);
}

bool FileTransferWidget::canAccept() const
{
	if (!m_transfer.handler())
		return false;
	if (FileTransferDirection::Outgoing == m_transfer.transferDirection())
		return false;
	if (FileTransferType::Stream != m_transfer.transferType())
		return false;
	if (m_transfer.transferStatus() == FileTransferStatus::WaitingForAccept)
		return true;
	return false;
}

void FileTransferWidget::accept()
{
	if (m_manager)
		m_manager->acceptFileTransfer(m_transfer, m_transfer.localFileName());

	updateButtons();
}

bool FileTransferWidget::canReject() const
{
	if (!m_transfer.handler())
		return false;
	if (FileTransferDirection::Outgoing == m_transfer.transferDirection())
		return false;
	if (FileTransferType::Stream != m_transfer.transferType())
		return false;
	if (m_transfer.transferStatus() == FileTransferStatus::WaitingForAccept)
		return true;
	return false;
}

void FileTransferWidget::reject()
{
	if (m_manager)
		m_manager->rejectFileTransfer(m_transfer);

	updateButtons();
}

bool FileTransferWidget::canSave() const
{
	if (FileTransferDirection::Outgoing == m_transfer.transferDirection())
		return false;
	if (FileTransferStatus::WaitingForConnection == m_transfer.transferStatus())
		return false;
	if (FileTransferStatus::WaitingForAccept == m_transfer.transferStatus())
		return false;
	if (FileTransferStatus::Transfer == m_transfer.transferStatus())
		return false;
	if (FileTransferStatus::Rejected == m_transfer.transferStatus())
		return false;
	if (FileTransferType::Url != m_transfer.transferType())
		return false;
	return true;
}

void FileTransferWidget::save()
{
	m_transfer.setTransferredSize(0);
	m_lastTransferredSize = 0;

	if (m_manager)
		m_manager->acceptFileTransfer(m_transfer, {});

	updateButtons();
}

bool FileTransferWidget::canSend() const
{
	if (FileTransferDirection::Outgoing != m_transfer.transferDirection())
		return false;
	if (!m_transfer.handler())
		return false;
	if (m_transfer.transferStatus() == FileTransferStatus::NotConnected)
		return true;
	if (m_transfer.transferStatus() == FileTransferStatus::Finished)
		return true;
	if (m_transfer.transferStatus() == FileTransferStatus::Rejected)
		return true;
	return false;
}

void FileTransferWidget::send()
{
	if (!canSend())
		return;

	m_manager->sendFile(m_transfer, m_transfer.localFileName());
	updateButtons();
}

bool FileTransferWidget::canStop() const
{
	if (!m_transfer.handler())
		return false;
	if (FileTransferStatus::WaitingForConnection == m_transfer.transferStatus())
		return true;
	if (FileTransferDirection::Outgoing == m_transfer.transferDirection() && FileTransferStatus::WaitingForAccept == m_transfer.transferStatus())
		return true;
	if (FileTransferStatus::Transfer == m_transfer.transferStatus())
		return true;
	return false;
}

void FileTransferWidget::stop()
{
	auto handler = qobject_cast<OutgoingFileTransferHandler *>(m_transfer.handler());
	if (handler)
		handler->stop();

	updateButtons();
}

bool FileTransferWidget::canOpenFile() const
{
	if (!canOpenFolder())
		return false;
	if (FileTransferDirection::Outgoing == m_transfer.transferDirection())
		return true;
	if (!m_transfer.error().isEmpty())
		return false;
	if (m_transfer.transferStatus() == FileTransferStatus::Finished)
		return true;
	return false;
}

void FileTransferWidget::openFile()
{
	if (canOpenFile())
		QDesktopServices::openUrl(QUrl::fromLocalFile(m_transfer.localFileName()));
}

bool FileTransferWidget::canOpenFolder() const
{
	if (m_transfer.localFileName().isEmpty())
		return false;

	QFileInfo info{m_transfer.localFileName()};
	return info.absoluteDir().exists();
}

void FileTransferWidget::openFolder()
{
	if (canOpenFolder())
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(m_transfer.localFileName()).absoluteDir().absolutePath()));
}

bool FileTransferWidget::canRemove() const
{
	if (!m_transfer.handler())
		return true;
	if (m_transfer.transferStatus() == FileTransferStatus::NotConnected)
		return true;
	if (m_transfer.transferStatus() == FileTransferStatus::ReadyToDownload)
		return true;
	if (m_transfer.transferStatus() == FileTransferStatus::Finished)
		return true;
	if (m_transfer.transferStatus() == FileTransferStatus::Rejected)
		return true;
	return false;
}

void FileTransferWidget::remove()
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
	}

	if (m_manager)
		m_manager->removeItem(m_transfer);

	deleteLater();
}

void FileTransferWidget::update()
{
	updateButtons();
	updateFileNameLabel();
	updateStatusLabel();
	updateProgressBar();
	updateTransferData();

	// not sure if needed
	QCoreApplication::processEvents();
}

void FileTransferWidget::updateButtons()
{
	if (m_transfer.transferStatus() == FileTransferStatus::Finished)
		m_sendButton->setText(tr("Send again"));
	else
		m_sendButton->setText(tr("Send"));

	m_acceptButton->setVisible(canAccept());
	m_rejectButton->setVisible(canReject());
	m_saveButton->setVisible(canSave());
	m_sendButton->setVisible(canSend());
	m_stopButton->setVisible(canStop());
	m_openButton->setVisible(canOpenFolder());
	m_openFileAction->setEnabled(canOpenFile());
	m_removeButton->setEnabled(canRemove());
}

void FileTransferWidget::updateFileNameLabel()
{
	auto fileName = QFileInfo{m_transfer.localFileName()}.fileName();
	if (fileName.isEmpty())
		fileName = m_transfer.remoteFileName();
	m_fileNameLabel->setText(fileName);
}

void FileTransferWidget::updateStatusLabel()
{
	if (!m_transfer)
	{
		m_statusLabel->setText({});
		return;
	}

	if (!m_transfer.error().isEmpty())
	{
		m_statusLabel->setText(QString{"<b>%1</b>"}.arg(m_transfer.error()));
		return;
	}

	switch (m_transfer.transferStatus())
	{
		case FileTransferStatus::NotConnected:
			m_statusLabel->setText({});
			break;

		case FileTransferStatus::WaitingForConnection:
			m_statusLabel->setText(tr("<b>Wait for connection</b>"));
			break;

		case FileTransferStatus::WaitingForAccept:
			m_statusLabel->setText(tr("<b>Wait for accept</b>"));
			break;

		case FileTransferStatus::ReadyToDownload:
			m_statusLabel->setText(tr("<b>Ready to download</b>"));
			break;

		case FileTransferStatus::Transfer:
			m_statusLabel->setText(tr("<b>Transfer</b>: %1 kB/s").arg(QString::number(m_speed)));
			break;

		case FileTransferStatus::Finished:
			m_statusLabel->setText(tr("<b>Finished</b>"));
			break;

		case FileTransferStatus::Rejected:
			m_statusLabel->setText(tr("<b>Rejected</b>"));
			break;
	}
}

void FileTransferWidget::updateProgressBar()
{
	if (!m_transfer.error().isEmpty())
	{
		m_progressBar->setValue(0);
		return;
	}

	switch (m_transfer.transferStatus())
	{
		case FileTransferStatus::Finished:
			m_progressBar->setValue(100);
			break;
		case FileTransferStatus::Transfer:
			m_progressBar->setValue(static_cast<int>(m_transfer.percent()));
			break;
		default:
			m_progressBar->setValue(0);
			break;
	}
}

void FileTransferWidget::updateTransferData()
{
	if (!m_transfer.error().isEmpty() || FileTransferStatus::Transfer != m_transfer.transferStatus() || !m_lastUpdateTime.isValid())
	{
		m_speed = 0;
		m_lastUpdateTime = QDateTime::currentDateTime();
		m_lastTransferredSize = m_transfer.transferredSize();
		return;
	}

	auto now = QDateTime::currentDateTime();
	auto timeDiff = now.toTime_t() - m_lastUpdateTime.toTime_t();
	if (0 < timeDiff && m_lastTransferredSize <= m_transfer.transferredSize())
	{
		m_speed = ((m_transfer.transferredSize() - m_lastTransferredSize) / 1024) / timeDiff;
		m_lastUpdateTime = QDateTime::currentDateTime();
		m_lastTransferredSize = m_transfer.transferredSize();
	}
}

#include "moc_file-transfer-widget.cpp"
