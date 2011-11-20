/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QHBoxLayout>

#include "icons/icons-manager.h"

#include "progress-label.h"

ProgressLabel::ProgressLabel(const QString &text, QWidget *parent) :
		QWidget(parent)
{
	createGui();

	setText(text);
}

ProgressLabel::~ProgressLabel()
{
}

void ProgressLabel::createGui()
{
	QHBoxLayout *layout = new QHBoxLayout(this);

	Icon = new ProgressIcon(this);
	Label = new QLabel(this);
	Label->setWordWrap(true);

	layout->addWidget(Icon, 0, Qt::AlignCenter);
	layout->addWidget(Label, 100, Qt::AlignVCenter | Qt::AlignLeft);

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
}

void ProgressLabel::setState(ProgressIcon::ProgressState state, const QString &text)
{
	Icon->setState(state);
	setText(text);
}

void ProgressLabel::setText(const QString &text)
{
	Label->setText(text);
}
