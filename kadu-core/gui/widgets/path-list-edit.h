/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef PATH_LIST_EDIT_H
#define PATH_LIST_EDIT_H

#include <QtGui/QPushButton>

class QLineEdit;
class QListWidget;

class PathListEditWindow;

class PathListEdit : public QPushButton
{
	Q_OBJECT

	PathListEditWindow *Dialog;
	QStringList PathList;

private slots:
	void showDialog();
	void dialogDestroyed();
	void pathListChanged(const QStringList &pathList);

public:
	PathListEdit(QWidget *parent = 0);
	virtual ~PathListEdit() {}

	const QStringList & pathList() const { return PathList; }
	void setPathList(const QStringList &pathList);

signals:
	void changed();

};

class PathListEditWindow : public QWidget
{
	Q_OBJECT

	QListWidget *PathListWidget;

	bool validatePath(QString &path);

private slots:
	void addPathClicked();
	void changePathClicked();
	void deletePathClicked();

	void okClicked();

	void closeEvent(QCloseEvent *e);
	void keyPressEvent(QKeyEvent *e);

public:
	explicit PathListEditWindow(const QStringList &pathList, QWidget *parent = 0);
	~PathListEditWindow();

public slots:
	void setPathList(const QStringList &list);

signals:
	void changed(const QStringList &paths);

};

#endif // PATH_LIST_EDIT_H
