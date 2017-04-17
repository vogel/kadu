/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "open-chat-with.h"
#include "open-chat-with.moc"

#include "open-chat-with-contact-list-runner.h"
#include "open-chat-with-runner-manager.h"

#include "activate.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-set.h"
#include "buddies/model/buddy-list-model.h"
#include "chat/chat-manager.h"
#include "chat/chat-storage.h"
#include "chat/type/chat-type-contact.h"
#include "configuration/configuration-api.h"
#include "configuration/configuration.h"
#include "contacts/contact.h"
#include "core/injected-factory.h"
#include "misc/paths-provider.h"
#include "model/model-chain.h"
#include "model/roles.h"
#include "talkable/model/talkable-proxy-model.h"
#include "widgets/chat-widget/chat-widget-manager.h"
#include "widgets/filtered-tree-view.h"
#include "widgets/line-edit-with-clear-button.h"

#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>
#include <QtQml/QQmlContext>
#include <QtQuick/QQuickItem>
#include <QtQuickWidgets/QQuickWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGraphicsObject>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyle>
#include <QtWidgets/QVBoxLayout>

OpenChatWith::OpenChatWith(QWidget *parent) : QWidget{parent, Qt::Window}, DesktopAwareObject{this}
{
}

OpenChatWith::~OpenChatWith()
{
    OpenChatWithRunnerManager::instance()->unregisterRunner(OpenChatRunner);

    delete OpenChatRunner;
    OpenChatRunner = 0;
}

void OpenChatWith::setBuddyManager(BuddyManager *buddyManager)
{
    m_buddyManager = buddyManager;
}

void OpenChatWith::setChatManager(ChatManager *chatManager)
{
    m_chatManager = chatManager;
}

void OpenChatWith::setChatStorage(ChatStorage *chatStorage)
{
    m_chatStorage = chatStorage;
}

void OpenChatWith::setChatWidgetManager(ChatWidgetManager *chatWidgetManager)
{
    m_chatWidgetManager = chatWidgetManager;
}

void OpenChatWith::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void OpenChatWith::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

void OpenChatWith::setPathsProvider(PathsProvider *pathsProvider)
{
    m_pathsProvider = pathsProvider;
}

void OpenChatWith::init()
{
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

    ContactID = m_injectedFactory->makeInjected<LineEditWithClearButton>(this);
    connect(ContactID, SIGNAL(textChanged(const QString &)), this, SLOT(inputChanged(const QString &)));
    idLayout->addWidget(ContactID);

    MainLayout->addWidget(idWidget);

    BuddiesView = new QQuickWidget();

    Chain = new ModelChain(this);
    ListModel = m_injectedFactory->makeInjected<BuddyListModel>(Chain);
    Chain->setBaseModel(ListModel);
    Chain->addProxyModel(m_injectedFactory->makeInjected<TalkableProxyModel>(Chain));

    QQmlContext *declarativeContext = BuddiesView->rootContext();
    declarativeContext->setContextProperty("buddies", Chain->lastModel());

    BuddiesView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    BuddiesView->setResizeMode(QQuickWidget::SizeRootObjectToView);
    BuddiesView->setSource(QUrl("file:///" + m_pathsProvider->dataPath() + "qml/openChatWith.qml"));

    if (BuddiesView->rootObject())
        connect(BuddiesView->rootObject(), SIGNAL(itemActivated(int)), this, SLOT(itemActivated(int)));

    MainLayout->addWidget(BuddiesView);

    QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

    QPushButton *okButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("&Ok"), this);
    buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
    QPushButton *cancelButton =
        new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("&Cancel"), this);
    buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

    connect(okButton, SIGNAL(clicked(bool)), this, SLOT(inputAccepted()));
    connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

    MainLayout->addSpacing(16);
    MainLayout->addWidget(buttons);

    OpenChatRunner = m_injectedFactory->makeInjected<OpenChatWithContactListRunner>();
    OpenChatWithRunnerManager::instance()->registerRunner(OpenChatRunner);

    inputChanged(QString());
}

void OpenChatWith::keyPressEvent(QKeyEvent *e)
{
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
    case Qt::Key_Down:
    case Qt::Key_Up:
    case Qt::Key_PageDown:
    case Qt::Key_PageUp:
        BuddiesView->setFocus();
        QCoreApplication::sendEvent(BuddiesView, e);
        focusQml();
        e->accept();
        return;
    }

    if (FilteredTreeView::shouldEventGoToFilter(e))
    {
        ContactID->setText(ContactID->text() + e->text());
        ContactID->setFocus(Qt::OtherFocusReason);
        e->accept();
        return;
    }

    QWidget::keyPressEvent(e);
}

void OpenChatWith::focusQml()
{
    auto rootObject = dynamic_cast<QObject *>(BuddiesView->rootObject());
    if (!rootObject)
        return;

    auto mainWidget = rootObject->findChild<QObject *>("mainWidget");
    if (!mainWidget)
        return;

    mainWidget->setProperty("focus", true);
}

void OpenChatWith::inputChanged(const QString &text)
{
    BuddyList matchingContacts = text.isEmpty() ? m_buddyManager->items().toList()
                                                : OpenChatWithRunnerManager::instance()->matchingContacts(text);

    ListModel->setBuddyList(matchingContacts);
}

void OpenChatWith::itemActivated(int index)
{
    QModelIndex modelIndex = Chain->lastModel()->index(index, 0);
    if (!modelIndex.isValid())
        return;

    Contact contact = modelIndex.data(ContactRole).value<Contact>();
    if (!contact)
        return;

    Chat chat = ChatTypeContact::findChat(m_chatManager, m_chatStorage, contact, ActionCreateAndAdd);
    if (!chat)
        return;

    m_chatWidgetManager->openChat(chat, OpenChatActivation::Activate);
    QTimer::singleShot(50, this, SLOT(close()));
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
        _activateWindow(m_configuration, this);
}
