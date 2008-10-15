/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QTimer>
#include <QtCore/QTextStream>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QCursor>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QMenu>
#include <QtGui/QToolButton>

#include "action.h"
#include "config_file.h"
#include "debug.h"
#include "icons_manager.h"
#include "kadu_main_window.h"
#include "message_box.h"
#include "misc.h"

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
				toolbar->buttonPressed();
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

ToolBar::ToolBar(QWidget * parent)
	: QToolBar(parent), XOffset(0), YOffset(0)
{
	kdebugf();

	setAcceptDrops(true);
	setIconSize(QSize(22, 22));

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

void ToolBar::addAction(const QString &actionName, bool showLabel, QAction *after)
{
	if (hasAction(actionName))
		return;

	ToolBarAction newAction;
	newAction.actionName = actionName;
	newAction.action = 0;
	newAction.button = 0;
	newAction.showLabel = showLabel;

	if (after)
	{
		int index = 0;

		foreach(const ToolBarAction &toolBarAction, ToolBarActions)
			if (toolBarAction.action == after)
			{
				newAction.action = KaduActions.createAction(actionName, dynamic_cast<KaduMainWindow *>(parent()));
				insertAction(after, newAction.action);
				newAction.button = dynamic_cast<QToolButton *>(widgetForAction(newAction.action));
				connect(newAction.button, SIGNAL(pressed()), this, SLOT(buttonPressed()));
				newAction.button->installEventFilter(watcher);
				if (newAction.showLabel)
					newAction.button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
				ToolBarActions.insert(index, newAction);
				return;
			}
			index++;
	}

	newAction.action = KaduActions.createAction(actionName, dynamic_cast<KaduMainWindow *>(parent()));
	QToolBar::addAction(newAction.action);
	newAction.button = dynamic_cast<QToolButton *>(widgetForAction(newAction.action));
	connect(newAction.button, SIGNAL(pressed()), this, SLOT(buttonPressed()));
	newAction.button->installEventFilter(watcher);
	if (newAction.showLabel)
		newAction.button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

	ToolBarActions.append(newAction);
}

void ToolBar::moveAction(const QString &actionName, bool showLabel, QAction *after)
{
 	bool actionFind = false;
	int index = 0;

	ToolBarAction newAction;
	newAction.actionName = actionName;
	newAction.action = 0;
	newAction.button = 0;
	newAction.showLabel = showLabel;

 	foreach(const ToolBarAction &toolBarAction, ToolBarActions)
	{
		if (toolBarAction.actionName == actionName)
		{
			if (toolBarAction.action == after)
				return;
			else
			{
				removeAction(toolBarAction.action);
				ToolBarActions.remove(toolBarAction);
			}

		}
		if (toolBarAction.action != after && !actionFind)
			++index;
		else
			actionFind = true;
	}
 	newAction.action = KaduActions.createAction(actionName, dynamic_cast<KaduMainWindow *>(parent()));

	if (index > ToolBarActions.count() -1)
		QToolBar::addAction(newAction.action);
	else
		insertAction(ToolBarActions[index].action, newAction.action);

	newAction.button = dynamic_cast<QToolButton *>(widgetForAction(newAction.action));
	connect(newAction.button, SIGNAL(pressed()), this, SLOT(buttonPressed()));
	newAction.button->installEventFilter(watcher);
	if (newAction.showLabel)
		newAction.button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	ToolBarActions.insert(index, newAction);
}

void ToolBar::addButtonClicked(QAction *action)
{
	addAction(action->data().toString(), false);
}

void ToolBar::buttonPressed()
{
	MouseStart = mapFromGlobal(QCursor::pos());
}

void ToolBar::mouseMoveEvent(QMouseEvent* e)
{
	if (isMovable() && (e->buttons() & Qt::LeftButton) && (MouseStart - e->pos()).manhattanLength() >= 15)
	{
		QAction *action = actionAt(MouseStart);
		if (!action)
			return;
		foreach(const ToolBarAction &toolBarAction, ToolBarActions)
		{
			if (toolBarAction.action == action)
			{
				QDrag* drag = new ActionDrag(toolBarAction.actionName, toolBarAction.showLabel, this);

				drag->exec(Qt::CopyAction);

				e->accept();
			}	
		}
	}
	else
		QToolBar::mouseMoveEvent(e);
}

void ToolBar::dragEnterEvent(QDragEnterEvent* event)
{
	kdebugf();
	ToolBar* source = dynamic_cast<ToolBar*>(event->source());
	if (source)
	{
		QString actionName;
		bool showLabel;

		if (ActionDrag::decode(event, actionName, showLabel) && (((event->source() == this) || (!hasAction(actionName) && dynamic_cast<KaduMainWindow *>(parent())->supportsActionType(KaduActions[actionName]->type())))))
		{
			event->acceptProposedAction();
			return;
		}
		
	}
	event->ignore();
	kdebugf2();
}

void ToolBar::dropEvent(QDropEvent* event)
{
	kdebugf();
	ToolBar* source = dynamic_cast<ToolBar*>(event->source());

	if (!source)
	{
		event->ignore();
		return;
	}

	QString actionName;
	bool showLabel;
	if (!ActionDrag::decode(event, actionName, showLabel))
	{
		event->ignore();
		return;
	}

	ToolBar* source_toolbar = dynamic_cast<ToolBar*>(event->source());
	QAction *after = actionAt(event->pos());

	if (source_toolbar != this)
	{
		source_toolbar->deleteAction(actionName);
		addAction(actionName, showLabel, after);
	}
	else
		moveAction(actionName, showLabel, after);

	event->acceptProposedAction();

	kdebugf2();
}

void ToolBar::contextMenuEvent(QContextMenuEvent *e)
{
	kdebugf();

// 	if (DockArea::blocked())
// 	{
// 		e->ignore();
// 		return;
// 	}

	QMenu *menu = createContextMenu(dynamic_cast<QToolButton *>(childAt(e->pos())));
	menu->popup(e->globalPos());

// TODO: add something intelligent here
// 	delete menu;

	e->accept();
	kdebugf2();
}

void ToolBar::configurationUpdated()
{
	QDomElement toolbarsConfig = xml_config_file->findElement(xml_config_file->rootElement(), "Toolbars");

	if (toolbarsConfig.isNull())
		return setMovable(false);

	setMovable(!toolbarsConfig.attribute("blocked").toInt());
}

void ToolBar::writeToConfig(QDomElement parent_element)
{
	kdebugf();
	QDomElement toolbar_elem = xml_config_file->createElement(parent_element, "ToolBar");

	toolbar_elem.setAttribute("x_offset", pos().x());
	toolbar_elem.setAttribute("y_offset", pos().y());

	foreach(const ToolBarAction &toolBarAction, ToolBarActions)
	{
		QDomElement button_elem = xml_config_file->createElement(toolbar_elem, "ToolButton");
		button_elem.setAttribute("action_name", toolBarAction.actionName);
		button_elem.setAttribute("uses_text_label", toolBarAction.showLabel);
	}

	kdebugf2();
}

bool ToolBar::hasAction(const QString &action_name)
{
	kdebugf();

	foreach(const ToolBarAction &toolBarAction, ToolBarActions)
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
			(*toolBarAction).button = 0;
			return;
		}
	}
}

void ToolBar::updateButtons()
{
	QAction *lastAction = 0;
	KaduMainWindow *kaduMainWindow = dynamic_cast<KaduMainWindow *>(parent());

	if (!kaduMainWindow)
		return;

	QList<ToolBarAction>::iterator toolBarAction;
	QList<ToolBarAction>::iterator toolBarNextAction;
 	for (toolBarAction = ToolBarActions.begin(), toolBarNextAction = ToolBarActions.begin() + 1; toolBarAction != ToolBarActions.end(); ++toolBarAction, ++toolBarNextAction)
	{
		const QString &actionName = (*toolBarAction).actionName;

		if ((*toolBarAction).action)
		{
			if (KaduActions.contains(actionName))
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

		if (KaduActions.contains(actionName) && kaduMainWindow->supportsActionType(KaduActions[actionName]->type()))
		{
			(*toolBarAction).action = KaduActions.createAction(actionName, dynamic_cast<KaduMainWindow *>(parent()));

			if (toolBarNextAction != ToolBarActions.end() && (*toolBarNextAction).action)
				insertAction((*toolBarNextAction).action, (*toolBarAction).action);
			else
				QToolBar::addAction((*toolBarAction).action);

			(*toolBarAction).button = dynamic_cast<QToolButton *>(widgetForAction((*toolBarAction).action));
			connect((*toolBarAction).button, SIGNAL(pressed()), this, SLOT(buttonPressed()));
			(*toolBarAction).button->installEventFilter(watcher);
			if ((*toolBarAction).showLabel)
				(*toolBarAction).button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

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

	for (QDomNode n = toolbar_element.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		const QDomElement &button_elem = n.toElement();
		if (button_elem.isNull())
			continue;
		if (button_elem.tagName() != "ToolButton")
			continue;

		ToolBarAction action;

		action.actionName = button_elem.attribute("action_name");

		if (hasAction(action.actionName))
			continue;

		if (button_elem.attribute("uses_text_label").isNull())
			action.showLabel = false;
		else
			action.showLabel = button_elem.attribute("uses_text_label") == "1";
		action.action = 0;
		action.button = 0;

		ToolBarActions.append(action);
	}

	updateButtons();

	kdebugf2();
}

QMenu * ToolBar::createContextMenu(QToolButton *button)
{
	QMenu *menu = new QMenu(this);

	if (isMovable())
	{
		currentButton = button;
		if (button)
		{
			QAction *showLabel = menu->addAction(tr("Show text label"), this, SLOT(showTextLabel()));
			showLabel->setCheckable(true);
			showLabel->setChecked(button->toolButtonStyle() != Qt::ToolButtonIconOnly);

			menu->addAction(tr("Delete button"), this, SLOT(deleteButton()));

			menu->addSeparator();
		}

		QMenu *actionsMenu = new QMenu(tr("Add new button"), this);
		foreach(ActionDescription *actionDescription, KaduActions.values())
		{
			bool supportsAction;
			KaduMainWindow *kaduMainWindow= 0;
			if (parent())
				kaduMainWindow = dynamic_cast<KaduMainWindow *>(parent());

			if (kaduMainWindow)
				supportsAction = kaduMainWindow->supportsActionType(actionDescription->type());
			else // TODO is it possible?
				supportsAction = actionDescription->type() == ActionDescription::TypeGlobal;

			if (!supportsAction)
				continue;

			if (!hasAction(actionDescription->name()))
			{
				QAction *action = actionsMenu->addAction(icons_manager->loadIcon(actionDescription->iconName()), actionDescription->text());
				action->setData(actionDescription->name());
			}
		}

		if (actionsMenu->isEmpty())
			actionsMenu->addAction(tr("No items to add found"))->setEnabled(false);
		else
			connect(actionsMenu, SIGNAL(triggered(QAction *)), this, SLOT(addButtonClicked(QAction *)));

		menu->addAction(tr("Delete toolbar"), this, SLOT(deleteToolbar()));
		menu->addMenu(actionsMenu);

		QMainWindow *mainWindow = dynamic_cast<QMainWindow *>(parent());

		menu->addSeparator();
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
	}

	QAction *blockToolbars = menu->addAction(tr("Block toolbars"), this, SLOT(setBlockToolbars(bool)));
	blockToolbars->setCheckable(true);
	blockToolbars->setChecked(!isMovable());

	return menu;
}

void ToolBar::deleteToolbar()
{
	kdebugf();
	if (MessageBox::ask(tr("Remove toolbar?"), "Warning", this))
		deleteLater();
	kdebugf2();
}

void ToolBar::setBlockToolbars(bool checked)
{
	QDomElement toolbarsConfig = xml_config_file->findElement(xml_config_file->rootElement(), "Toolbars");
	if (toolbarsConfig.isNull())
		toolbarsConfig = xml_config_file->createElement(xml_config_file->rootElement(), "Toolbars");

	toolbarsConfig.setAttribute("blocked", checked);
	ConfigurationAwareObject::notifyAll();
}

void ToolBar::showTextLabel()
{
	if (!currentButton)
		return;

	QList<ToolBarAction>::iterator toolBarAction;
 	for (toolBarAction = ToolBarActions.begin(); toolBarAction != ToolBarActions.end(); ++toolBarAction)
	{
		if ((*toolBarAction).button == currentButton)
		{
			if (currentButton->toolButtonStyle() == Qt::ToolButtonIconOnly)
			{
				(*toolBarAction).showLabel = true;
				currentButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
			}
			else
			{
				(*toolBarAction).showLabel = false;
				currentButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
			}
			return;
		}
	}
}

void ToolBar::deleteButton()
{
	if (!currentButton)
		return;

	foreach(const ToolBarAction &toolBarAction, ToolBarActions)
		if (toolBarAction.button == currentButton)
		{
			// TODO: again, lame solution
			removeAction(currentButton->defaultAction());
			currentButton = 0;

			ToolBarActions.remove(toolBarAction);
			return;
		}
}

void ToolBar::deleteAction(const QString &actionName)
{
	foreach(const ToolBarAction &toolBarAction, ToolBarActions)
		if (toolBarAction.actionName == actionName)
		{
			removeAction(toolBarAction.action);
			ToolBarActions.remove(toolBarAction);
			return;
		}

}

ActionDrag::ActionDrag(const QString &actionName, bool showLabel, QWidget* dragSource)
	: QDrag(dragSource)
{
	kdebugf();

	QByteArray data;
	QMimeData *mimeData = new QMimeData;

	QString string = actionName + "\n" + QString::number(showLabel ? 1 : 0);

	data = string.toUtf8();

	mimeData->setData("application/x-kadu-action", data);

	setMimeData(mimeData);

	kdebugf2();
}

bool ActionDrag::decode(QDropEvent *event, QString &actionName, bool &showLabel)
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
	showLabel = tmp;

	return true;
}
