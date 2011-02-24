/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef FILE_TRANSFER_WIDGET
#define FILE_TRANSFER_WIDGET

#include <QtCore/QDateTime>
#include <QtGui/QFrame>

#include "file-transfer/file-transfer.h"

class QLabel;
class QProgressBar;
class QPushButton;
class QTimer;

class FileTransferWidget : public QFrame
{
	Q_OBJECT

	FileTransfer CurrentTransfer;

	QDateTime LastUpdateTime;
	unsigned long LastTransferredSize;
	unsigned long Speed;

	QLabel *DescriptionLabel;
	QLabel *StatusLabel;
	QProgressBar *ProgressBar;
	QPushButton *PauseButton;
	QPushButton *ContinueButton;

	void createGui();

	FileTransferHandler * handler();

private slots:
	void fileTransferUpdate();

	void removeTransfer();
	void pauseTransfer();
	void continueTransfer();

public:
	explicit FileTransferWidget(FileTransfer fileTransfer = FileTransfer::null, QWidget *parent = 0);
	virtual ~FileTransferWidget();

	FileTransfer fileTransfer() { return CurrentTransfer; }

};

#endif // FILE_TRANSFER_WIDGET
