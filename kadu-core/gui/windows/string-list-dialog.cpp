/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "string-list-dialog.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

StringListDialog::StringListDialog(QString message, QString okButtonTitle, QStringList stringList, QWidget *parent) :
		// using C++ initializers breaks Qt's lupdate
		QDialog(parent)
{
	setWindowRole("kadu-string-list-dialog");
	setWindowTitle(tr("Kadu"));
	setAttribute(Qt::WA_DeleteOnClose);

	auto layout = new QVBoxLayout{this};
	layout->setMargin(8);

	auto messageLabel = new QLabel{std::move(message)};
	messageLabel->setWordWrap(true);
	layout->addWidget(messageLabel);

	auto pluginListWidget = new QListWidget{};
	pluginListWidget->addItems(std::move(stringList));
	layout->addWidget(pluginListWidget);

	layout->addSpacing(16);

	auto buttonBox = new QDialogButtonBox{Qt::Horizontal, this};
	auto okButton = new QPushButton{qApp->style()->standardIcon(QStyle::SP_DialogOkButton), std::move(okButtonTitle), this};
	okButton->setDefault(true);
	buttonBox->addButton(okButton, QDialogButtonBox::AcceptRole);
	buttonBox->addButton(QDialogButtonBox::Cancel);
	layout->addWidget(buttonBox);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

StringListDialog::~StringListDialog()
{
}
