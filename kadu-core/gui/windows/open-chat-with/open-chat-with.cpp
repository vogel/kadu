/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/QDeclarativeView>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGraphicsObject>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>

#include "buddies/buddy-manager.h"
#include "buddies/buddy-set.h"
#include "buddies/model/buddy-list-model.h"
#include "chat/chat-manager.h"
#include "chat/type/chat-type-contact.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget-manager.h"
#include "gui/widgets/filtered-tree-view.h"
#include "gui/widgets/line-edit-with-clear-button.h"
#include "misc/kadu-paths.h"
#include "model/model-chain.h"
#include "model/roles.h"
#include "talkable/model/talkable-proxy-model.h"

#include "activate.h"
#include "debug.h"

#include "open-chat-with-contact-list-runner.h"
#include "open-chat-with-runner-manager.h"

#include "open-chat-with.h"

OpenChatWith *OpenChatWith::Instance = 0;

OpenChatWith * OpenChatWith::instance()
{
	if (!Instance)
		Instance = new OpenChatWith();

	return Instance;
}

OpenChatWith::OpenChatWith() :
		QWidget(0, Qt::Window), DesktopAwareObject(this)
{
	kdebugf();

	setWindowRole("kadu-open-chat-with");

	setWindowTitle(tr("Open chat with..."));
	setAttribute(Qt::WA_DeleteOnClose);

	QRect availableGeometry = qApp->desktop()->availableGeometry(this);
	int width = static_cast<int>(0.25f * availableGeometry.width());
	int height = static_cast<int>(0.6f * availableGeometry.height());
	setGeometry(availableGeometry.center().x() - width / 2, availableGeometry.center().y() - height / 2, width, height);

	MainLayout = new QVBoxLayout(this);

	QWidget *idWidget = new QWidget(this);

	QHBoxLayout *idLayout = new QHBoxLayout(idWidget);
	idLayout->setMargin(0);
	idLayout->addWidget(new QLabel(tr("User name:"), idWidget));

	ContactID = new LineEditWithClearButton(this);
	ContactID->installEventFilter(this);
	connect(ContactID, SIGNAL(textChanged(const QString &)), this, SLOT(inputChanged(const QString &)));
	idLayout->addWidget(ContactID);

	MainLayout->addWidget(idWidget);

	BuddiesView = new QDeclarativeView();

	Chain = new ModelChain(this);
	ListModel = new BuddyListModel(Chain);
	Chain->setBaseModel(ListModel);
	Chain->addProxyModel(new TalkableProxyModel(Chain));

	QDeclarativeContext *declarativeContext = BuddiesView->rootContext();
	declarativeContext->setContextProperty("buddies", Chain->lastModel());

	BuddiesView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	BuddiesView->setResizeMode(QDeclarativeView::SizeRootObjectToView);
	BuddiesView->setSource(QUrl("file:///" + KaduPaths::instance()->dataPath() + "qml/openChatWith.qml"));

	if (BuddiesView->rootObject())
		connect(BuddiesView->rootObject(), SIGNAL(itemActivated(int)), this, SLOT(itemActivated(int)));

	MainLayout->addWidget(BuddiesView);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *okButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("&Ok"), this);
	buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("&Cancel"), this);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(okButton, SIGNAL(clicked(bool)), this, SLOT(inputAccepted()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	MainLayout->addSpacing(16);
	MainLayout->addWidget(buttons);

	OpenChatRunner = new OpenChatWithContactListRunner();
	OpenChatWithRunnerManager::instance()->registerRunner(OpenChatRunner);

	inputChanged(QString());
}

OpenChatWith::~OpenChatWith()
{
	OpenChatWithRunnerManager::instance()->unregisterRunner(OpenChatRunner);
	Instance = 0;

	delete OpenChatRunner;
	OpenChatRunner = 0;
}

bool OpenChatWith::eventFilter(QObject *obj, QEvent *e)
{
	if (obj == ContactID && e->type() == QEvent::KeyPress)
	{
		if (static_cast<QKeyEvent *>(e)->modifiers() == Qt::NoModifier)
		{
			int key = static_cast<QKeyEvent *>(e)->key();
			if (key == Qt::Key_Down ||
					key == Qt::Key_Up ||
					key == Qt::Key_PageDown ||
					key == Qt::Key_PageUp ||
					key == Qt::Key_Left ||
					key == Qt::Key_Right)
			{
				QCoreApplication::sendEvent(BuddiesView, e);
				return true;
			}
		}
	}

	return false;
}

void OpenChatWith::keyPressEvent(QKeyEvent *e)
{
	kdebugf();

	switch (e->key())
	{
		case Qt::Key_Enter:
		case Qt::Key_Return:
			e->accept();
			inputAccepted();
			return;
			break;
		case Qt::Key_Escape:
			e->accept();
			close();
			return;
			break;
	}

	if (FilteredTreeView::shouldEventGoToFilter(e))
	{
		ContactID->setText(e->text());
		ContactID->setFocus(Qt::OtherFocusReason);
		e->accept();
		return;
	}

	QWidget::keyPressEvent(e);

	kdebugf2();
}

void OpenChatWith::inputChanged(const QString &text)
{
	kdebugf();

	BuddyList matchingContacts = text.isEmpty()
			? BuddyManager::instance()->items().toList()
			: OpenChatWithRunnerManager::instance()->matchingContacts(text);

	ListModel->setBuddyList(matchingContacts);
	if (BuddiesView->rootObject())
		if (BuddiesView->rootObject()->property("currentIndex").toInt() < 0)
			BuddiesView->rootObject()->setProperty("currentIndex", 0);
}

void OpenChatWith::itemActivated(int index)
{
	QModelIndex modelIndex = Chain->lastModel()->index(index, 0);
	if (!modelIndex.isValid())
		return;

	Contact contact = modelIndex.data(ContactRole).value<Contact>();
	if (!contact)
		return;

	Chat chat = ChatTypeContact::findChat(contact, ActionCreateAndAdd);
	if (!chat)
		return;

	Core::instance()->chatWidgetManager()->openChat(chat, OpenChatActivation::Activate);
	deleteLater();
}

void OpenChatWith::inputAccepted()
{
	if (BuddiesView->rootObject())
		itemActivated(BuddiesView->rootObject()->property("currentIndex").toInt());
}

void OpenChatWith::show()
{
  	if (!isVisible())
		QWidget::show();
	else
		_activateWindow(this);
}

#include "moc_open-chat-with.cpp"
