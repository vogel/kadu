/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtWidgets/QWidget>

class QLabel;

class SelectFile;
class SoundManager;

class SoundSelectFile : public QWidget
{
	Q_OBJECT

public:
	explicit SoundSelectFile(SoundManager *manager, QWidget *parent = nullptr);
	virtual ~SoundSelectFile();

	QString file() const;
	void setFile(const QString &file);

	void stopSound();

signals:
	void fileChanged();

private:
	QPointer<SoundManager> m_manager;
	QPointer<QObject> m_sound;
	SelectFile *m_selectFile;

private slots:
	void test();

};
