/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2005, 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
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

#include <QtCore/QTimer>
#include <QtCore/QTextStream>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QCursor>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QMenu>
#include <QtGui/QPainter>
#include <QtGui/QToolButton>

#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "gui/actions/actions.h"
#include "gui/windows/main-window.h"
#include "gui/windows/message-dialog.h"

#include "debug.h"
#include "icons-manager.h"
#include "misc/misc.h"

#include "toolbar.h"

class DisabledActionsWatcher : public QObject
{
public:
	DisabledActionsWatcher()
	{
	}

	virtual bool eventFilter(QObject *o, QEvent *e)
	{
		QToolButton *button = dynamic_cast<QToolButton *>(o);
		if (!button)
			return false;
		if (button->isEnabled())
			return false;
		ToolBar *toolbar = dynamic_cast<ToolBar *>(button->parent());
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

ToolBar::ToolBar(QWidget *parent)
	: QToolBar(parent), XOffset(0), YOffset(0)
{
	kdebugf();

	dragging = false;
	dropmarker = -1;

	setAcceptDrops(true);
	setIconSize(IconsManager::instance()->getIconsSize());

	setAttribute(Qt::WA_PaintOutsidePaintEvent, true);

	if (!watcher)
		watcher = new DisabledActionsWatcher();

	connect(&KaduActions, SIGNAL(actionLoaded(const QString &)), this, SLOT(actionLoaded(const QString &)));
	connect(&KaduActions, SIGNAL(actionUnloaded(const QString &)), this, SLOT(actionUnloaded(const QString &)));

	configurationUpdated();

	kdebugf2();
}


ToolBar::~ToolBar()
{
	kdebugf();

	disconnect(&KaduActions, SIGNAL(actionLoaded(const QString &)), this, SLOT(actionLoaded(const QString &)));
	disconnect(&KaduActions, SIGNAL(actionUnloaded(const QString &)), this, SLOT(actionUnloaded(const QString &)));

	kdebugf2();
}

void ToolBar::addAction(const QString &actionName, Qt::ToolButtonStyle style, QAction *before)
{
	if (hasAction(actionName))
		return;

	ToolBarAction newAction;
	newAction.actionName = actionName;
	newAction.action = 0;
	newAction.widget = 0;
	newAction.style = style;

	if (before)
	{
		int index = 0;

		foreach (const ToolBarAction &toolBarAction, ToolBarActions)
		{
			if (toolBarAction.action == before)
			{
				if (actionName.startsWith("__separator"))
				{
					QWidget *widget = new ToolBarSeparator(this);
					newAction.action = insertWidget(before, widget);
					newAction.widget = widget;
					connect(widget, SIGNAL(pressed()), this, SLOT(widgetPressed()));
				}
				else if (actionName.startsWith("__spacer"))
				{
					QWidget *widget = new ToolBarSpacer(this);
					newAction.action = insertWidget(before, widget);
					newAction.widget = widget;
					connect(widget, SIGNAL(pressed()), this, SLOT(widgetPressed()));
				}
				else
				{
					newAction.action = KaduActions.createAction(actionName, dynamic_cast<MainWindow *>(parent()));
					insertAction(before, newAction.action);
					QToolButton *button = dynamic_cast<QToolButton *>(widgetForAction(newAction.action));
					newAction.widget = button;
					connect(button, SIGNAL(pressed()), this, SLOT(widgetPressed()));
					button->installEventFilter(watcher);
					button->setToolButtonStyle(newAction.style);
				}
				ToolBarActions.insert(index, newAction);
				return;
			}
			index++;
		}
	}

	if (actionName.startsWith("__separator"))
	{
		QWidget *widget = new ToolBarSeparator(this);
		newAction.action = addWidget(widget);
		newAction.widget = widget;
		connect(widget, SIGNAL(pressed()), this, SLOT(widgetPressed()));
	}
	else if (actionName.startsWith("__spacer"))
	{
		QWidget *widget = new ToolBarSpacer(this);
		newAction.action = addWidget(widget);
		newAction.widget = widget;
		connect(widget, SIGNAL(pressed()), this, SLOT(widgetPressed()));
	}
	else
	{
		newAction.action = KaduActions.createAction(actionName, dynamic_cast<MainWindow *>(parent()));
		QToolBar::addAction(newAction.action);
		QToolButton *button = dynamic_cast<QToolButton *>(widgetForAction(newAction.action));
		newAction.widget = button;
		connect(button, SIGNAL(pressed()), this, SLOT(widgetPressed()));
		button->installEventFilter(watcher);
		button->setToolButtonStyle(newAction.style);
	}
	ToolBarActions.append(newAction);
}

void ToolBar::moveAction(const QString &actionName, Qt::ToolButtonStyle style, QAction *before)
{
	bool actionFound = false;
	int index = 0;

	ToolBarAction newAction;
	newAction.actionName = actionName;
	newAction.action = 0;
	newAction.widget = 0;
	newAction.style = style;

 	foreach (const ToolBarAction &toolBarAction, ToolBarActions)
	{
		if (toolBarAction.actionName == actionName)
		{
			if (toolBarAction.action == before)
				return;
			else
			{
				removeAction(toolBarAction.action);
				newAction.action = 0;
				ToolBarActions.removeOne(toolBarAction);
				if (!actionFound)
					--index;
			}
		}
		if (toolBarAction.action != before && !actionFound)
			++index;
		else
			actionFound = true;
	}

	if (actionName.startsWith("__separator"))
	{
		QWidget *widget = new ToolBarSeparator(this);
		if (index > ToolBarActions.count() - 1)
			newAction.action = QToolBar::addWidget(widget);
		else
			newAction.action = insertWidget(ToolBarActions[index].action, widget);
		newAction.widget = widget;
		connect(widget, SIGNAL(pressed()), this, SLOT(widgetPressed()));
	}
	else if (actionName.startsWith("__spacer"))
	{
		QWidget *widget = new ToolBarSpacer(this);
		if (index > ToolBarActions.count() - 1)
			newAction.action = QToolBar::addWidget(widget);
		else
			newAction.action = insertWidget(ToolBarActions[index].action, widget);
		newAction.widget = widget;
		connect(widget, SIGNAL(pressed()), this, SLOT(widgetPressed()));
	}
	else
	{
		newAction.action = KaduActions.createAction(actionName, dynamic_cast<MainWindow *>(parent()));
		if (index > ToolBarActions.count() - 1)
			QToolBar::addAction(newAction.action);
		else
			insertAction(ToolBarActions[index].action, newAction.action);
		QToolButton *button = dynamic_cast<QToolButton *>(widgetForAction(newAction.action));
		newAction.widget = button;
		connect(button, SIGNAL(pressed()), this, SLOT(widgetPressed()));
		button->installEventFilter(watcher);
		button->setToolButtonStyle(newAction.style);
	}
	ToolBarActions.insert(index, newAction);
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
	ToolBar *source = dynamic_cast<ToolBar*>(event->source());
	if (source)
	{
		QString actionName;
		Qt::ToolButtonStyle style;

		if (
				ActionDrag::decode(event, actionName, style) &&
				(
					(event->source() == this)
					|| (!hasAction(actionName) && KaduActions.contains(actionName) && dynamic_cast<MainWindow *>(parent())->supportsActionType(KaduActions[actionName]->type()))
					|| (actionName.startsWith("__separator") || actionName.startsWith("__spacer"))
				)
			)
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
	ToolBar *source = dynamic_cast<ToolBar*>(event->source());

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

	ToolBar *source_toolbar = dynamic_cast<ToolBar*>(event->source());
	if (source_toolbar != this)
	{
		source_toolbar->deleteAction(actionName);
		addAction(actionName, style, before);
	}
	else
		moveAction(actionName, style, before);

	event->acceptProposedAction();

	kdebugf2();
}


QAction *ToolBar::findActionToDropBefore(QPoint pos)
{
	// event->pos() may be a point BETWEEN two toolbar's widgets !
	QMainWindow *mainWindow = dynamic_cast<QMainWindow *>(parent());
	QAction *action = 0;
	// middle of the toolbar (!)
	if (mainWindow->toolBarArea(this)==Qt::TopToolBarArea || mainWindow->toolBarArea(this)==Qt::BottomToolBarArea)
		pos.setY(height() / 2);
	if (mainWindow->toolBarArea(this)==Qt::LeftToolBarArea || mainWindow->toolBarArea(this)==Qt::RightToolBarArea)
		pos.setX(width() / 2);
	// search
	if (!actions().isEmpty())
	{
		while (!action)
		{
			action = actionAt(pos);
			if (mainWindow->toolBarArea(this)==Qt::TopToolBarArea || mainWindow->toolBarArea(this)==Qt::BottomToolBarArea)
				pos -= QPoint(1, 0);
			if (mainWindow->toolBarArea(this)==Qt::LeftToolBarArea || mainWindow->toolBarArea(this)==Qt::RightToolBarArea)
				pos -= QPoint(0, 1);
			if (pos.x() < 0 || pos.y() < 0)
			{
				action = actions().first();
				break;
			}
		}
	}
	QAction *before = action;
	if (action)
	{
		QWidget *actionwidget = widgetForAction(action);
		QPoint inwidgetpos = pos - actionwidget->pos();
		QSize widgetsize = actionwidget->size();
		if (
				((mainWindow->toolBarArea(this) == Qt::TopToolBarArea  || mainWindow->toolBarArea(this) == Qt::BottomToolBarArea) && inwidgetpos.x() >= widgetsize.width() / 2 )
				|| ((mainWindow->toolBarArea(this) == Qt::LeftToolBarArea || mainWindow->toolBarArea(this) == Qt::RightToolBarArea ) && inwidgetpos.y() >= widgetsize.height() / 2 )
			)
		{
			int index = actions().indexOf(action);
			before = index + 1 < actions().count() ? actions().at(index + 1) : 0;
		}
	}
	return before;
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
	QDomElement toolbarsConfig = xml_config_file->findElement(xml_config_file->rootElement(), "Toolbars");

	if (toolbarsConfig.isNull())
		return setMovable(false);

	setMovable(!toolbarsConfig.attribute("blocked").toInt());
	setIconSize(IconsManager::instance()->getIconsSize());
}

void ToolBar::writeToConfig(QDomElement parent_element)
{
	kdebugf();
	QDomElement toolbar_elem = xml_config_file->createElement(parent_element, "ToolBar");

	toolbar_elem.setAttribute("x_offset", pos().x());
	toolbar_elem.setAttribute("y_offset", pos().y());

	foreach (const ToolBarAction &toolBarAction, ToolBarActions)
	{
		QDomElement button_elem = xml_config_file->createElement(toolbar_elem, "ToolButton");
		if (toolBarAction.actionName.startsWith("__separator"))
			button_elem.setAttribute("action_name", "__separator");
		else if (toolBarAction.actionName.startsWith("__spacer"))
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

void ToolBar::actionLoaded(const QString &actionName)
{
	if (!hasAction(actionName))
		return;
	//workaround for modules
	QTimer::singleShot(0, this, SLOT(updateButtons()));
}

void ToolBar::actionUnloaded(const QString &actionName)
{
	if (!hasAction(actionName))
		return;

	QList<ToolBarAction>::iterator toolBarAction;
 	for (toolBarAction = ToolBarActions.begin(); toolBarAction != ToolBarActions.end(); ++toolBarAction)
	{
		if ((*toolBarAction).actionName == actionName)
		{
			(*toolBarAction).action = 0;
			(*toolBarAction).widget = 0;
			return;
		}
	}
}

void ToolBar::updateButtons()
{
	QAction *lastAction = 0;
	MainWindow *kaduMainWindow = dynamic_cast<MainWindow *>(parent());

	if (!kaduMainWindow)
		return;

	QList<ToolBarAction>::iterator toolBarAction;
	QList<ToolBarAction>::iterator toolBarNextAction;

 	for (toolBarAction = ToolBarActions.begin(), toolBarNextAction = ToolBarActions.begin() + 1;
			toolBarAction != ToolBarActions.end(); ++toolBarAction, ++toolBarNextAction)
	{
		const QString &actionName = (*toolBarAction).actionName;

		if ((*toolBarAction).action)
		{
			if (KaduActions.contains(actionName) || actionName.startsWith("__separator") || actionName.startsWith("__spacer"))
			{
				lastAction = (*toolBarAction).action;
				continue;
			}
			else
			{
				if ((*toolBarAction).action)
				{
					delete (*toolBarAction).action;
					(*toolBarAction).action = 0;
				}
			}
		}

		if (actionName.startsWith("__separator"))
		{
			QWidget *widget = new ToolBarSeparator(this);
			if (toolBarNextAction != ToolBarActions.end() && (*toolBarNextAction).action)
				(*toolBarAction).action = insertWidget((*toolBarNextAction).action, widget);
			else
				(*toolBarAction).action = QToolBar::addWidget(widget);
			(*toolBarAction).widget = widget;
			connect(widget, SIGNAL(pressed()), this, SLOT(widgetPressed()));
			lastAction = (*toolBarAction).action;
		}
		else if (actionName.startsWith("__spacer"))
		{
			QWidget *widget = new ToolBarSpacer(this);
			if (toolBarNextAction != ToolBarActions.end() && (*toolBarNextAction).action)
				(*toolBarAction).action = insertWidget((*toolBarNextAction).action, widget);
			else
				(*toolBarAction).action = QToolBar::addWidget(widget);
			(*toolBarAction).widget = widget;
			connect(widget, SIGNAL(pressed()), this, SLOT(widgetPressed()));
			lastAction = (*toolBarAction).action;
		}
		else if (KaduActions.contains(actionName) && KaduActions.contains(actionName) && kaduMainWindow->supportsActionType(KaduActions[actionName]->type()))
		{
			(*toolBarAction).action = KaduActions.createAction(actionName, dynamic_cast<MainWindow *>(parent()));
			if (toolBarNextAction != ToolBarActions.end() && (*toolBarNextAction).action)
				insertAction((*toolBarNextAction).action, (*toolBarAction).action);
			else
				QToolBar::addAction((*toolBarAction).action);
			QToolButton *button = dynamic_cast<QToolButton *>(widgetForAction((*toolBarAction).action));
			(*toolBarAction).widget = button;
			connect(button, SIGNAL(pressed()), this, SLOT(widgetPressed()));
			button->installEventFilter(watcher);
			button->setToolButtonStyle((*toolBarAction).style);
			lastAction = (*toolBarAction).action;
		}

	}
}

void ToolBar::loadFromConfig(QDomElement toolbar_element)
{
	kdebugf();

//	QString align = toolbar_element.attribute("align");
//	if (align == "right")
//		setLayoutDirection(Qt::RightToLeft);

	bool offset_ok;
	int offset = toolbar_element.attribute("offset").toInt(&offset_ok);

	if (offset_ok)
	{
		if (orientation() == Qt::Horizontal)
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

	QVariant toolButtonStyle;
	for (QDomNode n = toolbar_element.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		const QDomElement &button_elem = n.toElement();
		if (button_elem.isNull())
			continue;
		if (button_elem.tagName() != "ToolButton")
			continue;

		ToolBarAction action;

		action.actionName = button_elem.attribute("action_name");

		if (action.actionName == "__separator")
			action.actionName += QString::number(ToolBarSeparator::token());
		if (action.actionName == "__spacer")
			action.actionName += QString::number(ToolBarSpacer::token());

		if (hasAction(action.actionName))
			continue;
		//TODO:remove after 0.7.0
		if (button_elem.attribute("toolbutton_style").isNull())
			action.style = button_elem.attribute("uses_text_label") == "1"
					? Qt::ToolButtonTextBesideIcon : Qt::ToolButtonIconOnly;
		else
		{
			toolButtonStyle = button_elem.attribute("toolbutton_style");
			action.style = Qt::ToolButtonStyle(toolButtonStyle.toInt());
		}

		action.action = 0;
		action.widget = 0;

		ToolBarActions.append(action);
	}

	updateButtons();

	kdebugf2();
}

QMenu *ToolBar::createContextMenu(QWidget *widget)
{
	currentWidget = widget;

	QToolButton *button = dynamic_cast<QToolButton *>(widget);
	ToolBarSeparator *separator = dynamic_cast<ToolBarSeparator *>(widget);
	ToolBarSpacer *spacer = dynamic_cast<ToolBarSpacer *>(widget);

	QMenu *menu = new QMenu(this);

	if (isMovable())
	{
		connect(menu, SIGNAL(aboutToShow()), this, SLOT(slotContextAboutToShow()));

		if (button)
		{
			QMenu *textPositionMenu = new QMenu(tr("Text position"), this);
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
		if (separator)
			menu->addAction(tr("Remove this separator"), this, SLOT(removeSeparator()));
		if (spacer)
			menu->addAction(tr("Remove this expandable spacer"), this, SLOT(removeSpacer()));

		menu->addSeparator();

		QMenu *actionsMenu = new QMenu(tr("Add new button"), this);
		foreach (ActionDescription *actionDescription, KaduActions.values())
		{
			bool supportsAction;
			MainWindow *kaduMainWindow= 0;
			if (parent())
				kaduMainWindow = dynamic_cast<MainWindow *>(parent());

			if (kaduMainWindow)
				supportsAction = kaduMainWindow->supportsActionType(actionDescription->type());
			else // TODO is it possible?
				supportsAction = actionDescription->type() == ActionDescription::TypeGlobal;

			if (!supportsAction)
				continue;

			if (!hasAction(actionDescription->name()))
			{
				QAction *action = actionsMenu->addAction(IconsManager::instance()->iconByPath(actionDescription->iconPathOn()), actionDescription->text());
				action->setData(actionDescription->name());
			}
		}

		if (actionsMenu->isEmpty())
			actionsMenu->addAction(tr("No items to add found"))->setEnabled(false);
		else
			connect(actionsMenu, SIGNAL(triggered(QAction *)), this, SLOT(addButtonClicked(QAction *)));
		menu->addMenu(actionsMenu);

		menu->addAction(tr("Add new separator"), this, SLOT(addSeparatorClicked()));
		menu->addAction(tr("Add new expandable spacer"), this, SLOT(addSpacerClicked()));

		menu->addSeparator();

		QMainWindow *mainWindow = dynamic_cast<QMainWindow *>(parent());
		switch (mainWindow->toolBarArea(this))
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
	if (MessageDialog::ask(tr("Do you really want to remove selected toolbar?"), "dialog-warning", this))
		deleteLater();
	kdebugf2();
}

bool ToolBar::isBlockToolbars()
{
	QDomElement toolbarsConfig = xml_config_file->findElement(xml_config_file->rootElement(), "Toolbars");
	if (toolbarsConfig.isNull())
		toolbarsConfig = xml_config_file->createElement(xml_config_file->rootElement(), "Toolbars");

	return toolbarsConfig.attribute("blocked") == "1";
}

void ToolBar::setBlockToolbars(bool checked)
{
	QDomElement toolbarsConfig = xml_config_file->findElement(xml_config_file->rootElement(), "Toolbars");
	if (toolbarsConfig.isNull())
		toolbarsConfig = xml_config_file->createElement(xml_config_file->rootElement(), "Toolbars");

	toolbarsConfig.setAttribute("blocked", checked ? "1" : "0");
	ConfigurationAwareObject::notifyAll();
}

void ToolBar::removeButton()
{
	QToolButton *currentButton = dynamic_cast<QToolButton *>(currentWidget);
	if (!currentButton)
		return;
	foreach (const ToolBarAction &toolBarAction, ToolBarActions)
		if (toolBarAction.widget == currentButton)
		{
			// TODO: again, lame solution
			removeAction(toolBarAction.action);
			ToolBarActions.removeOne(toolBarAction);
			currentWidget = 0;
			return;
		}
}

void ToolBar::removeSeparator()
{
	ToolBarSeparator *currentSeparator = dynamic_cast<ToolBarSeparator *>(currentWidget);
	if (!currentSeparator)
		return;
	foreach (const ToolBarAction &toolBarAction, ToolBarActions)
		if (toolBarAction.widget == currentSeparator)
		{
			removeAction(toolBarAction.action);
			ToolBarActions.removeOne(toolBarAction);
			currentWidget = 0;
			return;
		}
}

void ToolBar::removeSpacer()
{
	ToolBarSpacer *currentSpacer = dynamic_cast<ToolBarSpacer *>(currentWidget);
	if (!currentSpacer)
		return;
	foreach (const ToolBarAction &toolBarAction, ToolBarActions)
		if (toolBarAction.widget == currentSpacer)
		{
			removeAction(toolBarAction.action);
			ToolBarActions.removeOne(toolBarAction);
			currentWidget = 0;
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
			return;
		}

}

void ToolBar::slotContextIcons()
{
	QToolButton *currentButton = dynamic_cast<QToolButton *>(currentWidget);
	if (!currentButton)
		return;

	QList<ToolBarAction>::iterator toolBarAction;
	for (toolBarAction = ToolBarActions.begin(); toolBarAction != ToolBarActions.end(); ++toolBarAction)
	{
		if ((*toolBarAction).widget == currentButton)
		{
			(*toolBarAction).style = Qt::ToolButtonIconOnly;
			currentButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
			return;
		}
	}
}

void ToolBar::slotContextText()
{
	QToolButton *currentButton = dynamic_cast<QToolButton *>(currentWidget);
	if (!currentButton)
		return;

	QList<ToolBarAction>::iterator toolBarAction;
	for (toolBarAction = ToolBarActions.begin(); toolBarAction != ToolBarActions.end(); ++toolBarAction)
	{
		if ((*toolBarAction).widget == currentButton)
		{
			(*toolBarAction).style = Qt::ToolButtonTextOnly;
			currentButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
			return;
		}
	}
}

void ToolBar::slotContextTextUnder()
{
	QToolButton *currentButton = dynamic_cast<QToolButton *>(currentWidget);
	if (!currentButton)
		return;

	QList<ToolBarAction>::iterator toolBarAction;
	for (toolBarAction = ToolBarActions.begin(); toolBarAction != ToolBarActions.end(); ++toolBarAction)
	{
		if ((*toolBarAction).widget == currentButton)
		{
			(*toolBarAction).style = Qt::ToolButtonTextUnderIcon;
			currentButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
			return;
		}
	}
}

void ToolBar::slotContextTextRight()
{
	QToolButton *currentButton = dynamic_cast<QToolButton *>(currentWidget);
	if (!currentButton)
		return;

	QList<ToolBarAction>::iterator toolBarAction;
	for (toolBarAction = ToolBarActions.begin(); toolBarAction != ToolBarActions.end(); ++toolBarAction)
	{
		if ((*toolBarAction).widget == currentButton)
		{
			(*toolBarAction).style = Qt::ToolButtonTextBesideIcon;
			currentButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
			return;
		}
	}
}

void ToolBar::slotContextAboutToShow()
{
	QToolButton *currentButton = dynamic_cast<QToolButton *>(currentWidget);
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
		dropmarker = -1;
		repaint();
		return;
	}
	QPoint pos = mapFromGlobal(QCursor::pos());
	QAction *before = findActionToDropBefore(pos);
	QMainWindow *mainWindow = dynamic_cast<QMainWindow *>(parent());
	int newdropmarker = -1;
	if (before)
	{
		if (mainWindow->toolBarArea(this) == Qt::TopToolBarArea || mainWindow->toolBarArea(this) == Qt::BottomToolBarArea)
			newdropmarker = widgetForAction(before)->x() - 1;
		if (mainWindow->toolBarArea(this) == Qt::LeftToolBarArea || mainWindow->toolBarArea(this) == Qt::RightToolBarArea)
			newdropmarker = widgetForAction(before)->y() - 1;
	}
	else
	{
		if (!actions().isEmpty())
		{
			if (mainWindow->toolBarArea(this) == Qt::TopToolBarArea || mainWindow->toolBarArea(this) == Qt::BottomToolBarArea)
				newdropmarker = widgetForAction(actions().last())->geometry().bottomRight().x() + 1;
			if (mainWindow->toolBarArea(this) == Qt::LeftToolBarArea || mainWindow->toolBarArea(this) == Qt::RightToolBarArea)
				newdropmarker = widgetForAction(actions().last())->geometry().bottomRight().y() + 1;
		}
		else
			newdropmarker = 0;
	}
	if (newdropmarker != dropmarker)
	{
		dropmarker = newdropmarker;
		repaint();
	}
}

void ToolBar::paintEvent(QPaintEvent *event)
{
	QToolBar::paintEvent(event);
	if (dropmarker > -1)
		QTimer::singleShot(0, this, SLOT(paintDropMarker()));
}

void ToolBar::paintDropMarker()
{
	QPainter painter(this);
	QMainWindow *mainWindow = dynamic_cast<QMainWindow *>(parent());
	if (mainWindow->toolBarArea(this) == Qt::TopToolBarArea || mainWindow->toolBarArea(this) == Qt::BottomToolBarArea)
	{
		int marker = dropmarker;
		if (marker > width() - 2)
			marker = width() - 2;
		if (marker < 1)
			marker = 1;
		for (int p = 0; p <= height() - 1; ++p)
		{
			painter.setPen(p%2 == 0 ? QColor(255,255,255,240) : QColor(16,16,16,240));
			painter.drawPoint(marker, p);
			painter.setPen(p%2 == 1 ? QColor(255,255,255,160) : QColor(16,16,16,160));
			painter.drawPoint(marker-1, p);
			painter.drawPoint(marker+1, p);
		}
	}
	if (mainWindow->toolBarArea(this)==Qt::LeftToolBarArea || mainWindow->toolBarArea(this)==Qt::RightToolBarArea )
	{
		int marker = dropmarker;
		if (marker > height() - 2)
			marker = height() - 2;
		if (marker < 1)
			marker = 1;
		for (int p = 0; p <= width() - 1; ++p)
		{
			painter.setPen(p%2 == 0 ? QColor(255,255,255,240) : QColor(16,16,16,240));
			painter.drawPoint(p, marker);
			painter.setPen(p%2 == 1 ? QColor(255,255,255,160) : QColor(16,16,16,160));
			painter.drawPoint(p, marker-1);
			painter.drawPoint(p, marker+1);
		}
	}
}

ActionDrag::ActionDrag(const QString &actionName, Qt::ToolButtonStyle style, QWidget *dragSource)
	: QDrag(dragSource)
{
	kdebugf();

	QByteArray data;
	QMimeData *mimeData = new QMimeData;

	QString string = actionName + "\n" + QString::number(Qt::ToolButtonStyle(style));

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
