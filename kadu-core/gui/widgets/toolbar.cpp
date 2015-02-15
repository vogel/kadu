/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010, 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QMimeData>
#include <QtCore/QTextStream>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QCursor>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QMenu>
#include <QtWidgets/QToolButton>
#include <QtXml/QDomElement>

#include "configuration/configuration-api.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/application.h"
#include "gui/actions/actions.h"
#include "gui/windows/main-window.h"
#include "gui/windows/message-dialog.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "misc/change-notifier-lock.h"
#include "misc/change-notifier.h"
#include "misc/misc.h"
#include "debug.h"

#include "toolbar.h"

class DisabledActionsWatcher : public QObject
{
public:
	DisabledActionsWatcher()
	{
	}

	virtual bool eventFilter(QObject *o, QEvent *e)
	{
		QToolButton *button = qobject_cast<QToolButton *>(o);
		if (!button)
			return false;
		if (button->isEnabled())
			return false;
		ToolBar *toolbar = qobject_cast<ToolBar *>(button->parentWidget());
		if (!toolbar)
			return false;

		switch (e->type())
		{
			case QEvent::MouseButtonPress:
				toolbar->widgetPressed();
				return true;
			case QEvent::MouseMove:
			{
				QMouseEvent event(QEvent::MouseMove, toolbar->mapFromGlobal(QCursor::pos()), Qt::NoButton, ((QMouseEvent *)e)->buttons(), ((QMouseEvent *)e)->modifiers());
				toolbar->mouseMoveEvent(&event);
				return event.isAccepted();
			}
			case QEvent::ContextMenu:
			{
				QContextMenuEvent event(QContextMenuEvent::Mouse, toolbar->mapFromGlobal(QCursor::pos()));
				toolbar->contextMenuEvent(&event);
				return event.isAccepted();
			}
			default:
				return false;
		}
	}
};

DisabledActionsWatcher *watcher = 0;


QMap< QString, QList<ToolBar::ToolBarAction> > ToolBar::DefaultActions;

ToolBar::ToolBar(QWidget *parent) :
		QToolBar(parent), XOffset(0), YOffset(0)
{
	kdebugf();

	connect(&MyChangeNotifier, SIGNAL(changed()), this, SIGNAL(updated()));

	dragging = false;
	dropmarker.visible = false;

	setAcceptDrops(true);
	setIconSize(IconsManager::instance()->getIconsSize());

	if (!watcher)
		watcher = new DisabledActionsWatcher();

	configurationUpdated();

	kdebugf2();
}


ToolBar::~ToolBar()
{
	kdebugf();

	kdebugf2();
}

ToolBarSeparator * ToolBar::createSeparator(QAction *before, ToolBarAction &action)
{
	ToolBarSeparator *separator = new ToolBarSeparator(this);
	action.action = insertWidget(before, separator);
	action.widget = separator;
	connect(separator, SIGNAL(pressed()), this, SLOT(widgetPressed()));

	return separator;
}

ToolBarSpacer * ToolBar::createSpacer(QAction *before, ToolBarAction &action)
{
	ToolBarSpacer *spacer = new ToolBarSpacer(this);
	action.action = insertWidget(before, spacer);
	action.widget = spacer;
	connect(spacer, SIGNAL(pressed()), this, SLOT(widgetPressed()));

	return spacer;
}

QToolButton * ToolBar::createPushButton(QAction *before, ToolBarAction &action)
{
	if (!Actions::instance()->contains(action.actionName))
		return nullptr;

	MainWindow *kaduMainWindow = qobject_cast<MainWindow *>(parentWidget());
	if (!kaduMainWindow)
		return nullptr;

	auto actionByName = Actions::instance()->value(action.actionName);
	if (!actionByName)
		return nullptr;

	if (!kaduMainWindow->supportsActionType(actionByName->type()))
		return nullptr;

	action.action = Actions::instance()->createAction(action.actionName, kaduMainWindow->actionContext(), kaduMainWindow);
	insertAction(before, action.action);

	QToolButton *button = qobject_cast<QToolButton *>(widgetForAction(action.action));

	action.widget = button;
	if (button)
	{
		connect(button, SIGNAL(pressed()), this, SLOT(widgetPressed()));
		button->installEventFilter(watcher);
		button->setToolButtonStyle(action.style);

		if (action.action->menu() && Actions::instance()->contains(action.actionName))
		{
			ActionDescription *actionDescription = Actions::instance()->value(action.actionName);
			if (actionDescription)
				button->setPopupMode(actionDescription->buttonPopupMode());
		}
	}

	return button;
}

QWidget * ToolBar::createActionWidget(QAction *before, ToolBarAction &action)
{
	if (action.actionName.startsWith(QLatin1String("__separator")))
		return createSeparator(before, action);
	else if (action.actionName.startsWith(QLatin1String("__spacer")))
		return createSpacer(before, action);
	else
		return createPushButton(before, action);
}

int ToolBar::indexOf(QAction *action)
{
	if (!action)
		return -1;

	int index = 0;
	foreach (const ToolBarAction &toolBarAction, ToolBarActions)
		if (toolBarAction.action == action)
			return index;
		else
			index++;

	return -1; // not found
}

int ToolBar::indexOf(const QString &action)
{
	int index = 0;
	foreach (const ToolBarAction &toolBarAction, ToolBarActions)
		if (toolBarAction.actionName == action)
			return index;
		else
			index++;

	return -1; // not found
}

void ToolBar::addAction(const QString &actionName, Qt::ToolButtonStyle style, QAction *before)
{
	if (windowHasAction(actionName, true))
		return;

	ToolBarAction newAction;
	newAction.actionName = actionName;
	newAction.action = 0;
	newAction.widget = 0;
	newAction.style = style;

	int beforeIndex = before
			? indexOf(before)
			: ToolBarActions.size();

	createActionWidget(before, newAction);

	if (!before)
		ToolBarActions.append(newAction);
	else
		ToolBarActions.insert(beforeIndex, newAction);

	MyChangeNotifier.notify();
}

void ToolBar::moveAction(const QString &actionName, Qt::ToolButtonStyle style, QAction *before)
{
	int index = indexOf(actionName);
	if (-1 == index)
		return;

	ToolBarAction currentAction = ToolBarActions.at(index);
	if (currentAction.action == before)
		return;

	removeAction(currentAction.action);
	currentAction.action = 0;
	ToolBarActions.removeOne(currentAction);

	addAction(actionName, style, before);
}

void ToolBar::addButtonClicked(QAction *action)
{
	addAction(action->data().toString(), Qt::ToolButtonIconOnly);
}

void ToolBar::widgetPressed()
{
	MouseStart = mapFromGlobal(QCursor::pos());
}

void ToolBar::mouseMoveEvent(QMouseEvent *e)
{
	if (isMovable() && (e->buttons() & Qt::LeftButton) && (MouseStart - e->pos()).manhattanLength() >= 15)
	{
		QAction *action = actionAt(MouseStart);
		if (!action)
			return;
		foreach (const ToolBarAction &toolBarAction, ToolBarActions)
		{
			if (toolBarAction.action == action)
			{
				QDrag *drag = new ActionDrag(toolBarAction.actionName, toolBarAction.style, this);
				drag->exec(Qt::MoveAction);
				e->accept();
			}
		}
	}
	else
		QToolBar::mouseMoveEvent(e);
}

void ToolBar::dragMoveEvent(QDragMoveEvent *event)
{
	Q_UNUSED(event);
	dragging = true;
	updateDropMarker();
}

void ToolBar::dragEnterEvent(QDragEnterEvent *event)
{
	kdebugf();
	ToolBar *source = qobject_cast<ToolBar*>(event->source());
	if (source)
	{
		QString actionName;
		Qt::ToolButtonStyle style;

		auto decoded = ActionDrag::decode(event, actionName, style);
		if (!decoded)
		{
			event->ignore();
			return;
		}

		auto mine = source == this;
		auto action = Actions::instance()->value(actionName);
		auto mainWindow = qobject_cast<MainWindow *>(parentWidget());
		auto supportedAction = action && mainWindow && mainWindow->supportsActionType(action->type());
		auto isSeparator = actionName.startsWith(QLatin1String("__separator"));
		auto isSpacer = actionName.startsWith(QLatin1String("__spacer"));

		if (mine || supportedAction || isSeparator || isSpacer)
		{
			dragging = true;
			updateDropMarker();
			event->acceptProposedAction();
			return;
		}

	}
	event->ignore();
	kdebugf2();
}

void ToolBar::dragLeaveEvent(QDragLeaveEvent *event)
{
	Q_UNUSED(event);
	dragging = false;
	updateDropMarker();
}

void ToolBar::leaveEvent(QEvent *event)
{
	Q_UNUSED(event);
	dragging = false;
	updateDropMarker();
}

void ToolBar::dropEvent(QDropEvent *event)
{
	kdebugf();
	ToolBar *source = qobject_cast<ToolBar*>(event->source());

	dragging = false;
	updateDropMarker();

	if (!source)
	{
		event->ignore();
		return;
	}

	QString actionName;
	Qt::ToolButtonStyle style;
	if (!ActionDrag::decode(event, actionName, style))
	{
		event->ignore();
		return;
	}

	QAction *before = findActionToDropBefore(event->pos());

	if (source != this)
	{
		source->deleteAction(actionName);
		addAction(actionName, style, before);
	}
	else
		moveAction(actionName, style, before);

	event->acceptProposedAction();

	kdebugf2();
}

bool ToolBar::event(QEvent *event)
{
	bool result = QToolBar::event(event);

	// this is for toolbar dragging
	if (result && QEvent::MouseButtonRelease == event->type() && static_cast<QMouseEvent *>(event)->button() == Qt::LeftButton)
		MyChangeNotifier.notify();

	return result;
}

Qt::ToolBarArea ToolBar::toolBarArea()
{
	QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget());
	if (!mainWindow)
		return Qt::NoToolBarArea;
	if (isFloating())
		return Qt::NoToolBarArea;
	return mainWindow->toolBarArea(this);
}

Qt::Orientation ToolBar::orientationByArea(Qt::ToolBarArea toolbararea)
{
	if (toolbararea == Qt::TopToolBarArea || toolbararea == Qt::BottomToolBarArea)
		return Qt::Horizontal;
	if (toolbararea == Qt::NoToolBarArea)
		return Qt::Horizontal;
	return Qt::Vertical;
}

QAction * ToolBar::findActionToDropBefore(const QPoint &pos)
{
	// action
	QAction *action = actionNear(pos);
	// before
	QAction *before = action;
	if (action)
	{
		QWidget *w = widgetForAction(action);
		QPoint inwidgetpos = pos - w->pos();
		if (
				(orientationByArea(toolBarArea()) == Qt::Horizontal && inwidgetpos.x() >= w->size().width() / 2 ) ||
				(orientationByArea(toolBarArea()) == Qt::Vertical && inwidgetpos.y() >= w->size().height() / 2 )
			)
		{
			int index = actions().indexOf(action);
			before = index + 1 < actions().count() ? actions().at(index + 1) : 0;
		}
	}
	return before;
}

QAction * ToolBar::actionNear(const QPoint &pos)
{
	QAction *action;
	action = actionAt(pos);
	if (action)
		return action;
	int r = rowAt(pos);
	QRect rect = rowRect(r);
	int x = orientationByArea(toolBarArea()) == Qt::Horizontal ? pos.x() : rect.x() + rect.width() / 2;
	int y = orientationByArea(toolBarArea()) == Qt::Vertical ? pos.y() : rect.y() + rect.height() / 2;
	while (x >= 0 && y >= 0)
	{
		action = actionAt(QPoint(x,y));
		if (action)
			return action;
		orientationByArea(toolBarArea()) == Qt::Horizontal ? --x : --y;
	}
	if (!action)
	{
		QList<QAction*> list = actionsForRow(r);
		if (!list.isEmpty())
			action = list.at(0);
	}
	return action;
}

int ToolBar::rowCount()
{
	int r = 0;
	int lastp = 0;
	foreach (QAction *action, actions())
	{
		QWidget *w = widgetForAction(action);
		int p = orientationByArea(toolBarArea()) == Qt::Horizontal ? w->x() : w->y();
		if (p < lastp)
			++r;
		lastp = orientationByArea(toolBarArea()) == Qt::Horizontal ? w->x() + w->width() : w->y() + w->height();
	}
	return r + 1;
}

int ToolBar::rowAt(const QPoint &pos)
{
	int rcount = rowCount();
	for (int r = 1; r < rcount; ++r)
	{
		if (orientationByArea(toolBarArea()) == Qt::Horizontal && rowRect(r).y() > pos.y())
			return r-1;
		if (orientationByArea(toolBarArea()) == Qt::Vertical && rowRect(r).x() > pos.x())
			return r-1;
	}
	return rcount - 1;
}

QRect ToolBar::rowRect(int row)
{
	int p = -1;
	int size = 0;
	foreach (QAction *action, actionsForRow(row))
	{
		QWidget *w = widgetForAction(action);
		int actionp = orientationByArea(toolBarArea()) == Qt::Horizontal ? w->y() : w->x();
		if (p == -1 || actionp < p)
			p = actionp;
		int actionsize = orientationByArea(toolBarArea()) == Qt::Horizontal ? w->height() : w->width();
		if (actionsize > size)
			size = actionsize;
	}
	if (p == -1)
		p = 0;
	if (orientationByArea(toolBarArea()) == Qt::Horizontal)
		return QRect( 0, p, width(), size );
	else
		return QRect( p, 0, size, height() );
}

QList<QAction *> ToolBar::actionsForRow(int row)
{
	QList<QAction *> list;
	int r = 0;
	int lastp = 0;
	foreach (QAction *action, actions())
	{
		QWidget *w = widgetForAction(action);
		int p = orientationByArea(toolBarArea()) == Qt::Horizontal ? w->x() : w->y();
		if( p < lastp )
		{
			++r;
			if (r > row)
				break;
		}
		lastp = orientationByArea(toolBarArea()) == Qt::Horizontal ? w->x() + w->width() : w->y() + w->height();
		if (r == row)
			list.append(action);
	}
	return list;
}

int ToolBar::actionRow(QAction *action)
{
	for (int r = 0; r < rowCount(); ++r)
	{
		if (actionsForRow(r).contains(action))
			return r;
	}
	return -1;
}

void ToolBar::contextMenuEvent(QContextMenuEvent *e)
{
	kdebugf();

// 	if (DockArea::blocked())
// 	{
// 		e->ignore();
// 		return;
// 	}

	QMenu *menu = createContextMenu(childAt(e->pos()));
	menu->exec(e->globalPos());
	delete menu;

	e->accept();
	kdebugf2();
}

void ToolBar::configurationUpdated()
{
	QDomElement toolbarsConfig = Application::instance()->configuration()->api()->findElement(Application::instance()->configuration()->api()->rootElement(), "Toolbars");

	if (!toolbarsConfig.hasAttribute("blocked"))
		toolbarsConfig.setAttribute("blocked", "1");

	if (toolbarsConfig.isNull())
		return setMovable(false);

	setMovable(!toolbarsConfig.attribute("blocked").toInt());
	setIconSize(IconsManager::instance()->getIconsSize());
}

void ToolBar::writeToConfig(const QDomElement &parent_element)
{
	kdebugf();
	QDomElement toolbar_elem = Application::instance()->configuration()->api()->createElement(parent_element, "ToolBar");

	toolbar_elem.setAttribute("x_offset", pos().x());
	toolbar_elem.setAttribute("y_offset", pos().y());

	foreach (const ToolBarAction &toolBarAction, ToolBarActions)
	{
		QDomElement button_elem = Application::instance()->configuration()->api()->createElement(toolbar_elem, "ToolButton");
		if (toolBarAction.actionName.startsWith(QLatin1String("__separator")))
			button_elem.setAttribute("action_name", "__separator");
		else if (toolBarAction.actionName.startsWith(QLatin1String("__spacer")))
			button_elem.setAttribute("action_name", "__spacer");
		else
			button_elem.setAttribute("action_name", toolBarAction.actionName);
		button_elem.setAttribute("toolbutton_style", toolBarAction.style);
	}

	kdebugf2();
}

bool ToolBar::hasAction(const QString &action_name)
{
	kdebugf();

	foreach (const ToolBarAction &toolBarAction, ToolBarActions)
		if (toolBarAction.actionName == action_name)
			return true;
	return false;

	kdebugf2();
}

bool ToolBar::windowHasAction(const QString &action_name, bool exclude)
{
	QWidget *parent = parentWidget();
	while (parent && !qobject_cast<MainWindow *>(parent))
		parent = parent->parentWidget();

	MainWindow *window = qobject_cast<MainWindow *>(parent);
	if (!window)
		return false;

	return window->hasAction(action_name, exclude ? this : 0);
}

void ToolBar::loadFromConfig(const QDomElement &toolbar_element)
{
	kdebugf();

	ChangeNotifierLock lock(MyChangeNotifier, ChangeNotifierLock::ModeForget);

//	QString align = toolbar_element.attribute("align");
//	if (align == "right")
//		setLayoutDirection(Qt::RightToLeft);

	bool offset_ok;
	int offset = toolbar_element.attribute("offset").toInt(&offset_ok);

	if (offset_ok)
	{
		if (orientationByArea(toolBarArea()) == Qt::Horizontal)
		{
			XOffset = offset;
			YOffset = 0;
		}
		else
		{
			XOffset = 0;
			YOffset = offset;
		}
	}
	else
	{
		XOffset = toolbar_element.attribute("x_offset").toInt();
		YOffset = toolbar_element.attribute("y_offset").toInt();
	}

	ToolBarActions.clear();
	clear();

	for (QDomNode n = toolbar_element.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		const QDomElement &button_elem = n.toElement();
		if (button_elem.isNull())
			continue;
		if (button_elem.tagName() != "ToolButton")
			continue;

		QString actionName = button_elem.attribute("action_name");
		if (actionName == "__separator")
			actionName += QString::number(ToolBarSeparator::token());
		else if (actionName == "__spacer")
			actionName += QString::number(ToolBarSpacer::token());
		// TODO: remove when we stop supporting migration from 0.10.x
		else if (actionName == "whoisAction")
			actionName = "lookupUserInfoAction";

		if (windowHasAction(actionName, true))
			continue;

		Qt::ToolButtonStyle buttonStyle;

		//TODO:remove after 0.9.0
		if (button_elem.attribute("toolbutton_style").isNull())
			buttonStyle = button_elem.attribute("uses_text_label") == "1"
					? Qt::ToolButtonTextBesideIcon : Qt::ToolButtonIconOnly;
		else
			buttonStyle = Qt::ToolButtonStyle(button_elem.attribute("toolbutton_style").toInt());

		addAction(actionName, buttonStyle, 0);
	}

	kdebugf2();
}

bool actionTextLessThan(QAction *a1, QAction *a2)
{
	return a1->text().toLower() < a2->text().toLower();
}

QMenu * ToolBar::createContextMenu(QWidget *widget)
{
	currentWidget = widget;

	QToolButton *button = qobject_cast<QToolButton *>(widget);
	ToolBarSeparator *separator = button ? 0 : qobject_cast<ToolBarSeparator *>(widget);
	ToolBarSpacer *spacer = (button || separator) ? 0 : qobject_cast<ToolBarSpacer *>(widget);

	QMenu *menu = new QMenu(this);

	if (isMovable())
	{
		connect(menu, SIGNAL(aboutToShow()), this, SLOT(slotContextAboutToShow()));

		if (button)
		{
			QMenu *textPositionMenu = new QMenu(tr("Text position"), menu);
			IconsOnly = textPositionMenu->addAction(tr( "Icon only"), this, SLOT(slotContextIcons()));
			IconsOnly->setChecked(true);
			TextOnly = textPositionMenu->addAction(tr("Text only"), this, SLOT(slotContextText()));
			Text = textPositionMenu->addAction(tr("Text alongside icon"), this, SLOT(slotContextTextRight()));
			TextUnder = textPositionMenu->addAction(tr("Text under icon"), this, SLOT(slotContextTextUnder()));
			QActionGroup *textGroup = new QActionGroup(textPositionMenu);
			foreach (QAction *action, textPositionMenu->actions())
			{
				action->setActionGroup(textGroup);
				action->setCheckable(true);
			}
			menu->addMenu(textPositionMenu);
		}

		if (button)
			menu->addAction(tr("Remove this button"), this, SLOT(removeButton()));
		else if (separator)
			menu->addAction(tr("Remove this separator"), this, SLOT(removeSeparator()));
		else if (spacer)
			menu->addAction(tr("Remove this expandable spacer"), this, SLOT(removeSpacer()));

		menu->addSeparator();

		QMenu *actionsMenu = new QMenu(tr("Add new button"), menu);
		QList<QAction *> actions;
		foreach (ActionDescription *actionDescription, Actions::instance()->values())
		{
			bool supportsAction;
			MainWindow *kaduMainWindow = qobject_cast<MainWindow *>(parentWidget());

			if (kaduMainWindow)
				supportsAction = kaduMainWindow->supportsActionType(actionDescription->type());
			else // TODO is it possible?
				supportsAction = actionDescription->type() == ActionDescription::TypeGlobal;

			if (!supportsAction)
				continue;

			if (!windowHasAction(actionDescription->name(), false))
			{
				QAction *action = new QAction(actionDescription->icon().icon(), actionDescription->text(), actionsMenu);
				action->setData(actionDescription->name());

				actions.append(action);
			}
		}

		qSort(actions.begin(), actions.end(), actionTextLessThan);
		foreach (QAction *action, actions)
			actionsMenu->addAction(action);

		if (actionsMenu->isEmpty())
			actionsMenu->addAction(tr("No items to add found"))->setEnabled(false);
		else
			connect(actionsMenu, SIGNAL(triggered(QAction *)), this, SLOT(addButtonClicked(QAction *)));
		menu->addMenu(actionsMenu);

		menu->addAction(tr("Add new separator"), this, SLOT(addSeparatorClicked()));
		menu->addAction(tr("Add new expandable spacer"), this, SLOT(addSpacerClicked()));

		menu->addSeparator();

		switch (toolBarArea())
		{
			case Qt::NoToolBarArea:
			case Qt::TopToolBarArea:
				menu->addAction(tr("Create new toolbar"), parent(), SLOT(addTopToolbar()));
				break;
			case Qt::BottomToolBarArea:
				menu->addAction(tr("Create new toolbar"), parent(), SLOT(addBottomToolbar()));
				break;
			case Qt::LeftToolBarArea:
				menu->addAction(tr("Create new toolbar"), parent(), SLOT(addLeftToolbar()));
				break;
			case Qt::RightToolBarArea:
				menu->addAction(tr("Create new toolbar"), parent(), SLOT(addRightToolbar()));
				break;
			default:
				break;
		}

		menu->addAction(tr("Remove this toolbar"), this, SLOT(removeToolbar()));

		menu->addSeparator();

	}

	QAction *blockToolbars = menu->addAction(tr("Block toolbars"), this, SLOT(setBlockToolbars(bool)));
	blockToolbars->setCheckable(true);
	blockToolbars->setChecked(!isMovable());

	return menu;
}

void ToolBar::addSeparatorClicked()
{
	addAction(QString("__separator%1").arg(ToolBarSeparator::token()), Qt::ToolButtonIconOnly);
}

void ToolBar::addSpacerClicked()
{
	addAction(QString("__spacer%1").arg(ToolBarSpacer::token()), Qt::ToolButtonIconOnly);
}

void ToolBar::removeToolbar()
{
	kdebugf();

	MessageDialog *dialog = MessageDialog::create(KaduIcon("dialog-warning"), tr("Kadu"), tr("Do you really want to remove this toolbar?"), this);
	dialog->addButton(QMessageBox::Yes, tr("Remove toolbar"));
	dialog->addButton(QMessageBox::No, tr("Cancel"));

	if (dialog->ask())
		emit removed(this); // parent window will remove this toolbar
	kdebugf2();
}

bool ToolBar::isBlockToolbars()
{
	QDomElement toolbarsConfig = Application::instance()->configuration()->api()->findElement(Application::instance()->configuration()->api()->rootElement(), "Toolbars");
	if (toolbarsConfig.isNull())
		toolbarsConfig = Application::instance()->configuration()->api()->createElement(Application::instance()->configuration()->api()->rootElement(), "Toolbars");

	return toolbarsConfig.attribute("blocked") == "1";
}

void ToolBar::setBlockToolbars(bool checked)
{
	QDomElement toolbarsConfig = Application::instance()->configuration()->api()->findElement(Application::instance()->configuration()->api()->rootElement(), "Toolbars");
	if (toolbarsConfig.isNull())
		toolbarsConfig = Application::instance()->configuration()->api()->createElement(Application::instance()->configuration()->api()->rootElement(), "Toolbars");

	toolbarsConfig.setAttribute("blocked", checked ? "1" : "0");
	ConfigurationAwareObject::notifyAll();
}

void ToolBar::removeButton()
{
	QToolButton *currentButton = qobject_cast<QToolButton *>(currentWidget);
	if (!currentButton)
		return;
	foreach (const ToolBarAction &toolBarAction, ToolBarActions)
		if (toolBarAction.widget == currentButton)
		{
			// TODO: again, lame solution
			removeAction(toolBarAction.action);
			ToolBarActions.removeOne(toolBarAction);
			currentWidget = 0;
			MyChangeNotifier.notify();
			return;
		}
}

void ToolBar::removeSeparator()
{
	ToolBarSeparator *currentSeparator = qobject_cast<ToolBarSeparator *>(currentWidget);
	if (!currentSeparator)
		return;
	foreach (const ToolBarAction &toolBarAction, ToolBarActions)
		if (toolBarAction.widget == currentSeparator)
		{
			removeAction(toolBarAction.action);
			ToolBarActions.removeOne(toolBarAction);
			currentWidget = 0;
			MyChangeNotifier.notify();
			return;
		}
}

void ToolBar::removeSpacer()
{
	ToolBarSpacer *currentSpacer = qobject_cast<ToolBarSpacer *>(currentWidget);
	if (!currentSpacer)
		return;
	foreach (const ToolBarAction &toolBarAction, ToolBarActions)
		if (toolBarAction.widget == currentSpacer)
		{
			removeAction(toolBarAction.action);
			ToolBarActions.removeOne(toolBarAction);
			currentWidget = 0;
			MyChangeNotifier.notify();
			return;
		}
}

void ToolBar::deleteAction(const QString &actionName)
{
	foreach (const ToolBarAction &toolBarAction, ToolBarActions)
		if (toolBarAction.actionName == actionName)
		{
			removeAction(toolBarAction.action);
			ToolBarActions.removeOne(toolBarAction);
			MyChangeNotifier.notify();
			return;
		}

}

void ToolBar::slotContextIcons()
{
	QToolButton *currentButton = qobject_cast<QToolButton *>(currentWidget);
	if (!currentButton)
		return;

	QList<ToolBarAction>::iterator toolBarAction;
	for (toolBarAction = ToolBarActions.begin(); toolBarAction != ToolBarActions.end(); ++toolBarAction)
	{
		if ((*toolBarAction).widget == currentButton)
		{
			(*toolBarAction).style = Qt::ToolButtonIconOnly;
			currentButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
			MyChangeNotifier.notify();
			return;
		}
	}
}

void ToolBar::slotContextText()
{
	QToolButton *currentButton = qobject_cast<QToolButton *>(currentWidget);
	if (!currentButton)
		return;

	QList<ToolBarAction>::iterator toolBarAction;
	for (toolBarAction = ToolBarActions.begin(); toolBarAction != ToolBarActions.end(); ++toolBarAction)
	{
		if ((*toolBarAction).widget == currentButton)
		{
			(*toolBarAction).style = Qt::ToolButtonTextOnly;
			currentButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
			MyChangeNotifier.notify();
			return;
		}
	}
}

void ToolBar::slotContextTextUnder()
{
	QToolButton *currentButton = qobject_cast<QToolButton *>(currentWidget);
	if (!currentButton)
		return;

	QList<ToolBarAction>::iterator toolBarAction;
	for (toolBarAction = ToolBarActions.begin(); toolBarAction != ToolBarActions.end(); ++toolBarAction)
	{
		if ((*toolBarAction).widget == currentButton)
		{
			(*toolBarAction).style = Qt::ToolButtonTextUnderIcon;
			currentButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
			MyChangeNotifier.notify();
			return;
		}
	}
}

void ToolBar::slotContextTextRight()
{
	QToolButton *currentButton = qobject_cast<QToolButton *>(currentWidget);
	if (!currentButton)
		return;

	QList<ToolBarAction>::iterator toolBarAction;
	for (toolBarAction = ToolBarActions.begin(); toolBarAction != ToolBarActions.end(); ++toolBarAction)
	{
		if ((*toolBarAction).widget == currentButton)
		{
			(*toolBarAction).style = Qt::ToolButtonTextBesideIcon;
			currentButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
			MyChangeNotifier.notify();
			return;
		}
	}
}

void ToolBar::slotContextAboutToShow()
{
	QToolButton *currentButton = qobject_cast<QToolButton *>(currentWidget);
	if (!currentButton)
		return;
	// Check the actions that should be checked
	switch (currentButton->toolButtonStyle())
	{
		case Qt::ToolButtonIconOnly:
		default:
			IconsOnly->setChecked(true);
			break;
		case Qt::ToolButtonTextBesideIcon:
			Text->setChecked(true);
			break;
		case Qt::ToolButtonTextOnly:
			TextOnly->setChecked(true);
			break;
		case Qt::ToolButtonTextUnderIcon:
			TextUnder->setChecked(true);
			break;
	}
}

void ToolBar::updateDropMarker()
{
	if (!dragging)
	{
		dropmarker.visible = false;
		update();
		return;
	}
	// action
	QPoint pos = mapFromGlobal(QCursor::pos());
	QAction *before = findActionToDropBefore(pos);
	int r = rowAt(pos);
	ToolBarDropMarker newdropmarker;
	newdropmarker.visible = true;
	newdropmarker.x = newdropmarker.y = 0;
	// marker's position
	if (before)
	{
		bool set = false;
		if (r < actionRow(before))
		{
			QList<QAction*> list = actionsForRow(r);
			if (!list.isEmpty())
			{
				QAction *action = list.last();
				if (orientationByArea(toolBarArea()) == Qt::Horizontal)
					newdropmarker.x = widgetForAction(action)->geometry().bottomRight().x() + 1;
				else
					newdropmarker.y = widgetForAction(action)->geometry().bottomRight().y() + 1;
				set = true;
			}
		}
		if (!set)
		{
			if (orientationByArea(toolBarArea()) == Qt::Horizontal)
				newdropmarker.x = widgetForAction(before)->x() - 1;
			else
				newdropmarker.y = widgetForAction(before)->y() - 1;
		}
	}
	else
	{
		if (!actions().isEmpty())
		{
			if (orientationByArea(toolBarArea()) == Qt::Horizontal)
				newdropmarker.x = widgetForAction(actions().last())->geometry().bottomRight().x() + 1;
			else
				newdropmarker.y = widgetForAction(actions().last())->geometry().bottomRight().y() + 1;
		}
		else
			newdropmarker.visible = false;
	}
	// count marker's position and size
	QRect rowrect = rowRect(r);
	if (orientationByArea(toolBarArea()) == Qt::Horizontal)
	{
		newdropmarker.y = rowrect.y();
		newdropmarker.size = rowrect.height();
	}
	else
	{
		newdropmarker.x = rowrect.x();
		newdropmarker.size = rowrect.width();
	}
	// update if needed
	if (newdropmarker != dropmarker)
	{
		dropmarker = newdropmarker;
		update();
	}
}

void ToolBar::paintEvent(QPaintEvent *event)
{
	QToolBar::paintEvent(event);
	if (dropmarker.visible)
		paintDropMarker();
}

void ToolBar::paintDropMarker()
{
	QPainter painter(this);
	if (orientationByArea(toolBarArea()) == Qt::Horizontal)
	{
		int x = dropmarker.x;
		if (x > width() - 2)
			x = width() - 2;
		if (x < 1)
			x = 1;
		int y = dropmarker.y;
		for (int dy = 0; dy < dropmarker.size; ++dy)
		{
			painter.setPen(dy%2 == 0 ? QColor(255,255,255,240) : QColor(16,16,16,240));
			painter.drawPoint(x, y+dy);
			painter.setPen(dy%2 == 1 ? QColor(255,255,255,160) : QColor(16,16,16,160));
			painter.drawPoint(x-1, y+dy);
			painter.drawPoint(x+1, y+dy);
		}
	}
	else
	{
		int y = dropmarker.y;
		if (y > height() - 2)
			y = height() - 2;
		if (y < 1)
			y = 1;
		int x = dropmarker.x;
		for (int dx = 0; dx < dropmarker.size; ++dx)
		{
			painter.setPen(dx%2 == 0 ? QColor(255,255,255,240) : QColor(16,16,16,240));
			painter.drawPoint(x+dx, y);
			painter.setPen(dx%2 == 1 ? QColor(255,255,255,160) : QColor(16,16,16,160));
			painter.drawPoint(x+dx, y-1);
			painter.drawPoint(x+dx, y+1);
		}
	}
}

ActionDrag::ActionDrag(const QString &actionName, Qt::ToolButtonStyle style, QWidget *dragSource)
	: QDrag(dragSource)
{
	kdebugf();

	QByteArray data;
	QMimeData *mimeData = new QMimeData;

	QString string = actionName + '\n' + QString::number(Qt::ToolButtonStyle(style));

	data = string.toUtf8();

	mimeData->setData("application/x-kadu-action", data);

	setMimeData(mimeData);

	kdebugf2();
}

bool ActionDrag::decode(QDropEvent *event, QString &actionName, Qt::ToolButtonStyle &style)
{
	const QMimeData *mimeData = event->mimeData();

	if (!mimeData->hasFormat("application/x-kadu-action"))
		return false;

	QTextStream stream(mimeData->data("application/x-kadu-action"), QIODevice::ReadOnly);
	stream.setCodec("UTF-8");

	if (stream.atEnd())
		return false;

	actionName = stream.readLine();

	if (stream.atEnd())
		return false;

	int tmp;
	stream >> tmp;
	style = Qt::ToolButtonStyle(tmp);

	return true;
}


ToolBarSeparator::ToolBarSeparator(QWidget *parent) : QWidget(parent)
{
	resize(QSize(TOOLBAR_SEPARATOR_SIZE, TOOLBAR_SEPARATOR_SIZE));
	setMinimumSize(size());
	setMaximumSize(size());
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

void ToolBarSeparator::mousePressEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
	emit pressed();
}

int ToolBarSeparator::Token = 0;

int ToolBarSeparator::token()
{
	++Token;
	return Token;
}


ToolBarSpacer::ToolBarSpacer(QWidget *parent) : QWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void ToolBarSpacer::mousePressEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
	emit pressed();
}

int ToolBarSpacer::Token = 0;

int ToolBarSpacer::token()
{
	++Token;
	return Token;
}

#include "moc_toolbar.cpp"
