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

#ifndef PROGRESS_WINDOW2_H
#define PROGRESS_WINDOW2_H

#include <QtGui/QDialog>

#include "exports.h"

class QLabel;
class QProgressBar;

class KADUAPI ProgressWindow2 : public QDialog
{
	Q_OBJECT

	QLabel *TextLabel;
	QProgressBar *ProgressBar;
	QPushButton *CloseButton;

	bool CanClose;

	void createGui();

protected:
	virtual void closeEvent(QCloseEvent *);

public:
	explicit ProgressWindow2(const QString &initText, QWidget *parent = 0);
	virtual ~ProgressWindow2();

	void setText(const QString &text);
	void enableClosing();

};

#endif // PROGRESS_WINDOW_H
