/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qcursor.h>
#include <qobjectlist.h>
#include <qtooltip.h>

#include "chat.h" // TODO: akcje powinny byæ niezale¿ne od chat
#include "config_file.h"
#include "debug.h"
#include "icons_manager.h"
#include "kadu.h"
#include "misc.h"
#include "toolbar.h"


ToolButton::ToolButton(QWidget* parent, const QString& action_name)
	: QToolButton(parent, 0)
{
	ActionName = action_name;
}

void ToolButton::mouseMoveEvent(QMouseEvent* e)
{
//	kdebugf();
	QToolButton::mouseMoveEvent(e);
	if (e->state() & LeftButton)
	{
		QDragObject* d = new ToolButtonDrag(this, parentWidget());
		d->dragMove();
	}	
//	kdebugf2();
}

void ToolButton::contextMenuEvent(QContextMenuEvent* e)
{
	kdebugf();
	QPopupMenu* p = new QPopupMenu(this);
	p->insertItem(tr("Delete button"), this, SLOT(deleteLater()));
	p->exec(QCursor::pos());
	delete p;
	e->accept();
	kdebugf2();
}

void ToolButton::writeToConfig(QDomElement parent_element)
{
	kdebugf();
	QDomElement button_elem = xml_config_file->createElement(parent_element, "ToolButton");
	button_elem.setAttribute("action_name", ActionName);
	kdebugf2();
}

ToolButtonDrag::ToolButtonDrag(ToolButton* button, QWidget* dragSource, const char* name)
	: QTextDrag(button->name(), dragSource, name)
{
}

ToolBar::ToolBar(const QString& label, QMainWindow* mainWindow, QWidget* parent)
	: QToolBar(label, mainWindow, parent)
{
	kdebugf();
	setAcceptDrops(true);
	kdebugf2();
}

void ToolBar::addButtonClicked(int action_index)
{
	kdebugf();
	kdebug("action_index = %d\n", action_index);
	KaduActions[KaduActions.keys()[action_index]]->addToToolbar(this);
	kdebugf2();
}

void ToolBar::dragEnterEvent(QDragEnterEvent* event)
{
	kdebugf();
	event->accept(dynamic_cast<ToolBar*>(event->source()) != NULL);
	kdebugf2();
}

void ToolBar::dropEvent(QDropEvent* event)
{
	kdebugf();
	ToolBar* source = dynamic_cast<ToolBar*>(event->source());
	if (source != NULL)
	{
		QString text;
		if (QTextDrag::decode(event, text))
		{
			dynamic_cast<ToolButton*>(source->child(text))->reparent(this, QPoint(0,0), true);
		}
	}
	kdebugf2();
}

void ToolBar::contextMenuEvent(QContextMenuEvent* e)
{
	kdebugf();
	QPopupMenu* p = new QPopupMenu(this);
	p->insertItem(tr("Delete toolbar"), this, SLOT(deleteLater()));
	QPopupMenu* p2 = new QPopupMenu(p);
	int param = 0;
	CONST_FOREACH(a, KaduActions)
	{
		int id = (*a)->addToPopupMenu(p2, false);
		p2->setItemParameter(id, param);
		p2->connectItem(id, this, SLOT(addButtonClicked(int)));
		param++;
	}
	p->insertItem(tr("Add new button"), p2);
	p->exec(QCursor::pos());
	delete p;
	e->accept();
	kdebugf2();
}

void ToolBar::writeToConfig(QDomElement parent_element)
{
	kdebugf();
	QDomElement toolbar_elem = xml_config_file->createElement(
		parent_element, "ToolBar");
	toolbar_elem.setAttribute("offset", offset());
	QObjectList* l = queryList("ToolButton");
	for (QObjectList::iterator i = l->begin(); i != l->end(); i++)
		((ToolButton*)(*i))->writeToConfig(toolbar_elem);
	kdebugf2();
}


DockArea::DockArea(Orientation o, HandlePosition h,
			QWidget * parent, const char * name)
	: QDockArea(o, h, parent, name)
{
	kdebugf();
	kdebugf2();
}

void DockArea::contextMenuEvent(QContextMenuEvent* e)
{
	kdebugf();
	QPopupMenu* p = new QPopupMenu(this);
	p->insertItem(tr("Create new toolbar"), this, SLOT(createNewToolbar()));
	p->exec(QCursor::pos());
	delete p;
	e->accept();
	kdebugf2();
}

void DockArea::createNewToolbar()
{
	kdebugf();
	for (QWidget* w = parentWidget(); w != NULL; w = w->parentWidget())
	{
		QMainWindow* mw = dynamic_cast<QMainWindow*>(w);
		if (mw != NULL)
		{
			kdebug("Creating new toolbar\n");
			ToolBar* tb = new ToolBar("New toolbar", mw, mw);
			tb->show();
			moveDockWindow(tb);
			setAcceptDockWindow(tb, true);
			writeToConfig();
			break;
		}
	}
	kdebugf2();
}

void DockArea::writeToConfig()
{
	kdebugf();
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement toolbars_elem = xml_config_file->accessElement(root_elem, "Toolbars");
	QDomElement dockarea_elem = xml_config_file->accessElementByProperty(
		toolbars_elem, "DockArea", "name", name());
	xml_config_file->removeChildren(dockarea_elem);
	QPtrList<QDockWindow> dock_windows = dockWindowList();
	for (QPtrList<QDockWindow>::iterator i = dock_windows.begin();
		i != dock_windows.end(); i++)
	{
		ToolBar* toolbar = dynamic_cast<ToolBar*>(*i);
		if (toolbar != NULL)
			toolbar->writeToConfig(dockarea_elem);
	}
	kdebugf2();
}

Action::Action(const QIconSet& icon, const QString& text, const char* name, QKeySequence accel)
	: QAction(icon, text, accel, kadu, name)
{
	kdebugf();
	kdebugf2();
}

void Action::toolButtonClicked()
{
	kdebugf();
	const ToolButton* button = dynamic_cast<const ToolButton*>(sender());
	for (QWidget* w = button->parentWidget(); w != NULL; w = w->parentWidget())
	{
		Chat* c = dynamic_cast<Chat*>(w);
		if (c != NULL)
			emit activated(c->users(), button, button->isOn());
	}
	kdebugf2();
}

void Action::toolButtonDestroyed(QObject* obj)
{
	kdebugf();
	ToolButton* btn = static_cast<ToolButton*>(obj);
	QValueList<ToolButton*>::iterator it = ToolButtons.find(btn);
	ToolButtons.remove(it);
	kdebugf2();
}

ToolButton* Action::addToToolbar(ToolBar* toolbar)
{
	kdebugf();
	ToolButton* btn = new ToolButton(toolbar, name());
	btn->setIconSet(iconSet());
	btn->setToggleButton(isToggleAction());
	QToolTip::add(btn, menuText());
	connect(btn, SIGNAL(clicked()), this, SLOT(toolButtonClicked()));
	connect(btn, SIGNAL(destroyed(QObject*)), this, SLOT(toolButtonDestroyed(QObject*)));
	ToolButtons.append(btn);
	UserListElements user_list_elems;
	for (QWidget* w = toolbar->parentWidget(); w != NULL; w = w->parentWidget())
	{
		Chat* c = dynamic_cast<Chat*>(w);
		if (c != NULL)
		{
			user_list_elems = c->users()->toUserListElements();
			break;
		}
	}
	btn->setOn(isOn(user_list_elems));
	emit addedToToolbar(btn, toolbar, user_list_elems);
	kdebugf2();
	return btn;
}

int Action::addToPopupMenu(QPopupMenu* menu, bool connect_signal)
{
	kdebugf();
	int id = menu->insertItem(iconSet(), menuText());
	if (connect_signal)
		menu->connectItem(id, this, SIGNAL(activated()));
	kdebugf2();
	return id;
}

QValueList<ToolButton*> Action::toolButtonsForUserListElements(const UserListElements& users)
{
	kdebugf();
	QValueList<ToolButton*> buttons;
	for (QValueList<ToolButton*>::iterator i = ToolButtons.begin(); i != ToolButtons.end(); i++)
	{
		for (QWidget* w = (*i)->parentWidget(); w != NULL; w = w->parentWidget())
		{
			Chat* c = dynamic_cast<Chat*>(w);
			if (c != NULL)
			{
				if (c->users()->toUserListElements().equals(users))
					buttons.append(*i);
			}
		}
	}
	kdebugf2();
	return buttons;
}

bool Action::isOn(const UserListElements& users)
{
	kdebugf();
	for (QValueList<ToggleStateStruct>::iterator i = ToggleState.begin(); i != ToggleState.end(); i++)
		if ((*i).elems == users)
		{
			kdebug("state: %i\n", (*i).state);
			kdebugf2();
			return (*i).state;
		}
	return false;
	kdebugf2();
}

void Action::setOn(const UserListElements& users, bool on)
{
	kdebugf();
	QValueList<ToolButton*> buttons = toolButtonsForUserListElements(users);
	for (QValueList<ToolButton*>::iterator i = buttons.begin(); i != buttons.end(); i++)
		(*i)->setOn(on);
	for (QValueList<ToggleStateStruct>::iterator i = ToggleState.begin(); i != ToggleState.end(); i++)
		if ((*i).elems == users)
		{
			(*i).state = on;
			return;
		}
	ToggleStateStruct s;
	s.elems = users;
	s.state = on;
	ToggleState.push_back(s);
	kdebugf2();
}

void Action::setPixmaps(const UserListElements& users, const QPixmap& pixmap)
{
	kdebugf();
	QValueList<ToolButton*> buttons = toolButtonsForUserListElements(users);
	for (QValueList<ToolButton*>::iterator i = buttons.begin(); i != buttons.end(); i++)
		(*i)->setPixmap(pixmap);
	kdebugf2();
}

Actions::Actions()
{
}

Actions KaduActions;


QValueList<MainToolBar::ToolButton> MainToolBar::RegisteredToolButtons;
MainToolBar* MainToolBar::instance=NULL;

MainToolBar::MainToolBar(QMainWindow* parent) : QToolBar(parent, "mainToolbar")
{
	kdebugf();
	setCloseMode(QDockWindow::Undocked);
	setLabel(qApp->translate("ToolBar", "Main toolbar"));

	config_file.addVariable("General", "ToolBarHidden", false);
	if (config_file.readBoolEntry("General", "ToolBarHidden"))
		hide();

	setVerticallyStretchable(true);
	setHorizontallyStretchable(true);

	createControls();
	instance=this;
	kdebugf2();
}

MainToolBar::~MainToolBar()
{
	config_file.writeEntry("General", "ToolBarHidden", isHidden());
	instance=NULL;
}

void MainToolBar::createControls()
{
	kdebugf();
	FOREACH(j, RegisteredToolButtons)
		if ((*j).caption== "--separator--")
			addSeparator();
		else
			(*j).button = new QToolButton(icons_manager->loadIcon((*j).iconname), (*j).caption,
				QString::null, (*j).receiver, (*j).slot, this, (*j).name);

	setStretchableWidget(new QWidget(this));
	kdebugf2();
}

void MainToolBar::registerSeparator(int position)
{
	kdebugf();
	if(instance!=NULL)
		instance->clear();

	ToolButton RToolButton;
	RToolButton.caption="--separator--";

	if ((RegisteredToolButtons.count()<(uint)(position+1)) || (position == -1))
		RegisteredToolButtons.append(RToolButton);
	else
		RegisteredToolButtons.insert(RegisteredToolButtons.at(position), RToolButton);

	if(instance!=NULL)
		instance->createControls();
	kdebugf2();
}

void MainToolBar::registerButton(const QString &iconname, const QString& caption,
			QObject* receiver, const char* slot, int position, const char* name)
{
	kdebugf();
	if(instance!=NULL)
		instance->clear();

	ToolButton RToolButton;

	RToolButton.iconname= iconname;
	RToolButton.caption= caption;
	RToolButton.receiver= receiver;
	RToolButton.slot= slot;
	RToolButton.position= position;
	RToolButton.name= name;

	if ((RegisteredToolButtons.count()<(uint)(position+1)) || (position == -1))
		RegisteredToolButtons.append(RToolButton);
	else
		RegisteredToolButtons.insert(RegisteredToolButtons.at(position), RToolButton);

	if(instance!=NULL)
		instance->createControls();
	kdebugf2();
}

void MainToolBar::unregisterButton(const char* name)
{
	kdebugf();
	if(instance!=NULL)
		instance->clear();

	FOREACH(j, RegisteredToolButtons)
		if ((*j).name == name)
		{
			RegisteredToolButtons.remove(j);
			break;
		}

	if(instance!=NULL)
		instance->createControls();
	kdebugf2();
}

QToolButton* MainToolBar::getButton(const char* name)
{
	CONST_FOREACH(j, RegisteredToolButtons)
		if ((*j).name == name)
			return (*j).button;
	kdebugmf(KDEBUG_WARNING, "'%s' return NULL\n", name?name:"[null]");
	return NULL;
}

void MainToolBar::refreshIcons(const QString &caption, const QString &newIconName, const QString &newCaption)
{
	kdebugf();
	if (caption==QString::null) //wszystkie siê od¶wie¿aj±
	{
		FOREACH(j, RegisteredToolButtons)
			if ((*j).caption!="--separator--")
				(*j).button->setIconSet(icons_manager->loadIcon((*j).iconname));
		if (kadu->isVisible())
		{
			kadu->hide();
			kadu->show();
		}
	}
	else
		FOREACH(j, RegisteredToolButtons)
			if ((*j).caption == caption)
			{
				if (newIconName!=QString::null)
					(*j).iconname=newIconName;
				(*j).button->setIconSet(icons_manager->loadIcon((*j).iconname));
				if (newCaption!=QString::null)
				{
					(*j).caption=newCaption;
					(*j).button->setTextLabel(newCaption);
				}
				break;
			}
	kdebugf2();
}
