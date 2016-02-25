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

#include "hints-widget.h"

#include "hints-configuration.h"
#include "hint.h"

#include "core/injected-factory.h"
#include "misc/memory.h"

#include <QtWidgets/QVBoxLayout>

HintsWidget::HintsWidget(QWidget *parent) :
		QFrame{parent, Qt::FramelessWindowHint | Qt::Tool | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint | Qt::MSWindowsOwnDC}
{
	setObjectName(QStringLiteral("hints"));
	setStyleSheet(QStringLiteral("#hints { border: 1px solid %1; }").arg(palette().window().color().darker().name()));

	m_layout = make_owned<QVBoxLayout>(this);
	m_layout->setSpacing(0);
	m_layout->setMargin(0);

}

HintsWidget::~HintsWidget()
{
}

void HintsWidget::setHintsConfiguration(HintsConfiguration *hintsConfiguration)
{
	m_hintsConfiguration = hintsConfiguration;
}

void HintsWidget::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void HintsWidget::addNotification(const Notification &notification)
{
	auto hint = m_injectedFactory->makeOwned<Hint>(notification, m_hintsConfiguration, this);

	connect(hint, &Hint::leftButtonClicked, this, &HintsWidget::acceptHint);
	connect(hint, &Hint::rightButtonClicked, this, &HintsWidget::discardHint);
	connect(hint, &Hint::midButtonClicked, this, &HintsWidget::discardAllHints);

	m_layout->addWidget(hint);
	adjustSize();

	show();
}

void HintsWidget::removeHint(Hint *hint)
{
	m_layout->removeWidget(hint);
	hint->deleteLater();

	if (m_layout->isEmpty())
		hide();
	else
		adjustSize();
}

void HintsWidget::resizeEvent(QResizeEvent *re)
{
	QWidget::resizeEvent(re);
	emit sizeChanged();
}

void HintsWidget::showEvent(QShowEvent *se)
{
	QWidget::showEvent(se);
	emit shown();
}

void HintsWidget::acceptHint(Hint *hint)
{
	hint->acceptNotification();
	removeHint(hint);
}

void HintsWidget::discardHint(Hint *hint)
{
	hint->discardNotification();
	removeHint(hint);
}

void HintsWidget::discardAllHints()
{
	while (!m_layout->isEmpty())
		removeHint(static_cast<Hint *>(*m_layout->children().begin()));
}

#include "moc_hints-widget.cpp"
