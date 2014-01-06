/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

#include "title-widget.h"


TitleWidget::TitleWidget(QWidget *parent)
		: QWidget(parent)
{
	QFrame *titleFrame = new QFrame(this);
	titleFrame->setFrameShape(QFrame::StyledPanel);
	titleFrame->setFrameShadow(QFrame::Plain);
	titleFrame->setBackgroundRole(QPalette::Base);

	QGridLayout *headerLayout = new QGridLayout(titleFrame);
	headerLayout->setColumnStretch(0, 1);
	headerLayout->setMargin(6);

	textLabel = new QLabel(titleFrame);
	textLabel->setVisible(false);
	textLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse);
	QFont font = textLabel->font();
	font.setBold(true);
	textLabel->setFont(font);

	QLabel *imageLabel = new QLabel(titleFrame);
	imageLabel->setVisible(false);

	headerLayout->addWidget(textLabel, 0, 0);
	headerLayout->addWidget(imageLabel, 0, 1, 1, 2);

	commentLabel = new QLabel(titleFrame);
	commentLabel->setVisible(false);
	commentLabel->setOpenExternalLinks(true);
	commentLabel->setWordWrap(true);
	commentLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse);
	headerLayout->addWidget(commentLabel, 1, 0);

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(titleFrame);
	mainLayout->setMargin(0);
	setLayout(mainLayout);
}

TitleWidget::~TitleWidget()
{
}

void TitleWidget::setText(const QString &text, Qt::Alignment alignment)
{
	textLabel->setVisible(!text.isNull());
	textLabel->setText(text);
	textLabel->setAlignment(alignment);
	show();
}

void TitleWidget::setComment(const QString &comment)
{
	commentLabel->setVisible(!comment.isNull());
	commentLabel->setText(comment);
	show();
}

#include "moc_title-widget.cpp"
