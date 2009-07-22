/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QHBoxLayout>
#include <QtGui/QCheckBox>

#include "notify-group-box.h"

NotifyGroupBox::NotifyGroupBox(Notifier *notificator, const QString &caption, QWidget *parent) :
		QWidget(parent), Notificator(notificator)
{
	new QHBoxLayout(this);

	NotifierCheckBox = new QCheckBox(caption);
	NotifierCheckBox->setIcon(notificator->icon());
	layout()->addWidget(NotifierCheckBox);
	connect(NotifierCheckBox, SIGNAL(clicked(bool)), this, SLOT(toggledSlot(bool)));
}

void NotifyGroupBox::setChecked(bool checked)
{
	NotifierCheckBox->setChecked(checked);
}

void NotifyGroupBox::addWidget(QWidget *widget)
{
	layout()->addWidget(widget);
	widget->setEnabled(NotifierCheckBox->isChecked());
	connect(NotifierCheckBox, SIGNAL(toggled(bool)), widget, SLOT(setEnabled(bool)));
}

void NotifyGroupBox::toggledSlot(bool toggle)
{
	emit toggled(Notificator, toggle);
}
