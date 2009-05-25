 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QTabWidget>

#include "gadu-edit-account-widget.h"

GaduEditAccountWidget::GaduEditAccountWidget(Account *account, QWidget *parent) :
		AccountEditWidget(account, parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	createGui();
}


GaduEditAccountWidget::~GaduEditAccountWidget()
{
}

void GaduEditAccountWidget::createGui()
{
	QTabWidget *tabWidget = new QTabWidget(this);
	tabWidget->addTab(new QWidget(), tr("General"));
	tabWidget->addTab(new QWidget(), tr("Personal data"));
	tabWidget->addTab(new QWidget(), tr("Buddies"));
	tabWidget->addTab(new QWidget(), tr("Connection"));
	tabWidget->addTab(new QWidget(), tr("Functions"));
}
