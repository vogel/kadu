/*
 * %kadu copyright begin%
 * Copyright 2008, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef PROGRESS_LABEL_H
#define PROGRESS_LABEL_H

#include <QtGui/QWidget>

#include "gui/widgets/progress-icon.h"

class ProgressLabel : public QWidget
{
	Q_OBJECT

	ProgressIcon *Icon;
	QLabel *Label;

	void createGui();

public:
	explicit ProgressLabel(const QString &text, QWidget *parent = 0);
	virtual ~ProgressLabel();

	void setState(ProgressIcon::ProgressState state, const QString &text);
	void setText(const QString &text);

};

#endif // PROGRESS_LABEL_H
