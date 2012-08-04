/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef WINDOW_GEOMETRY_MANAGER_H
#define WINDOW_GEOMETRY_MANAGER_H

#include <QtCore/QObject>
#include <QtCore/QRect>
#include <QtCore/QTimer>

#include "exports.h"

class QWidget;

class VariantWrapper;

class KADUAPI WindowGeometryManager : public QObject
{
	Q_OBJECT

	VariantWrapper *MyVariantWrapper;
	QRect DefaultGeometry;
	QRect NormalGeometry;
	QTimer Timer;

	void restoreGeometry();

private slots:
	void saveGeometry();

public:
	explicit WindowGeometryManager(VariantWrapper *variantWrapper, const QRect &defaultGeometry, QWidget *parent = 0);
	virtual ~WindowGeometryManager();

	virtual bool eventFilter(QObject *watched, QEvent *event);

};

#endif // WINDOW_GEOMETRY_MANAGER_H
