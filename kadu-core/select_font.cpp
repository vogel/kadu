/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QFontDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>

#include "select_font.h"

SelectFont::SelectFont(QWidget *parent, const char *name)
	: QWidget(parent, name)
{
	QHBoxLayout *layout = new QHBoxLayout;
	layout->setSpacing(5);

	fontEdit = new QLineEdit(this);
	fontEdit->setReadOnly(true);

	QPushButton *button = new QPushButton(tr("Select"), this);
	connect(button, SIGNAL(clicked()), this, SLOT(onClick()));

	layout->addWidget(fontEdit);
	layout->addWidget(button);

	setLayout(layout);
}

void SelectFont::setFont(const QFont &font)
{
	currentFont = font;
	fontEdit->setText(QString("%1 %2").arg(currentFont.family(), QString::number(currentFont.pointSize())));

	emit fontChanged(currentFont);
}

const QFont & SelectFont::font() const
{
	return currentFont;
}

void SelectFont::onClick()
{
	bool ok;
	QFont f = QFontDialog::getFont(&ok, currentFont, dynamic_cast<QWidget *>(parent()));

	if (ok)
		setFont(f);
}
