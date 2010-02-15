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
	setIconSize(IconsManager::instance()->getIconsSize());

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

void ToolBar::addAction(const QString &actionName, Qt::ToolButtonStyle style, QAction *after)
{
	if (hasAction(actionName))
		return;

	ToolBarAction newAction;
	newAction.actionName = actionName;
	newAction.action = 0;
	newAction.button = 0;
	newAction.style = style;

	if (after)
	{
		int index = 0;

		foreach(const ToolBarAction &toolBarAction, ToolBarActions)
			if (toolBarAction.action == after)
			{
				newAction.action = KaduActions.createAction(actionName, dynamic_cast<MainWindow *>(parent()));
				insertAction(after, newAction.action);
				newAction.button = dynamic_cast<QToolButton *>(widgetForAction(newAction.action));
				connect(newAction.button, SIGNAL(pressed()), this, SLOT(buttonPressed()));
				newAction.button->installEventFilter(watcher);
				newAction.button->setToolButtonStyle(newAction.style);
				ToolBarActions.insert(index, newAction);
				return;
			}
			index++;
	}

	newAction.action = KaduActions.createAction(actionName, dynamic_cast<MainWindow *>(parent()));
	QToolBar::addAction(newAction.action);
	newAction.button = dynamic_cast<QToolButton *>(widgetForAction(newAction.action));
	connect(newAction.button, SIGNAL(pressed()), this, SLOT(buttonPressed()));
	newAction.button->installEventFilter(watcher);
	newAction.button->setToolButtonStyle(newAction.style);

	ToolBarActions.append(newAction);
}

void ToolBar::moveAction(const QString &actionName, Qt::ToolButtonStyle style, QAction *after)
{
 	bool actionFind = false;
	int index = 0;

	ToolBarAction newAction;
	newAction.actionName = actionName;
	newAction.action = 0;
	newAction.button = 0;
	newAction.style = style;

 	foreach(const ToolBarAction &toolBarAction, ToolBarActions)
	{
		if (toolBarAction.actionName == actionName)
		{
			if (toolBarAction.action == after)
				return;
			else
			{
				removeAction(toolBarAction.action);
				ToolBarActions.removeAll(toolBarAction);
			}

		}
		if (toolBarAction.action != after && !actionFind)
			++index;
		else
			actionFind = true;
	}
 	newAction.action = KaduActions.createAction(actionName, dynamic_cast<MainWindow *>(parent()));

	if (index > ToolBarActions.count() -1)
		QToolBar::addAction(newAction.action);
	else
		insertAction(ToolBarActions[index].action, newAction.action);

	newAction.button = dynamic_cast<QToolButton *>(widgetForAction(newAction.action));
	connect(newAction.button, SIGNAL(pressed()), this, SLOT(buttonPressed()));
	newAction.button->installEventFilter(watcher);
	newAction.button->setToolButtonStyle(newAction.style);
	ToolBarActions.insert(index, newAction);
}

void ToolBar::addButtonClicked(QAction *action)
{
	addAction(action->data().toString(), Qt::ToolButtonIconOnly);
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
				QDrag* drag = new ActionDrag(toolBarAction.actionName, toolBarAction.style, this);

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
		Qt::ToolButtonStyle style;

		if (ActionDrag::decode(event, actionName, style) && (((event->source() == this)
		    || (!hasAction(actionName) && dynamic_cast<MainWindow *>(parent())->supportsActionType(KaduActions[actionName]->type())))))
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
	Qt::ToolButtonStyle style;
	if (!ActionDrag::decode(event, actionName, style))
	{
		event->ignore();
		return;
	}

	ToolBar* source_toolbar = dynamic_cast<ToolBar*>(event->source());
	QAction *after = actionAt(event->pos());

	if (source_toolbar != this)
	{
		source_toolbar->deleteAction(actionName);
		addAction(actionName, style, after);
	}
	else
		moveAction(actionName, style, after);

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
	setIconSize(IconsManager::instance()->getIconsSize());
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
		button_elem.setAttribute("toolbutton_style", toolBarAction.style);
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
			(*toolBarAction).action = KaduActions.createAction(actionName, dynamic_cast<MainWindow *>(parent()));

			if (toolBarNextAction != ToolBarActions.end() && (*toolBarNextAction).action)
				insertAction((*toolBarNextAction).action, (*toolBarAction).action);
			else
				QToolBar::addAction((*toolBarAction).action);

			(*toolBarAction).button = dynamic_cast<QToolButton *>(widgetForAction((*toolBarAction).action));
			connect((*toolBarAction).button, SIGNAL(pressed()), this, SLOT(buttonPressed()));
			(*toolBarAction).button->installEventFilter(watcher);
			(*toolBarAction).button->setToolButtonStyle((*toolBarAction).style);

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

		if (hasAction(action.actionName))
			continue;
	//remove after 0.7.0
		if (button_elem.attribute("toolbutton_style").isNull())
			action.style = button_elem.attribute("uses_text_label") == "1"
					? Qt::ToolButtonTextBesideIcon : Qt::ToolButtonIconOnly;
		else
		{
			toolButtonStyle = button_elem.attribute("toolbutton_style");
			action.style = Qt::ToolButtonStyle(toolButtonStyle.value<int>());
		}

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
	connect(menu, SIGNAL(aboutToShow()), this, SLOT(slotContextAboutToShow()));

	if (isMovable())
	{
		currentButton = button;
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

			menu->addAction(tr("Delete button"), this, SLOT(deleteButton()));

			menu->addSeparator();
		}

		QMenu *actionsMenu = new QMenu(tr("Add new button"), this);
		foreach(ActionDescription *actionDescription, KaduActions.values())
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
	if (MessageDialog::ask(tr("Remove toolbar?"), "32x32/dialog-warning.png", this))
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

void ToolBar::deleteButton()
{
	if (!currentButton)
		return;

	foreach (const ToolBarAction &toolBarAction, ToolBarActions)
		if (toolBarAction.button == currentButton)
		{
			// TODO: again, lame solution
			removeAction(currentButton->defaultAction());
			currentButton = 0;

			ToolBarActions.removeAll(toolBarAction);
			return;
		}
}

void ToolBar::deleteAction(const QString &actionName)
{
	foreach (const ToolBarAction &toolBarAction, ToolBarActions)
		if (toolBarAction.actionName == actionName)
		{
			removeAction(toolBarAction.action);
			ToolBarActions.removeAll(toolBarAction);
			return;
		}

}

void ToolBar::slotContextIcons()
{
    	if (!currentButton)
		return;

	QList<ToolBarAction>::iterator toolBarAction;
	for (toolBarAction = ToolBarActions.begin(); toolBarAction != ToolBarActions.end(); ++toolBarAction)
	{
		if ((*toolBarAction).button == currentButton)
		{
			(*toolBarAction).style = Qt::ToolButtonIconOnly;
			(*toolBarAction).button->setToolButtonStyle(Qt::ToolButtonIconOnly);
			return;
		}
	}
}

void ToolBar::slotContextText()
{
	if (!currentButton)
		return;

	QList<ToolBarAction>::iterator toolBarAction;
	for (toolBarAction = ToolBarActions.begin(); toolBarAction != ToolBarActions.end(); ++toolBarAction)
	{
		if ((*toolBarAction).button == currentButton)
		{
			(*toolBarAction).style = Qt::ToolButtonTextOnly;
			(*toolBarAction).button->setToolButtonStyle(Qt::ToolButtonTextOnly);
			return;
		}
	}
}
 
void ToolBar::slotContextTextUnder()
{
	if (!currentButton)
		return;

	QList<ToolBarAction>::iterator toolBarAction;
	for (toolBarAction = ToolBarActions.begin(); toolBarAction != ToolBarActions.end(); ++toolBarAction)
	{
		if ((*toolBarAction).button == currentButton)
		{
			(*toolBarAction).style = Qt::ToolButtonTextUnderIcon;
			(*toolBarAction).button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
			return;
		}
	}
}
 
void ToolBar::slotContextTextRight()
{
	if (!currentButton)
		return;

	QList<ToolBarAction>::iterator toolBarAction;
	for (toolBarAction = ToolBarActions.begin(); toolBarAction != ToolBarActions.end(); ++toolBarAction)
	{
		if ((*toolBarAction).button == currentButton)
		{
			(*toolBarAction).style = Qt::ToolButtonTextBesideIcon;
			(*toolBarAction).button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
			return;
		}
	}
}

void ToolBar::slotContextAboutToShow()
{
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

ActionDrag::ActionDrag(const QString &actionName, Qt::ToolButtonStyle style, QWidget* dragSource)
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
