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

#include "provider/default-provider.h"
#include "provider/simple-provider.h"
#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class InjectedFactory;
class KaduWindow;

class KADUAPI KaduWindowService : public QObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit KaduWindowService(QObject *parent = nullptr);
	virtual ~KaduWindowService();

	void createWindow();

	KaduWindow * kaduWindow();
	void showMainWindow();

	void setShowMainWindowOnStart(bool showMainWindowOnStart);
	std::shared_ptr<DefaultProvider<QWidget *>> mainWindowProvider() const;

private:
	QPointer<InjectedFactory> m_injectedFactory;

	KaduWindow *m_kaduWindow;
	std::shared_ptr<SimpleProvider<QWidget *>> m_kaduWindowProvider;
	std::shared_ptr<DefaultProvider<QWidget *>> m_mainWindowProvider;
	bool m_showMainWindowOnStart; // TODO: it is a hack for docking

private slots:
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_DONE void done();

	void kaduWindowDestroyed();

};
