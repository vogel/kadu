/**************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include <QtGui/QCheckBox>

#include <QtGui/QLineEdit>

#include "configuration/contact-account-data-manager.h"
#include "contacts/contact-account-data.h"
#include "misc/misc.h"

#include "contact-options-configuration-widget.h"

ContactOptionsConfigurationWidget::ContactOptionsConfigurationWidget(QWidget *parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

ContactOptionsConfigurationWidget::~ContactOptionsConfigurationWidget()
{
}

void ContactOptionsConfigurationWidget::createGui()
{
	QGridLayout *layout = new QGridLayout(this);
	layout->setColumnStretch(3, 10);

	int row = 0;

	QLabel *tabLabel = new QLabel(tr("Options"), this);
	layout->addWidget(tabLabel, row++, 0, 1, 4);       

	QCheckBox *blockCheckBox = new QCheckBox(tr("Block contact"), this);
	layout->addWidget(blockCheckBox, row++, 1, 1, 2);      

	QCheckBox *offlineToCheckBox = new QCheckBox(tr("Always appear as offline to contact"), this);
	layout->addWidget(offlineToCheckBox, row++, 1, 1, 2);        

	QCheckBox *notifyCheckBox = new QCheckBox(tr("Notify when contact's status changes"), this);
	layout->addWidget(notifyCheckBox, row++, 1, 1, 2);   

	layout->setRowStretch(row, 100);                            
}
