/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#pragma once

#include "file-transfer/file-transfer.h"
#include "misc/memory.h"

#include <QtCore/QDateTime>
#include <QtWidgets/QFrame>

class QLabel;
class QProgressBar;
class QPushButton;

class FileTransferWidget : public QFrame
{
	Q_OBJECT

public:
	explicit FileTransferWidget(FileTransfer transfer = FileTransfer::null, QWidget *parent = nullptr);
	virtual ~FileTransferWidget();

	FileTransfer fileTransfer() const;

private:
	FileTransfer m_transfer;

	QDateTime m_lastUpdateTime;
	unsigned long m_lastTransferredSize;
	unsigned long m_speed;

	owned_qptr<QLabel> m_descriptionLabel;
	owned_qptr<QLabel> m_statusLabel;
	owned_qptr<QProgressBar> m_progressBar;
	owned_qptr<QPushButton> m_startButton;
	owned_qptr<QPushButton> m_stopButton;

	void createGui();

private slots:
	void fileTransferUpdate();

	void startTransfer();
	void stopTransfer();
	void removeTransfer();

};