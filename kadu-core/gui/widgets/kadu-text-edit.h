/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef KADU_TEXT_EDIT_H
#define KADU_TEXT_EDIT_H

#include <QtWidgets/QTextEdit>

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class KaduTextEdit
 * @author Rafał 'Vogel' Malinowski
 * @short Improved QTextEdit widget.
 *
 * This widget is an improved version of QTextEdit.
 *
 * The only one improvement is that this widget can have different default height. This height is set in number
 * of lines to display. Use setPreferredLines() to set this value. Default number of lines is 5.
 */
class KaduTextEdit : public QTextEdit
{
	Q_OBJECT

	quint16 PreferredLines;

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new KaduTextEdit.
	 * @param parent QWidget parent of KaduTextEdit
	 */
	explicit KaduTextEdit(QWidget *parent = 0);
	virtual ~KaduTextEdit();

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Set preferred number of lines for this widgte.
	 * @param preferredLines preferred number of lines
	 *
	 * Size hint of this widget depends of number of preffered lines.
	 */
	void setPreferredLines(quint16 preferredLines);

	virtual QSize sizeHint() const;

};

/**
 * @}
 */

#endif // KADU_TEXT_EDIT_H
