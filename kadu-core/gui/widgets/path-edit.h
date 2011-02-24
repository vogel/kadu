/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
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

#ifndef PATH_EDIT_H
#define PATH_EDIT_H

#include <QtGui/QWidget>
#include "exports.h"

class QLineEdit;

class KADUAPI PathEdit : public QWidget
{
	Q_OBJECT

	QString DialogTitle;
	QLineEdit *PathLineEdit;

	void createGui();

private slots:
	void selectPath();

public:
	explicit PathEdit(const QString &dialogTitle, QWidget *parent);
	virtual ~PathEdit();

	QString path();

signals:
	void pathChanged(const QString &path);

};

#endif // PATH_EDIT_H
