/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QStringList>
#include <QtCore/QAbstractItemModel>
#include <QtGui/QTreeWidget>
#include <QtGui/QLayout>
#include <QtGui/QPushButton>
#include <QtNetwork/QHostAddress>

#include "infos_dialog.h"

#include "misc.h"
#include "modules.h"
#include "userlist.h"
#include "debug.h"


InfosDialog::InfosDialog(const LastSeen &lastSeen, QWidget *parent, const char *name, bool modal, Qt::WindowFlags f)
:QDialog(parent, name, modal, f)
{
	kdebugf();
//	this->resize(800, this->height());
	this->setCaption(tr("Information about contacts"));

	QVBoxLayout *mainLayout = new QVBoxLayout(this, 10, -1, "infos_dialog_main_layout");
	QTreeWidget *listView = new QTreeWidget(this);
	listView->setColumnCount(7);
	listView->setSelectionMode(QAbstractItemView::NoSelection);
	mainLayout->addWidget(listView);
	QHBoxLayout *buttonsLayout = new QHBoxLayout(mainLayout, -1, "infos_dialog_buttons_layout");
	listView->setAllColumnsShowFocus(true);
	QStringList labels;
	labels << tr("Contact") << tr("UIN") << tr("Nick") << tr("IP") << tr("Domain name") << tr("Description") << tr("Last time seen on");
	listView->setHeaderLabels(labels);
	QPushButton *closeButton = new QPushButton(tr("&Close"), this, "infos close button");
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(closeButton);
	
	for(UserList::const_iterator it = userlist->begin(); it != userlist->end(); ++it)
	{
		if((*it).usesProtocol("Gadu"))
		{
			QString desc, ip;
			if((*it).status("Gadu").hasDescription())
				desc = (*it).status("Gadu").description();
			desc.replace('\n', ' ');
			if((*it).hasIP("Gadu"))
				ip = (*it).IP("Gadu").toString();

			QStringList labels;
			labels << (*it).altNick() << (*it).ID("Gadu") << (*it).nickName() << ip <<
					(*it).DNSName("Gadu") << desc << lastSeen[(*it).ID("Gadu")];

			listView->addTopLevelItem(new QTreeWidgetItem(labels));
		}
	}
	listView->sortItems(0, Qt::AscendingOrder);
	
	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
	
	loadWindowGeometry(this, "LastSeen", "LastSeenWidgetGeometry", 0, 0, 800, 300);

	kdebugf2();
}

InfosDialog::~InfosDialog()
{
	kdebugf();

	saveWindowGeometry(this, "LastSeen", "LastSeenWidgetGeometry");

	kdebugf2();
}
