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

#include <QtCore/QFileInfo>
#include <QtCore/QMimeData>
#include <QtGui/QIcon>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QShortcut>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "buddies/buddy-set.h"
#include "buddies/buddy.h"
#include "buddies/model/buddy-list-model.h"
#include "chat-style/engine/chat-style-renderer-factory-provider.h"
#include "chat/chat-manager.h"
#include "chat/type/chat-type-manager.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-set.h"
#include "contacts/contact.h"
#include "contacts/model/chat-adapter.h"
#include "contacts/model/contact-data-extractor.h"
#include "contacts/model/contact-list-model.h"
#include "core/application.h"
#include "core/core.h"
#include "formatted-string/formatted-string-factory.h"
#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "gui/configuration/chat-configuration-holder.h"
#include "gui/hot-key.h"
#include "gui/web-view-highlighter.h"
#include "gui/widgets/chat-edit-box-size-manager.h"
#include "gui/widgets/chat-edit-box.h"
#include "gui/widgets/chat-top-bar-container-widget.h"
#include "gui/widgets/chat-widget/chat-widget-actions.h"
#include "gui/widgets/chat-widget/chat-widget-title.h"
#include "gui/widgets/color-selector.h"
#include "gui/widgets/custom-input.h"
#include "gui/widgets/filtered-tree-view.h"
#include "gui/widgets/search-bar.h"
#include "gui/widgets/talkable-tree-view.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view-factory.h"
#include "gui/widgets/webkit-messages-view/webkit-messages-view.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "message/message-manager.h"
#include "message/sorted-messages.h"
#include "misc/misc.h"
#include "model/model-chain.h"
#include "parser/parser.h"
#include "protocols/protocol.h"
#include "protocols/services/chat-state.h"
#include "talkable/filter/name-talkable-filter.h"
#include "talkable/model/talkable-proxy-model.h"
#include "activate.h"
#include "debug.h"

#include "chat-widget.h"

ChatWidget::ChatWidget(Chat chat, QWidget *parent) :
		QWidget{parent},
		CurrentChat{chat},
		BuddiesWidget{0},
		ProxyModel{0},
		InputBox{0},
		HorizontalSplitter{0},
		IsComposing{false},
		CurrentContactActivity{ChatState::None},
		SplittersInitialized{false}
{
	kdebugf();

	Title = new ChatWidgetTitle{this};

	setAcceptDrops(true);

	createGui();
	configurationUpdated();

	ComposingTimer.setInterval(2 * 1000);
	connect(&ComposingTimer, SIGNAL(timeout()), this, SLOT(checkComposing()));

	connect(edit(), SIGNAL(textChanged()), this, SLOT(updateComposing()));

	// icon for conference never changes
	if (CurrentChat.contacts().count() == 1)
	{
		if (currentProtocol() && currentProtocol()->chatStateService())
			connect(currentProtocol()->chatStateService(), SIGNAL(peerStateChanged(const Contact &, ChatState)),
					this, SLOT(contactActivityChanged(const Contact &, ChatState)));
	}

	connect(CurrentChat, SIGNAL(updated()), this, SLOT(chatUpdated()));

	CurrentChat.setOpen(true);

	kdebugf2();
}

ChatWidget::~ChatWidget()
{
	kdebugf();
	ComposingTimer.stop();

	kaduStoreGeometry();

	emit widgetDestroyed(CurrentChat);
	emit widgetDestroyed(this);

	if (currentProtocol() && currentProtocol()->chatStateService() && chat().contacts().toContact())
		currentProtocol()->chatStateService()->sendState(chat().contacts().toContact(), ChatState::Gone);

	CurrentChat.setOpen(false);

	kdebugmf(KDEBUG_FUNCTION_END, "chat destroyed\n");
}

void ChatWidget::setFormattedStringFactory(FormattedStringFactory *formattedStringFactory)
{
	CurrentFormattedStringFactory = formattedStringFactory;
}

void ChatWidget::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);

	TopBarContainer = new ChatTopBarContainerWidget(Core::instance()->chatTopBarWidgetFactoryRepository(), CurrentChat);
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

	MessagesView = Core::instance()->webkitMessagesViewFactory()->createWebkitMessagesView(CurrentChat, true, frame);

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

	InputBox = new ChatEditBox(CurrentChat, this);
	InputBox->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));
	InputBox->setMinimumHeight(10);

	messagesSearchBar->setSearchWidget(InputBox->inputBox());

	ChatType *chatType = ChatTypeManager::instance()->chatType(CurrentChat.type());
	if (chatType && chatType->name() != "Contact")
		createContactsList();

	VerticalSplitter->addWidget(HorizontalSplitter);
	VerticalSplitter->setStretchFactor(0, 1);
	VerticalSplitter->addWidget(InputBox);
	VerticalSplitter->setStretchFactor(1, 0);

	connect(VerticalSplitter, SIGNAL(splitterMoved(int, int)), this, SLOT(verticalSplitterMoved(int, int)));
	connect(InputBox->inputBox(), SIGNAL(sendMessage()), this, SLOT(sendMessage()));
	connect(InputBox->inputBox(), SIGNAL(keyPressed(QKeyEvent *, CustomInput *, bool &)),
			this, SLOT(keyPressedSlot(QKeyEvent *, CustomInput *, bool &)));
}

void ChatWidget::createContactsList()
{
	QWidget *contactsListContainer = new QWidget(HorizontalSplitter);

	QVBoxLayout *layout = new QVBoxLayout(contactsListContainer);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	BuddiesWidget = new FilteredTreeView(FilteredTreeView::FilterAtTop, this);
	BuddiesWidget->setMinimumSize(QSize(30, 30));

	TalkableTreeView *view = new TalkableTreeView(BuddiesWidget);
	view->setItemsExpandable(false);

	ModelChain *chain = new ModelChain(this);
	ContactListModel *contactListModel = new ContactListModel(chain);
	new ChatAdapter(contactListModel, CurrentChat);
	chain->setBaseModel(contactListModel);
	ProxyModel = new TalkableProxyModel(chain);

	NameTalkableFilter *nameFilter = new NameTalkableFilter(NameTalkableFilter::UndecidedMatching, ProxyModel);
	connect(BuddiesWidget, SIGNAL(filterChanged(QString)), nameFilter, SLOT(setName(QString)));

	ProxyModel->addFilter(nameFilter);
	chain->addProxyModel(ProxyModel);

	view->setChain(chain);
	view->setRootIsDecorated(false);
	view->setShowIdentityNameIfMany(false);
	view->setContextMenuEnabled(true);

	connect(view, SIGNAL(talkableActivated(Talkable)),
			Core::instance()->kaduWindow(), SLOT(talkableActivatedSlot(Talkable)));

	BuddiesWidget->setView(view);

	QToolBar *toolBar = new QToolBar(contactsListContainer);
	toolBar->addAction(Actions::instance()->createAction("editUserAction", InputBox->actionContext(), InputBox));
	toolBar->addAction(Actions::instance()->createAction("leaveChatAction", InputBox->actionContext(), InputBox));

	layout->addWidget(toolBar);
	layout->addWidget(BuddiesWidget);

	QList<int> sizes;
	sizes.append(3);
	sizes.append(1);
	HorizontalSplitter->setSizes(sizes);
}

void ChatWidget::configurationUpdated()
{
	InputBox->inputBox()->setFont(ChatConfigurationHolder::instance()->chatFont());
	QString style;
	QColor color = qApp->palette().text().color();
	if (ChatConfigurationHolder::instance()->chatTextCustomColors())
	{
		style = QString("background-color:%1;").arg(ChatConfigurationHolder::instance()->chatTextBgColor().name());
		color = ChatConfigurationHolder::instance()->chatTextFontColor();
	}
	InputBox->inputBox()->viewport()->setStyleSheet(style);
	QPalette palette = InputBox->inputBox()->palette();
	palette.setBrush(QPalette::Text, color);
	InputBox->inputBox()->setPalette(palette);
}

void ChatWidget::chatUpdated()
{
	qApp->alert(window());
}

bool ChatWidget::keyPressEventHandled(QKeyEvent *e)
{
	if (e->matches(QKeySequence::Copy) && !MessagesView->selectedText().isEmpty())
	{
		// Do not use triggerPageAction(), see bug #2345.
		MessagesView->pageAction(QWebPage::Copy)->trigger();
		return true;
	}

	if (HotKey::shortCut(e,"ShortCuts", "chat_clear"))
	{
		clearChatWindow();
		return true;
	}

	if (HotKey::shortCut(e,"ShortCuts", "chat_close"))
	{
		emit closeRequested(this);
		return true;
	}

	if (HotKey::shortCut(e,"ShortCuts", "kadu_searchuser"))
	{
		Actions::instance()->createAction("lookupUserInfoAction", InputBox->actionContext(), InputBox)->activate(QAction::Trigger);
		return true;
	}

	if (HotKey::shortCut(e,"ShortCuts", "kadu_openchatwith"))
	{
		Actions::instance()->createAction("openChatWithAction", InputBox->actionContext(), InputBox)->activate(QAction::Trigger);
		return true;
	}

	return false;
}

void ChatWidget::keyPressEvent(QKeyEvent *e)
{
	kdebugf();
	if (keyPressEventHandled(e))
		e->accept();
	else
		QWidget::keyPressEvent(e);
	kdebugf2();
}

void ChatWidget::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);

	if (ChatEditBoxSizeManager::instance()->initialized())
		commonHeightChanged(ChatEditBoxSizeManager::instance()->commonHeight());
}

ChatWidgetTitle * ChatWidget::title() const
{
	return Title;
}

void ChatWidget::addMessages(const SortedMessages &messages)
{
	if (messages.empty())
		return;

	auto unread = std::any_of(begin(messages), end(messages), [](const Message &message){
		return message.type() == MessageTypeReceived || message.type() == MessageTypeSystem;
	});

	MessagesView->setForcePruneDisabled(true);
	MessagesView->add(messages);
	if (unread)
		LastReceivedMessageTime = QDateTime::currentDateTime();
}

void ChatWidget::addMessage(const Message &message)
{
	MessagesView->add(message);

	if (message.type() != MessageTypeReceived && message.type() != MessageTypeSystem)
		return;

	LastReceivedMessageTime = QDateTime::currentDateTime();
	emit messageReceived(this);
}

SortedMessages ChatWidget::messages() const
{
	return MessagesView->messages();
}

void ChatWidget::appendSystemMessage(const QString &content)
{
	FormattedStringFactory *formattedStringFactory = Core::instance()->formattedStringFactory();
	appendSystemMessage(formattedStringFactory->fromText(content));
}

void ChatWidget::appendSystemMessage(std::unique_ptr<FormattedString> &&content)
{
	Message message = Message::create();
	message.setMessageChat(CurrentChat);
	message.setType(MessageTypeSystem);
	message.setContent(std::move(content));
	message.setReceiveDate(QDateTime::currentDateTime());
	message.setSendDate(QDateTime::currentDateTime());

	MessagesView->add(message);
}

void ChatWidget::resetEditBox()
{
	InputBox->inputBox()->clear();

	Action *action;
	action = Core::instance()->chatWidgetActions()->bold()->action(InputBox->actionContext());
	if (action)
		InputBox->inputBox()->setFontWeight(action->isChecked() ? QFont::Bold : QFont::Normal);

	action = Core::instance()->chatWidgetActions()->italic()->action(InputBox->actionContext());
	if (action)
		InputBox->inputBox()->setFontItalic(action->isChecked());

	action = Core::instance()->chatWidgetActions()->underline()->action(InputBox->actionContext());
	if (action)
		InputBox->inputBox()->setFontUnderline(action->isChecked());
}

void ChatWidget::clearChatWindow()
{
	kdebugf();

	MessageDialog *dialog = MessageDialog::create(KaduIcon("dialog-question"), tr("Kadu"), tr("Chat window will be cleared. Continue?"));
	dialog->addButton(QMessageBox::Yes, tr("Clear chat window"));
	dialog->addButton(QMessageBox::No, tr("Cancel"));

	if (!Application::instance()->configuration()->deprecatedApi()->readBoolEntry("Chat", "ConfirmChatClear") || dialog->ask())
	{
		MessagesView->clearMessages();
		MessagesView->setForcePruneDisabled(false);
		activateWindow();
	}

	kdebugf2();
}

/* sends the message typed */
void ChatWidget::sendMessage()
{
	kdebugf();
	if (InputBox->inputBox()->toPlainText().isEmpty())
	{
		kdebugf2();
		return;
	}

	emit messageSendRequested(this);

	if (!CurrentChat.isConnected())
	{
		MessageDialog::show(KaduIcon("dialog-error"), tr("Kadu"),
				QString("%1\n%2%3")
						.arg(tr("Cannot send message while being offline."))
						.arg(tr("Account:"))
						.arg(chat().chatAccount().id()),
				QMessageBox::Ok, this);
		kdebugmf(KDEBUG_FUNCTION_END, "not connected!\n");
		return;
	}

	if (!MessageManager::instance()->sendMessage(CurrentChat, InputBox->inputBox()->formattedString()))
		return;

	resetEditBox();

	// We sent the message and reseted the edit box, so composing of that message is done.
	// Note that if ComposingTimer is not active, it means that we already reported
	// composing had stopped.
	if (ComposingTimer.isActive())
		composingStopped();

	emit messageSent(this);
	kdebugf2();
}

void ChatWidget::colorSelectorAboutToClose()
{
	kdebugf();
	kdebugf2();
}

CustomInput * ChatWidget::edit() const
{
	return InputBox ? InputBox->inputBox() : 0;
}

TalkableProxyModel * ChatWidget::talkableProxyModel() const
{
	return ProxyModel;
}

int ChatWidget::countMessages() const
{
	return MessagesView ? MessagesView->countMessages() : 0;
}

bool ChatWidget::decodeLocalFiles(QDropEvent *event, QStringList &files)
{
	if (!event->mimeData()->hasUrls() || event->source() == MessagesView.get())
		return false;

	QList<QUrl> urls = event->mimeData()->urls();

	foreach (const QUrl &url, urls)
	{
		QString file = url.toLocalFile();
		if (!file.isEmpty())
		{
			//is needed to check if file refer to local file?
			QFileInfo fileInfo(file);
			if (fileInfo.exists())
				files.append(file);
		}
	}
	return !files.isEmpty();

}

void ChatWidget::dragEnterEvent(QDragEnterEvent *e)
{
	QStringList files;

	if (decodeLocalFiles(e, files))
		e->acceptProposedAction();

}

void ChatWidget::dragMoveEvent(QDragMoveEvent *e)
{
	QStringList files;

	if (decodeLocalFiles(e, files))
		e->acceptProposedAction();
}

void ChatWidget::dropEvent(QDropEvent *e)
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

Protocol *ChatWidget::currentProtocol() const
{
	return CurrentChat.chatAccount().protocolHandler();
}

void ChatWidget::requestClose()
{
	emit closeRequested(this);
}

void ChatWidget::verticalSplitterMoved(int pos, int index)
{
	Q_UNUSED(pos)
	Q_UNUSED(index)

	if (SplittersInitialized)
		ChatEditBoxSizeManager::instance()->setCommonHeight(VerticalSplitter->sizes().at(1));
}

void ChatWidget::kaduRestoreGeometry()
{
	if (!HorizontalSplitter)
		return;

	if (!chat())
		return;

	QList<int> horizSizes = stringToIntList(chat().property("chat-geometry:WidgetHorizontalSizes", QString()).toString());
	if (!horizSizes.isEmpty())
		HorizontalSplitter->setSizes(horizSizes);
}

void ChatWidget::kaduStoreGeometry()
{
	if (!HorizontalSplitter)
		return;

	if (!chat())
		return;

	chat().addProperty("chat-geometry:WidgetHorizontalSizes", intListToString(HorizontalSplitter->sizes()),
			CustomProperties::Storable);
}

void ChatWidget::showEvent(QShowEvent *e)
{
	QWidget::showEvent(e);
	if (!SplittersInitialized)
		QMetaObject::invokeMethod(this, "setUpVerticalSizes", Qt::QueuedConnection);
}

void ChatWidget::setUpVerticalSizes()
{
	// now we can accept this signal
	connect(ChatEditBoxSizeManager::instance(), SIGNAL(commonHeightChanged(int)), this, SLOT(commonHeightChanged(int)));

	// already set up by other window, so we use this window setting
	if (ChatEditBoxSizeManager::instance()->initialized())
	{
		commonHeightChanged(ChatEditBoxSizeManager::instance()->commonHeight());
		SplittersInitialized = true;
		return;
	}

	QList<int> vertSizes;
	int h = height();
	vertSizes.append(h / 3 * 2 + h % 3);
	vertSizes.append(h / 3);

	VerticalSplitter->setSizes(vertSizes);
	SplittersInitialized = true;
	ChatEditBoxSizeManager::instance()->setCommonHeight(vertSizes.at(1));
}

void ChatWidget::commonHeightChanged(int commonHeight)
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

void ChatWidget::composingStopped()
{
	ComposingTimer.stop();
	IsComposing = false;

	if (currentProtocol() && currentProtocol()->chatStateService() && chat().contacts().toContact())
		currentProtocol()->chatStateService()->sendState(chat().contacts().toContact(), ChatState::Paused);
}

void ChatWidget::checkComposing()
{
	if (!IsComposing)
		composingStopped();
	else
		// Reset IsComposing to false, so if updateComposing() method doesn't set it to true
		// before ComposingTimer hits this method again, we will call composingStopped().
		IsComposing = false;
}

void ChatWidget::updateComposing()
{
	if (!currentProtocol() || !currentProtocol()->chatStateService())
		return;

	if (!ComposingTimer.isActive())
	{
		// If the text was deleted either by sending a message or explicitly by the user,
		// let's not report it as composing.
		if (edit()->toPlainText().isEmpty())
			return;

		if (chat().contacts().toContact())
			currentProtocol()->chatStateService()->sendState(chat().contacts().toContact(), ChatState::Composing);

		ComposingTimer.start();
	}
	IsComposing = true;
}

ChatState ChatWidget::chatState() const
{
	return CurrentContactActivity;
}

void ChatWidget::contactActivityChanged(const Contact &contact, ChatState state)
{
	if (CurrentContactActivity == state)
		return;

	if (!CurrentChat.contacts().contains(contact))
		return;

	CurrentContactActivity = state;
	emit chatStateChanged(CurrentContactActivity);

	if (!CurrentFormattedStringFactory)
		return;

	if (ChatConfigurationHolder::instance()->contactStateChats())
		MessagesView->contactActivityChanged(contact, state);

	if (CurrentContactActivity == ChatState::Gone)
	{
		QString msg = "[ " + tr("%1 ended the conversation").arg(contact.ownerBuddy().display()) + " ]";
		Message message = Message::create();
		message.setMessageChat(CurrentChat);
		message.setType(MessageTypeSystem);
		message.setMessageSender(contact);
		message.setContent(CurrentFormattedStringFactory.data()->fromPlainText(msg));
		message.setSendDate(QDateTime::currentDateTime());
		message.setReceiveDate(QDateTime::currentDateTime());

		MessagesView->add(message);
	}
}

void ChatWidget::keyPressedSlot(QKeyEvent *e, CustomInput *input, bool &handled)
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

#include "moc_chat-widget.cpp"
