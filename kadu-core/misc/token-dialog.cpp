/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include "misc/image-widget.h"
#include "debug.h"

#include "token-dialog.h"

TokenDialog::TokenDialog(QPixmap tokenImage, QDialog *parent)
	: QDialog(parent), tokenedit(0)
{
	kdebugf();
	QGridLayout *grid = new QGridLayout(this);

	QLabel *l_tokenimage = new QLabel(tr("Read this code ..."), this);
	ImageWidget *tokenimage = new ImageWidget(this);

	QLabel *l_tokenedit = new QLabel(tr("and type here"), this);
	tokenedit = new QLineEdit(this);

	QPushButton *b_ok = new QPushButton(tr("&OK"), this);
	connect(b_ok, SIGNAL(clicked()), this, SLOT(accept()));
	QPushButton *b_cancel = new QPushButton(tr("&Cancel"), this);
	connect(b_cancel, SIGNAL(clicked()), this, SLOT(reject()));

	grid->addWidget(l_tokenimage, 0, 0);
	grid->addWidget(tokenimage, 0, 1);
	grid->addWidget(l_tokenedit, 1, 0);
	grid->addWidget(tokenedit, 1, 1);
	grid->addWidget(b_ok, 2, 0);
	grid->addWidget(b_cancel, 2, 1);

	tokenimage->setImage(tokenImage);

	show();
	b_cancel->setDefault(false);
	b_ok->setDefault(true);
	kdebugf2();
}

void TokenDialog::getValue(QString &tokenValue)
{
	tokenValue = tokenedit->text();
}

QString TokenDialog::getValue()
{
	return tokenedit->text();
}
