/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QCheckBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "icons-manager.h"

#include "subscription-window.h"

void SubscriptionWindow::getSubscription(const QString &uid, QObject *receiver, const char *slot)
{
	SubscriptionWindow *window = new SubscriptionWindow(uid);
	connect(window, SIGNAL(requestAccepted(QString &)), receiver, slot);

	window->exec();
}

SubscriptionWindow::SubscriptionWindow(const QString& uid, QWidget *parent) :
		QDialog(parent), Uid(uid)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("New Contact Request"));
	
	QGridLayout *layout = new QGridLayout(this);

	QLabel *messageLabel = new QLabel(tr("%1 wants to be able to chat with you.").arg(Uid), this);
	QLabel *visibleNameLabel = new QLabel(tr("Visible Name") + ":", this);
	QLineEdit *visibleName = new QLineEdit(this);
	QLabel *groupLabel = new QLabel(tr("Add in Group") + ":", this);
	QLineEdit *group = new QLineEdit(this);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *okButton = new QPushButton(IconsManager::instance()->loadIcon("OkWindowButton"), tr("Allow"), this);
	okButton->setDefault(true);
	buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
	QPushButton *cancelButton = new QPushButton(IconsManager::instance()->loadIcon("CloseWindowButton"), tr("Ignore"), this);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(okButton, SIGNAL(clicked(bool)), this, SLOT(accepted()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	layout->addWidget(messageLabel, 0, 0, 1, 2);
	layout->addWidget(visibleNameLabel, 1, 0, 1, 1);
	layout->addWidget(visibleName, 1, 1, 1, 1);
	layout->addWidget(groupLabel, 2, 0, 1, 1);
	layout->addWidget(group, 2, 1, 1, 1);
	layout->addWidget(buttons, 3, 1, 1, 3);
}

SubscriptionWindow::~SubscriptionWindow()
{
}

void SubscriptionWindow::accepted()
{
	emit requestAccepted(Uid);
	close();
}
