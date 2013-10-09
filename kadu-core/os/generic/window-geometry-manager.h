/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

/**
 * @addtogroup OS
 * @{
 */

/**
 * @class WindowGeometryManager
 * @short Class for initializing and book-keeping of a window's geometry data.
 * @author Bartosz 'beevvy' Brachaczek
 *
 * Objects of this class keep always up-to-date (with 100 ms delay, to be precise)
 * information about a window's geometry data. The information is set and accessed
 * through an object implementing VariantMap interface that can be passed through
 * constructor. Note that ownership of that object is taken by objects of this class.
 *
 * Constructor of this class uses advanced techniques, including using QWidget::restoreGeometry()
 * method and hacks around it, to restore all geometry information of the managed window
 * that is already saved in the resource behind the object implementing VariantMap interface.
 * If that resource does not contain any information yet, default geometry is set, which
 * can be passed through constructor.
 *
 * There is also an alternative way of providing default geometry that takes precedence
 * over the geometry passed in constructor. That is, you can provide default geometry
 * by writing result of rectToString() Kadu global helper function to the resource
 * behind the object implementing VariantMap interface.
 *
 * However constructor requires that the managed widget is actually a window, this class
 * handles possible parent changes of the widge which can result in the widget stopping
 * being a window. In such case all actions of this class are stopped until the widget
 * becomes a window again. When it happens, this class also restores original geometry
 * of the window. This is useful for example for Kadu main window, which may be embedded
 * in a window provided by single_window plugin.
 */
class KADUAPI WindowGeometryManager : public QObject
{
	Q_OBJECT

	VariantWrapper *MyVariantWrapper;
	QRect DefaultGeometry;
	QRect NormalGeometry;
	QTimer Timer;

	void restoreGeometry();

	virtual bool eventFilter(QObject *watched, QEvent *event);

private slots:
	void saveGeometry();

public:
	/**
	 * @short Constructs object book-keeping the window's geometry and sets its initial geometry.
	 * @author Bartosz 'beevvy' Brachaczek
	 * @param variantWrapper an object implementing VariantWrapper interface
	 * @param defaultGeometry default initial geometry of the window, if no is provided by variantWrapper
	 * @param window the window whose geometry will be managed
	 *
	 * Constructs object book-keeping the window's geometry through @p variantWrapper
	 * and sets the window's initial geometry @p defaultGeometry or one provided by
	 * @p variantWrapper if it can provide any.
	 *
	 * Ownership of @p variantWrapper object is passed to this object.
	 *
	 * QWidget @p window must be actually a window when calling this constructor,
	 * i.e., QWidget::isWindow() must return true.
	 */
	explicit WindowGeometryManager(VariantWrapper *variantWrapper, const QRect &defaultGeometry, QWidget *window = 0);
	virtual ~WindowGeometryManager();

};

/**
 * @}
 */

#endif // WINDOW_GEOMETRY_MANAGER_H
