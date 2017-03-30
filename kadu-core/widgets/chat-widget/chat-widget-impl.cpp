/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat-widget-impl.h"

#include "actions/action.h"
#include "actions/actions.h"
#include "actions/chat-widget/bold-action.h"
#include "actions/chat-widget/italic-action.h"
#include "actions/chat-widget/underline-action.h"
#include "chat/chat-state-service-repository.h"
#include "chat/type/chat-type-manager.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-set.h"
#include "contacts/model/chat-adapter.h"
#include "contacts/model/contact-list-model.h"
#include "core/injected-factory.h"
#include "gui/configuration/chat-configuration-holder.h"
#include "gui/hot-key.h"
#include "gui/web-view-highlighter.h"
#include "html/html-conversion.h"
#include "html/html-string.h"
#include "icons/icons-manager.h"
#include "message/message-manager.h"
#include "message/message-storage.h"
#include "message/sorted-messages.h"
#include "misc/misc.h"
#include "model/model-chain.h"
#include "protocols/protocol.h"
#include "protocols/services/chat-state.h"
#include "talkable/filter/name-talkable-filter.h"
#include "talkable/model/talkable-proxy-model.h"
#include "widgets/chat-edit-box-size-manager.h"
#include "widgets/chat-edit-box.h"
#include "widgets/chat-top-bar-container-widget.h"
#include "widgets/chat-widget/actions/chat-widget-actions.h"
#include "widgets/chat-widget/chat-widget-title.h"
#include "widgets/custom-input.h"
#include "widgets/filtered-tree-view.h"
#include "widgets/search-bar.h"
#include "widgets/talkable-tree-view.h"
#include "widgets/webkit-messages-view/webkit-messages-view-factory.h"
#include "widgets/webkit-messages-view/webkit-messages-view.h"
#include "windows/kadu-window-service.h"
#include "windows/kadu-window.h"
#include "windows/message-dialog.h"

#include <QtCore/QFileInfo>
#include <QtCore/QMimeData>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QShortcut>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>

ChatWidgetImpl::ChatWidgetImpl(Chat chat, QWidget *parent)
        : ChatWidget{parent}, CurrentChat{chat}, BuddiesWidget{0}, ProxyModel{0}, InputBox{0}, HorizontalSplitter{0},
          IsComposing{false}, CurrentContactActivity{ChatState::None}, SplittersInitialized{false}
{
}

ChatWidgetImpl::~ChatWidgetImpl()
{
    ComposingTimer.stop();

    kaduStoreGeometry();

    emit widgetDestroyed(CurrentChat);
    emit widgetDestroyed(this);

    CurrentChat.setOpen(false);
}

void ChatWidgetImpl::setActions(Actions *actions)
{
    m_actions = actions;
}

void ChatWidgetImpl::setBoldAction(BoldAction *boldAction)
{
    m_boldAction = boldAction;
}

void ChatWidgetImpl::setChatConfigurationHolder(ChatConfigurationHolder *chatConfigurationHolder)
{
    m_chatConfigurationHolder = chatConfigurationHolder;
}

void ChatWidgetImpl::setChatEditBoxSizeManager(ChatEditBoxSizeManager *chatEditBoxSizeManager)
{
    m_chatEditBoxSizeManager = chatEditBoxSizeManager;
}

void ChatWidgetImpl::setChatStateServiceRepository(ChatStateServiceRepository *chatStateServiceRepository)
{
    m_chatStateServiceRepository = chatStateServiceRepository;
}

void ChatWidgetImpl::setChatTypeManager(ChatTypeManager *chatTypeManager)
{
    m_chatTypeManager = chatTypeManager;
}

void ChatWidgetImpl::setChatWidgetActions(ChatWidgetActions *chatWidgetActions)
{
    m_chatWidgetActions = chatWidgetActions;
}

void ChatWidgetImpl::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void ChatWidgetImpl::setIconsManager(IconsManager *iconsManager)
{
    m_iconsManager = iconsManager;
}

void ChatWidgetImpl::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

void ChatWidgetImpl::setItalicAction(ItalicAction *italicAction)
{
    m_italicAction = italicAction;
}

void ChatWidgetImpl::setKaduWindowService(KaduWindowService *kaduWindowService)
{
    m_kaduWindowService = kaduWindowService;
}

void ChatWidgetImpl::setMessageManager(MessageManager *messageManager)
{
    m_messageManager = messageManager;
}

void ChatWidgetImpl::setMessageStorage(MessageStorage *messageStorage)
{
    m_messageStorage = messageStorage;
}

void ChatWidgetImpl::setUnderlineAction(UnderlineAction *underlineAction)
{
    m_underlineAction = underlineAction;
}

void ChatWidgetImpl::setWebkitMessagesViewFactory(WebkitMessagesViewFactory *webkitMessagesViewFactory)
{
    m_webkitMessagesViewFactory = webkitMessagesViewFactory;
}

void ChatWidgetImpl::init()
{
    Title = m_injectedFactory->makeInjected<ChatWidgetTitle>(this);

    setAcceptDrops(true);

    createGui();
    configurationUpdated();

    ComposingTimer.setInterval(2 * 1000);
    connect(&ComposingTimer, SIGNAL(timeout()), this, SLOT(checkComposing()));

    connect(edit(), SIGNAL(textChanged()), this, SLOT(updateComposing()));

    // icon for conference never changes
    if (CurrentChat.contacts().count() == 1)
    {
        auto chatStateService = m_chatStateServiceRepository->chatStateService(chat().chatAccount());
        connect(chatStateService, &ChatStateService::peerStateChanged, this, &ChatWidgetImpl::contactActivityChanged);
    }

    connect(CurrentChat, SIGNAL(updated()), this, SLOT(chatUpdated()));

    CurrentChat.setOpen(true);
}

void ChatWidgetImpl::createGui()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    TopBarContainer = m_injectedFactory->makeInjected<ChatTopBarContainerWidget>(CurrentChat);
    mainLayout->addWidget(TopBarContainer);

    VerticalSplitter = new QSplitter(Qt::Vertical, this);

    mainLayout->addWidget(VerticalSplitter);

    HorizontalSplitter = new QSplitter(Qt::Horizontal, this);
    HorizontalSplitter->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    HorizontalSplitter->setMinimumHeight(10);

    QFrame *frame = new QFrame(HorizontalSplitter);
    frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

    QVBoxLayout *frameLayout = new QVBoxLayout(frame);
    frameLayout->setMargin(0);
    frameLayout->setSpacing(0);

    MessagesView = m_webkitMessagesViewFactory->createWebkitMessagesView(CurrentChat, true, frame);

    frameLayout->addWidget(MessagesView.get());

    WebViewHighlighter *highligher = new WebViewHighlighter(MessagesView.get());

    SearchBar *messagesSearchBar = new SearchBar(frame);
    frameLayout->addWidget(messagesSearchBar);

    connect(messagesSearchBar, SIGNAL(searchPrevious(QString)), highligher, SLOT(selectPrevious(QString)));
    connect(messagesSearchBar, SIGNAL(searchNext(QString)), highligher, SLOT(selectNext(QString)));
    connect(messagesSearchBar, SIGNAL(clearSearch()), highligher, SLOT(clearSelect()));
    connect(highligher, SIGNAL(somethingFound(bool)), messagesSearchBar, SLOT(somethingFound(bool)));

    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_PageUp + Qt::SHIFT), this);
    connect(shortcut, SIGNAL(activated()), MessagesView.get(), SLOT(pageUp()));

    shortcut = new QShortcut(QKeySequence(Qt::Key_PageDown + Qt::SHIFT), this);
    connect(shortcut, SIGNAL(activated()), MessagesView.get(), SLOT(pageDown()));

    shortcut = new QShortcut(QKeySequence(Qt::Key_PageUp + Qt::ControlModifier), this);
    connect(shortcut, SIGNAL(activated()), MessagesView.get(), SLOT(pageUp()));

    shortcut = new QShortcut(QKeySequence(Qt::Key_PageDown + Qt::ControlModifier), this);
    connect(shortcut, SIGNAL(activated()), MessagesView.get(), SLOT(pageDown()));
    HorizontalSplitter->addWidget(frame);

    InputBox = m_injectedFactory->makeInjected<ChatEditBox>(CurrentChat, this);
    InputBox->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));
    InputBox->setMinimumHeight(10);

    messagesSearchBar->setSearchWidget(InputBox->inputBox());

    auto *chatType = m_chatTypeManager->chatType(CurrentChat.type());
    if (chatType && chatType->name() != "Contact")
        createContactsList();

    VerticalSplitter->addWidget(HorizontalSplitter);
    VerticalSplitter->setStretchFactor(0, 1);
    VerticalSplitter->addWidget(InputBox);
    VerticalSplitter->setStretchFactor(1, 0);

    connect(VerticalSplitter, SIGNAL(splitterMoved(int, int)), this, SLOT(verticalSplitterMoved(int, int)));
    connect(InputBox->inputBox(), SIGNAL(sendMessage()), this, SLOT(sendMessage()));
    connect(
        InputBox->inputBox(), SIGNAL(keyPressed(QKeyEvent *, CustomInput *, bool &)), this,
        SLOT(keyPressedSlot(QKeyEvent *, CustomInput *, bool &)));
}

void ChatWidgetImpl::createContactsList()
{
    QWidget *contactsListContainer = new QWidget(HorizontalSplitter);

    QVBoxLayout *layout = new QVBoxLayout(contactsListContainer);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    BuddiesWidget = m_injectedFactory->makeInjected<FilteredTreeView>(FilteredTreeView::FilterAtTop, this);
    BuddiesWidget->setMinimumSize(QSize(30, 30));

    TalkableTreeView *view = m_injectedFactory->makeInjected<TalkableTreeView>(BuddiesWidget);
    view->setItemsExpandable(false);

    auto chain = new ModelChain(this);
    auto contactListModel = m_injectedFactory->makeInjected<ContactListModel>(chain);
    new ChatAdapter(contactListModel, CurrentChat);
    chain->setBaseModel(contactListModel);
    ProxyModel = m_injectedFactory->makeInjected<TalkableProxyModel>(chain);

    NameTalkableFilter *nameFilter = new NameTalkableFilter(NameTalkableFilter::UndecidedMatching, ProxyModel);
    connect(BuddiesWidget, SIGNAL(filterChanged(QString)), nameFilter, SLOT(setName(QString)));

    ProxyModel->addFilter(nameFilter);
    chain->addProxyModel(ProxyModel);

    view->setChain(chain);
    view->setRootIsDecorated(false);
    view->setShowIdentityNameIfMany(false);
    view->setContextMenuEnabled(true);

    connect(
        view, SIGNAL(talkableActivated(Talkable)), m_kaduWindowService->kaduWindow(),
        SLOT(talkableActivatedSlot(Talkable)));

    BuddiesWidget->setView(view);

    QToolBar *toolBar = new QToolBar(contactsListContainer);
    toolBar->addAction(m_actions->createAction("editUserAction", InputBox->actionContext(), InputBox));
    toolBar->addAction(m_actions->createAction("leaveChatAction", InputBox->actionContext(), InputBox));

    layout->addWidget(toolBar);
    layout->addWidget(BuddiesWidget);

    QList<int> sizes;
    sizes.append(3);
    sizes.append(1);
    HorizontalSplitter->setSizes(sizes);
}

void ChatWidgetImpl::configurationUpdated()
{
    InputBox->inputBox()->setFont(m_chatConfigurationHolder->chatFont());
    QString style;
    QColor color = qApp->palette().text().color();
    if (m_chatConfigurationHolder->chatTextCustomColors())
    {
        style = QString("background-color:%1;").arg(m_chatConfigurationHolder->chatTextBgColor().name());
        color = m_chatConfigurationHolder->chatTextFontColor();
    }
    InputBox->inputBox()->viewport()->setStyleSheet(style);
    QPalette palette = InputBox->inputBox()->palette();
    palette.setBrush(QPalette::Text, color);
    InputBox->inputBox()->setPalette(palette);
}

void ChatWidgetImpl::chatUpdated()
{
    qApp->alert(window());
}

bool ChatWidgetImpl::keyPressEventHandled(QKeyEvent *e)
{
    if (e->matches(QKeySequence::Copy) && !MessagesView->selectedText().isEmpty())
    {
        // Do not use triggerPageAction(), see bug #2345.
        MessagesView->pageAction(QWebPage::Copy)->trigger();
        return true;
    }

    if (HotKey::shortCut(m_configuration, e, "ShortCuts", "chat_clear"))
    {
        clearChatWindow();
        return true;
    }

    if (HotKey::shortCut(m_configuration, e, "ShortCuts", "chat_close"))
    {
        emit closeRequested(this);
        return true;
    }

    if (HotKey::shortCut(m_configuration, e, "ShortCuts", "kadu_searchuser"))
    {
        m_actions->createAction("lookupUserInfoAction", InputBox->actionContext(), InputBox)
            ->activate(QAction::Trigger);
        return true;
    }

    if (HotKey::shortCut(m_configuration, e, "ShortCuts", "kadu_openchatwith"))
    {
        m_actions->createAction("openChatWithAction", InputBox->actionContext(), InputBox)->activate(QAction::Trigger);
        return true;
    }

    return false;
}

void ChatWidgetImpl::keyPressEvent(QKeyEvent *e)
{
    if (keyPressEventHandled(e))
        e->accept();
    else
        QWidget::keyPressEvent(e);
}

void ChatWidgetImpl::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    if (m_chatEditBoxSizeManager->initialized())
        commonHeightChanged(m_chatEditBoxSizeManager->commonHeight());
}

ChatWidgetTitle *ChatWidgetImpl::title() const
{
    return Title;
}

void ChatWidgetImpl::addMessages(const SortedMessages &messages)
{
    if (messages.empty())
        return;

    auto unread = std::any_of(begin(messages), end(messages), [](const Message &message) {
        return message.type() == MessageTypeReceived || message.type() == MessageTypeSystem;
    });

    MessagesView->setForcePruneDisabled(true);
    MessagesView->add(messages);
    if (unread)
        LastReceivedMessageTime = QDateTime::currentDateTime();
}

void ChatWidgetImpl::addMessage(const Message &message)
{
    MessagesView->add(message);

    if (message.type() != MessageTypeReceived && message.type() != MessageTypeSystem)
        return;

    LastReceivedMessageTime = QDateTime::currentDateTime();
    emit messageReceived(this);
}

SortedMessages ChatWidgetImpl::messages() const
{
    return MessagesView->messages();
}

void ChatWidgetImpl::appendSystemMessage(NormalizedHtmlString htmlContent)
{
    Message message = m_messageStorage->create();
    message.setMessageChat(CurrentChat);
    message.setType(MessageTypeSystem);
    message.setContent(htmlContent);
    message.setReceiveDate(QDateTime::currentDateTime());
    message.setSendDate(QDateTime::currentDateTime());

    MessagesView->add(message);
}

void ChatWidgetImpl::resetEditBox()
{
    InputBox->inputBox()->clear();

    Action *action;
    action = m_boldAction->action(InputBox->actionContext());
    if (action)
        InputBox->inputBox()->setFontWeight(action->isChecked() ? QFont::Bold : QFont::Normal);

    action = m_italicAction->action(InputBox->actionContext());
    if (action)
        InputBox->inputBox()->setFontItalic(action->isChecked());

    action = m_underlineAction->action(InputBox->actionContext());
    if (action)
        InputBox->inputBox()->setFontUnderline(action->isChecked());
}

void ChatWidgetImpl::clearChatWindow()
{
    MessageDialog *dialog = MessageDialog::create(
        m_iconsManager->iconByPath(KaduIcon("dialog-question")), tr("Kadu"),
        tr("Chat window will be cleared. Continue?"));
    dialog->addButton(QMessageBox::Yes, tr("Clear chat window"));
    dialog->addButton(QMessageBox::No, tr("Cancel"));

    if (!m_configuration->deprecatedApi()->readBoolEntry("Chat", "ConfirmChatClear") || dialog->ask())
    {
        MessagesView->clearMessages();
        MessagesView->setForcePruneDisabled(false);
        activateWindow();
    }
}

/* sends the message typed */
void ChatWidgetImpl::sendMessage()
{
    if (InputBox->inputBox()->toPlainText().isEmpty())
        return;

    emit messageSendRequested(this);

    if (!CurrentChat.isConnected())
    {
        MessageDialog::show(
            m_iconsManager->iconByPath(KaduIcon("dialog-error")), tr("Kadu"),
            QString("%1\n%2%3")
                .arg(tr("Cannot send message while being offline."))
                .arg(tr("Account:"))
                .arg(chat().chatAccount().id()),
            QMessageBox::Ok, this);
        return;
    }

    if (!m_messageManager->sendMessage(CurrentChat, InputBox->inputBox()->htmlMessage()))
        return;

    resetEditBox();

    // We sent the message and reseted the edit box, so composing of that message is done.
    // Note that if ComposingTimer is not active, it means that we already reported
    // composing had stopped.
    if (ComposingTimer.isActive())
        composingStopped();

    emit messageSent(this);
}

void ChatWidgetImpl::colorSelectorAboutToClose()
{
}

CustomInput *ChatWidgetImpl::edit() const
{
    return InputBox ? InputBox->inputBox() : 0;
}

TalkableProxyModel *ChatWidgetImpl::talkableProxyModel() const
{
    return ProxyModel;
}

int ChatWidgetImpl::countMessages() const
{
    return MessagesView ? MessagesView->countMessages() : 0;
}

bool ChatWidgetImpl::decodeLocalFiles(QDropEvent *event, QStringList &files)
{
    if (!event->mimeData()->hasUrls() || event->source() == MessagesView.get())
        return false;

    QList<QUrl> urls = event->mimeData()->urls();

    for (auto const &url : urls)
    {
        QString file = url.toLocalFile();
        if (!file.isEmpty())
        {
            // is needed to check if file refer to local file?
            QFileInfo fileInfo(file);
            if (fileInfo.exists())
                files.append(file);
        }
    }
    return !files.isEmpty();
}

void ChatWidgetImpl::dragEnterEvent(QDragEnterEvent *e)
{
    QStringList files;

    if (decodeLocalFiles(e, files))
        e->acceptProposedAction();
}

void ChatWidgetImpl::dragMoveEvent(QDragMoveEvent *e)
{
    QStringList files;

    if (decodeLocalFiles(e, files))
        e->acceptProposedAction();
}

void ChatWidgetImpl::dropEvent(QDropEvent *e)
{
    QStringList files;

    if (decodeLocalFiles(e, files))
    {
        e->acceptProposedAction();

        QStringList::const_iterator i = files.constBegin();
        QStringList::const_iterator end = files.constEnd();

        for (; i != end; ++i)
            emit fileDropped(CurrentChat, *i);
    }
}

Protocol *ChatWidgetImpl::currentProtocol() const
{
    return CurrentChat.chatAccount().protocolHandler();
}

void ChatWidgetImpl::requestClose()
{
    emit closeRequested(this);
}

void ChatWidgetImpl::verticalSplitterMoved(int pos, int index)
{
    Q_UNUSED(pos)
    Q_UNUSED(index)

    if (SplittersInitialized)
        m_chatEditBoxSizeManager->setCommonHeight(VerticalSplitter->sizes().at(1));
}

void ChatWidgetImpl::kaduRestoreGeometry()
{
    if (!HorizontalSplitter)
        return;

    if (!chat())
        return;

    QList<int> horizSizes =
        stringToIntList(chat().property("chat-geometry:WidgetHorizontalSizes", QString()).toString());
    if (!horizSizes.isEmpty())
        HorizontalSplitter->setSizes(horizSizes);
}

void ChatWidgetImpl::kaduStoreGeometry()
{
    if (!HorizontalSplitter)
        return;

    if (!chat())
        return;

    chat().addProperty(
        "chat-geometry:WidgetHorizontalSizes", intListToString(HorizontalSplitter->sizes()),
        CustomProperties::Storable);
}

void ChatWidgetImpl::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    if (!SplittersInitialized)
        QMetaObject::invokeMethod(this, "setUpVerticalSizes", Qt::QueuedConnection);
}

void ChatWidgetImpl::setUpVerticalSizes()
{
    // now we can accept this signal
    connect(m_chatEditBoxSizeManager, SIGNAL(commonHeightChanged(int)), this, SLOT(commonHeightChanged(int)));

    // already set up by other window, so we use this window setting
    if (m_chatEditBoxSizeManager->initialized())
    {
        commonHeightChanged(m_chatEditBoxSizeManager->commonHeight());
        SplittersInitialized = true;
        return;
    }

    QList<int> vertSizes;
    int h = height();
    vertSizes.append(h / 3 * 2 + h % 3);
    vertSizes.append(h / 3);

    VerticalSplitter->setSizes(vertSizes);
    SplittersInitialized = true;
    m_chatEditBoxSizeManager->setCommonHeight(vertSizes.at(1));
}

void ChatWidgetImpl::commonHeightChanged(int commonHeight)
{
    QList<int> sizes = VerticalSplitter->sizes();

    int sum = 0;
    if (2 == sizes.count())
    {
        if (sizes.at(1) == commonHeight)
            return;
        sum = sizes.at(0) + sizes.at(1);
    }
    else
        sum = height();

    if (sum < commonHeight)
        commonHeight = sum / 3;

    sizes.clear();
    sizes.append(sum - commonHeight);
    sizes.append(commonHeight);
    VerticalSplitter->setSizes(sizes);
}

void ChatWidgetImpl::composingStopped()
{
    ComposingTimer.stop();
    IsComposing = false;

    auto chatStateService = m_chatStateServiceRepository->chatStateService(chat().chatAccount());
    if (chatStateService && chat().contacts().toContact())
        chatStateService->sendState(chat().contacts().toContact(), ChatState::Paused);
}

void ChatWidgetImpl::checkComposing()
{
    if (!IsComposing)
        composingStopped();
    else
        // Reset IsComposing to false, so if updateComposing() method doesn't set it to true
        // before ComposingTimer hits this method again, we will call composingStopped().
        IsComposing = false;
}

void ChatWidgetImpl::updateComposing()
{
    auto chatStateService = m_chatStateServiceRepository->chatStateService(chat().chatAccount());
    if (!chatStateService)
        return;

    if (!ComposingTimer.isActive())
    {
        // If the text was deleted either by sending a message or explicitly by the user,
        // let's not report it as composing.
        if (edit()->toPlainText().isEmpty())
            return;

        if (chat().contacts().toContact())
            chatStateService->sendState(chat().contacts().toContact(), ChatState::Composing);

        ComposingTimer.start();
    }
    IsComposing = true;
}

ChatState ChatWidgetImpl::chatState() const
{
    return CurrentContactActivity;
}

void ChatWidgetImpl::contactActivityChanged(const Contact &contact, ChatState state)
{
    if (CurrentContactActivity == state)
        return;

    if (!CurrentChat.contacts().contains(contact))
        return;

    CurrentContactActivity = state;
    emit chatStateChanged(CurrentContactActivity);

    if (m_chatConfigurationHolder->contactStateChats())
        MessagesView->contactActivityChanged(contact, state);

    if (CurrentContactActivity == ChatState::Gone)
    {
        auto msg = QString{"[ " + tr("%1 ended the conversation").arg(contact.ownerBuddy().display()) + " ]"};
        Message message = m_messageStorage->create();
        message.setMessageChat(CurrentChat);
        message.setType(MessageTypeSystem);
        message.setMessageSender(contact);
        message.setContent(normalizeHtml(plainToHtml(msg)));
        message.setSendDate(QDateTime::currentDateTime());
        message.setReceiveDate(QDateTime::currentDateTime());

        MessagesView->add(message);
    }
}

void ChatWidgetImpl::keyPressedSlot(QKeyEvent *e, CustomInput *input, bool &handled)
{
    Q_UNUSED(input)

    if (e->key() == Qt::Key_Home && e->modifiers() == Qt::AltModifier)
        MessagesView->scrollToTop();
    else if (e->key() == Qt::Key_End && e->modifiers() == Qt::AltModifier)
        MessagesView->forceScrollToBottom();

    if (handled)
        return;

    handled = keyPressEventHandled(e);
}

#include "moc_chat-widget-impl.cpp"
