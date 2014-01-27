/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2005, 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
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

#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QtGui/QDrag>
#include <QtGui/QToolBar>

#include "configuration/configuration-aware-object.h"
#include "misc/change-notifier.h"

#include "exports.h"

class QDomElement;
class QMenu;
class QToolButton;

class ToolBarSeparator;
class ToolBarSpacer;

#define TOOLBAR_SEPARATOR_SIZE 12 /*px*/

struct ToolBarDropMarker
{
	bool visible;
	int x;
	int y;
	int size;
	bool operator==(const ToolBarDropMarker &other) const
	{
		if (visible != other.visible) return false;
		if (x != other.x) return false;
		if (y != other.y) return false;
		if (size != other.size) return false;
		return true;
	}
	bool operator!=(const ToolBarDropMarker &other) const
	{
		return *this == other ? false : true;
	}
};

/**
	Klasa tworząca pasek narzędziowy
	\class ToolBar
	\brief Pasek narzędziowy
**/

class KADUAPI ToolBar : public QToolBar, public ConfigurationAwareObject
{
	Q_OBJECT

	friend class DisabledActionsWatcher;

	ChangeNotifier MyChangeNotifier;

	// TODO: ugly hack
	QWidget *currentWidget;
	QAction *IconsOnly, *TextOnly, *Text, *TextUnder;

	struct ToolBarAction {
		QString actionName;
		QAction *action;
		QWidget *widget;
		Qt::ToolButtonStyle style;
		bool operator == (struct ToolBarAction action) const {
			return actionName == action.actionName;
		}
	};

	QList<ToolBarAction> ToolBarActions;

	static QMap<QString, QList<ToolBarAction> > DefaultActions;

	// TODO: remove, used only when reading from config
	int XOffset;
	int YOffset;

	QPoint MouseStart;

	Qt::ToolBarArea toolBarArea();
	Qt::Orientation orientationByArea(Qt::ToolBarArea toolbararea);

	bool dragging;

	QAction * findActionToDropBefore(const QPoint &pos);
	QAction * actionNear(const QPoint &pos);
	int rowCount();
	int rowAt(const QPoint &pos);
	QRect rowRect(int row);
	QList<QAction *> actionsForRow(int row);
	int actionRow(QAction *action);

	ToolBarDropMarker dropmarker;
	void updateDropMarker();

	void addAction(const QString &actionName, Qt::ToolButtonStyle style, QAction *before = 0);

	int indexOf(QAction *action);
	int indexOf(const QString &action);

	ToolBarSeparator * createSeparator(QAction *before, ToolBarAction &action);
	ToolBarSpacer * createSpacer(QAction *before, ToolBarAction &action);
	QToolButton * createPushButton(QAction *before, ToolBarAction &action);
	QWidget * createActionWidget(QAction *before, ToolBarAction &action);

	QMenu * createContextMenu(QWidget *widget);

	void paintDropMarker();

private slots:
	/**
		\fn void addButtonClicked()
		Slot dodający wybrany przycisk
	**/
	void addButtonClicked(QAction *action);
	void removeButton();

	/**
		\fn void addSeparatorClicked()
		Slot dodający separator
	**/
	void addSeparatorClicked();

	/**
		\fn void addSpacerClicked()
		Slot dodający swobodny odstęp
	**/
	void addSpacerClicked();

	void removeSeparator();
	void removeSpacer();

	/**
		\fn void removeToolbar()
		Slot obsługujący usuwanie paska narzędzi
	**/
	void removeToolbar();

	void setBlockToolbars(bool checked);

	void widgetPressed();

	void slotContextIcons();
	void slotContextText();
	void slotContextTextUnder();
	void slotContextTextRight();

	void slotContextAboutToShow();

protected:
	/**
		\fn virtual void dragEnterEvent(QDragEnterEvent* event)
		Funkcja obsługująca przeciąganie akcji między paskami
	**/
	virtual void dragEnterEvent(QDragEnterEvent *event);

	/**
		\fn virtual void dropEvent(QDropEvent* event)
		Funkcja obsługująca upuszczenie przycisku na pasku
	**/
	virtual void dropEvent(QDropEvent *event);

	virtual bool event(QEvent *event);

	virtual void dragLeaveEvent(QDragLeaveEvent *event);
	virtual void leaveEvent(QEvent *event);

	virtual void paintEvent(QPaintEvent *event);

	virtual void dragMoveEvent(QDragMoveEvent *event);

	/**
		\fn virtual void contextMenuEvent(QContextMenuEvent* e)
		Funkcja obsługująca tworzenie menu kontekstowego paska
	**/
	virtual void contextMenuEvent(QContextMenuEvent *e);

	virtual void mouseMoveEvent(QMouseEvent *e);

	virtual void configurationUpdated();

public:
	static bool isBlockToolbars();

	/**
		Konstruktor paska narzędzi
		\fn ToolBar(QWidget* parent, const char *name)
		\param parent rodzic obiektu
		\param name nazwa obiektu
	**/
	explicit ToolBar(QWidget *parent);

	/**
		\fn ~ToolBar()
		Destruktor paska narzędzi
	**/
	virtual ~ToolBar();

 	void deleteAction(const QString &actionName);
	void moveAction(const QString &actionName, Qt::ToolButtonStyle style, QAction *before);

	/**
		\fn void loadFromConfig(QDomElement parent_element)
		\param parent_element rodzic obiektu
		Wczytuje dane z pliku konfiguracyjnego
	**/
	void loadFromConfig(const QDomElement &parent_element);

	/**
		\fn hasAction(QString action_name)
		\param action_name nazwa szukanej akcji
		Funkcja zwraca wartość boolowską, okreslającą, czy akcja
		o podanej nazwie znajduje się już na pasku narzędzi.
	**/
	bool hasAction(const QString &action_name);
	bool windowHasAction(const QString &action_name, bool exclude);

	int xOffset() { return XOffset; }
	int yOffset() { return YOffset; }

public slots:
	/**
		\fn writeToConfig(QDomElement parent_element)
		\param parent_element rodzic obiektu
		Zapisuje ustawienia paska (jak offset), oraz (pośrednio)
		akcje znajdujące się na pasku.
	**/
	void writeToConfig(const QDomElement &parent_element);

signals:
	void updated();
	void removed(ToolBar *toolbar);

};

class KADUAPI ActionDrag : public QDrag
{
	Q_OBJECT

public:
	static bool decode(QDropEvent *event, QString &actionName, Qt::ToolButtonStyle &style);

	ActionDrag(const QString &actionName, Qt::ToolButtonStyle style, QWidget *dragSource = 0);
};

class ToolBarSeparator : public QWidget
{
	Q_OBJECT

	static int Token;

protected:
	virtual void mousePressEvent(QMouseEvent *event);

public:
	static int token();

	explicit ToolBarSeparator(QWidget *parent = 0);

signals:
	void pressed();

};

class ToolBarSpacer : public QWidget
{
	Q_OBJECT

	static int Token;

protected:
	virtual void mousePressEvent(QMouseEvent *event);

public:
	static int token();

	explicit ToolBarSpacer(QWidget *parent = 0);

signals:
	void pressed();

};

#endif // TOOLBAR_H
