/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtCore/QTimer>
#include <QtWidgets/QFrame>
#include <injeqt/injeqt.h>

class HintsConfiguration;
class Hint;
class InjectedFactory;
struct Notification;

class QVBoxLayout;

class HintsWidget : public QFrame
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit HintsWidget(QWidget *parent = nullptr);
	virtual ~HintsWidget();

	void addNotification(const Notification &notification);

signals:
	void sizeChanged();
	void shown();

protected:
	virtual void resizeEvent(QResizeEvent *) override;
	virtual void showEvent(QShowEvent *) override;

private:
	QPointer<HintsConfiguration> m_hintsConfiguration;
	QPointer<InjectedFactory> m_injectedFactory;

	QTimer m_timer;
	QPointer<QVBoxLayout> m_layout;

	void removeHint(Hint *hint);
	void removeExpiredHints();
	void updateTimer();

private slots:
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setHintsConfiguration(HintsConfiguration *hintsConfiguration);

	void acceptHint(Hint *hint);
	void discardHint(Hint *hint);
	void discardAllHints();
	void hintDestroyed(Hint *hint);
	void updateHints();

};
