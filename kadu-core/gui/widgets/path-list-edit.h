/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtCore/QPointer>
#include <QtWidgets/QPushButton>
#include <injeqt/injeqt.h>

class Configuration;
class IconsManager;
class PathListEditWindow;

class QLineEdit;
class QListWidget;

class PathListEdit : public QPushButton
{
	Q_OBJECT

public:
	PathListEdit(QWidget *parent = nullptr);
	virtual ~PathListEdit() {}

	const QStringList & pathList() const { return PathList; }
	void setPathList(const QStringList &pathList);

signals:
	void changed();

private:
	QPointer<PathListEditWindow> Dialog;
	QStringList PathList;

private slots:
	void showDialog();
	void pathListChanged(const QStringList &pathList);

};

class PathListEditWindow : public QWidget
{
	Q_OBJECT

public:
	explicit PathListEditWindow(const QStringList &pathList, QWidget *parent = nullptr);
	~PathListEditWindow();

public slots:
	void setPathList(const QStringList &list);

signals:
	void changed(const QStringList &paths);

private:
	QPointer<Configuration> m_configuration;
	QPointer<IconsManager> m_iconsManager;
	QStringList PathList;

	QListWidget *PathListWidget;

	void createGui();
	bool validatePath(QString &path);

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_INIT void init();

	void addPathClicked();
	void changePathClicked();
	void deletePathClicked();

	void okClicked();

	void keyPressEvent(QKeyEvent *e);

};
