/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QListWidget>
#include <QtCore/QProcess>
#include <QtGui/QPushButton>

#include "gui/widgets/image-widget.h"

#include "sms-image-dialog.h"

SmsImageDialog::SmsImageDialog(QWidget* parent, const QByteArray& image)
	: QDialog(parent), code_edit(0)
{
	ImageWidget *image_widget = new ImageWidget(image, this);
	QLabel* label = new QLabel(tr("Enter text from the picture:"), this);
	code_edit = new QLineEdit(this);

	QGridLayout *grid = new QGridLayout(this);
	grid->addWidget(image_widget, 0, 0, 1, 2);
	grid->addWidget(label, 1, 0, 1, 1);
	grid->addWidget(code_edit, 1, 1, 1, 1);
	
	QWidget *buttonsWidget = new QWidget(this);
	QHBoxLayout *buttonsLayout = new QHBoxLayout(buttonsWidget);

	QPushButton *okButton = new QPushButton(tr("Ok"), buttonsWidget);
	QPushButton *cancelButton = new QPushButton(tr("Cancel"), buttonsWidget);

	buttonsLayout->setSpacing(10);
	buttonsLayout->addWidget(okButton);
	buttonsLayout->addWidget(cancelButton);

	grid->addWidget(buttonsWidget, 2, 0, 1, 2);

	connect(code_edit, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));
	connect(okButton, SIGNAL(clicked()), this, SLOT(onReturnPressed()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

void SmsImageDialog::reject()
{
	emit codeEntered(QString::null);
	QDialog::reject();
}

void SmsImageDialog::onReturnPressed()
{
	accept();
	emit codeEntered(code_edit->text());
}
