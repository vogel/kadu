/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Kermit (plaza.maciej@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDesktopWidget>
#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>

#include "buddies/buddy-preferred-manager.h"
#include "configuration/config-file-data-manager.h"
#include "core/core.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "notify/chat-notification.h"

#include "hint-over-user-configuration-window.h"

#include "activate.h"

#include "hints-configuration-ui-handler.h"

HintsConfigurationUiHandler::HintsConfigurationUiHandler(const QString &style, QObject *parent):
	AdvancedWindow(0), overUserConfigurationWindow(0)
{
#ifdef Q_OS_MAC
	previewHintsFrame = new QFrame(dynamic_cast<QWidget *>(parent), Qt::FramelessWindowHint | Qt::SplashScreen | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint |Qt::MSWindowsOwnDC);
#else
	previewHintsFrame = new QFrame(dynamic_cast<QWidget *>(parent), Qt::FramelessWindowHint | Qt::Tool | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint |Qt::MSWindowsOwnDC);
#endif
	previewHintsFrame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	previewHintsLayout = new QVBoxLayout(previewHintsFrame);
	previewHintsLayout->setSpacing(0);
	previewHintsLayout->setMargin(1);
	previewHintsLayout->setSizeConstraint(QLayout::SetFixedSize);
	previewHintsFrame->setStyleSheet(style);

	connect(this, SIGNAL(searchingForTrayPosition(QPoint &)), Core::instance(), SIGNAL(searchingForTrayPosition(QPoint &)));
}

HintsConfigurationUiHandler::~HintsConfigurationUiHandler()
{
	delete previewHintsFrame;
	previewHintsFrame = 0;
}

void HintsConfigurationUiHandler::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow, SIGNAL(destroyed(QObject *)), this, SLOT(mainConfigurationWindowDestroyed()));

	connect(mainConfigurationWindow->widget()->widgetById("hints/advanced"), SIGNAL(clicked()), this, SLOT(showAdvanced()));

	connect(mainConfigurationWindow->widget()->widgetById("hints/showContent"), SIGNAL(toggled(bool)),
	mainConfigurationWindow->widget()->widgetById("hints/showContentCount"), SLOT(setEnabled(bool)));

	connect(mainConfigurationWindow->widget()->widgetById("toolTipClasses"), SIGNAL(currentIndexChanged(const QString &)),
		this, SLOT(toolTipClassesHighlighted(const QString &)));

	ConfigGroupBox *toolTipBox = mainConfigurationWindow->widget()->configGroupBox("Look", "Buddy List", "General");

	QWidget *configureHint = new QWidget(toolTipBox->widget());
	overUserConfigurationPreview = new QFrame(configureHint);
	QHBoxLayout *lay = new QHBoxLayout(overUserConfigurationPreview);
	lay->setMargin(10);
	lay->setSizeConstraint(QLayout::SetFixedSize);

	overUserConfigurationTipLabel = new QLabel(overUserConfigurationPreview);
	overUserConfigurationTipLabel->setTextFormat(Qt::RichText);
	overUserConfigurationTipLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	overUserConfigurationTipLabel->setContentsMargins(10, 0, 0, 0);

	lay->addWidget(overUserConfigurationTipLabel);

   	configureOverUserHint = new QPushButton(tr("Configure"));
	connect(configureOverUserHint, SIGNAL(clicked()), this, SLOT(showOverUserConfigurationWindow()));

	Buddy example = Buddy::dummy();

	if (!example.isNull())
		hint_manager->prepareOverUserHint(overUserConfigurationPreview, overUserConfigurationTipLabel, example);

	lay = new QHBoxLayout(configureHint);
	lay->addWidget(overUserConfigurationPreview);
	lay->addWidget(configureOverUserHint);

	toolTipBox->addWidgets(new QLabel("Hint over buddy list: "), configureHint);
}

void HintsConfigurationUiHandler::showAdvanced()
{
	if (!AdvancedWindow)
	{
		AdvancedWindow = new ConfigurationWindow("HintsAdvanced", tr("Advenced hints's configuration"), "Notification", MainConfigurationWindow::instanceDataManager());
		AdvancedWindow->widget()->appendUiFile(dataPath("kadu/modules/configuration/hints-advanced.ui"));

		newHintUnder = dynamic_cast<QComboBox *>(AdvancedWindow->widget()->widgetById("hints/newHintUnder"));

		ownPosition = dynamic_cast<QCheckBox *>(AdvancedWindow->widget()->widgetById("hints/ownPosition"));
		connect(ownPosition, SIGNAL(toggled(bool)), AdvancedWindow->widget()->widgetById("hints/ownPositionX"), SLOT(setEnabled(bool)));
		connect(ownPosition, SIGNAL(toggled(bool)), AdvancedWindow->widget()->widgetById("hints/ownPositionY"), SLOT(setEnabled(bool)));
		connect(ownPosition, SIGNAL(toggled(bool)), AdvancedWindow->widget()->widgetById("hints/ownPositionCorner"), SLOT(setEnabled(bool)));
		connect(ownPosition, SIGNAL(toggled(bool)), this, SLOT(updateHintsPreview()));

		minimumWidth = dynamic_cast<QSpinBox *>(AdvancedWindow->widget()->widgetById("hints/minimumWidth"));
		maximumWidth = dynamic_cast<QSpinBox *>(AdvancedWindow->widget()->widgetById("hints/maximumWidth"));
		connect(minimumWidth, SIGNAL(valueChanged(int)), this, SLOT(minimumWidthChanged(int)));
		connect(maximumWidth, SIGNAL(valueChanged(int)), this, SLOT(maximumWidthChanged(int)));

		xPosition = dynamic_cast<QSpinBox *>(AdvancedWindow->widget()->widgetById("hints/ownPositionX"));
		connect(xPosition, SIGNAL(valueChanged(int)), this, SLOT(updateHintsPreview()));
		yPosition = dynamic_cast<QSpinBox *>(AdvancedWindow->widget()->widgetById("hints/ownPositionY"));
		connect(yPosition, SIGNAL(valueChanged(int)), this, SLOT(updateHintsPreview()));

		ownPositionCorner = dynamic_cast<QComboBox *>(AdvancedWindow->widget()->widgetById("hints/ownPositionCorner"));
		connect(ownPositionCorner, SIGNAL(currentIndexChanged(int)), this, SLOT(updateHintsPreview()));

		QPushButton *previewButton = dynamic_cast<QPushButton *>(AdvancedWindow->widget()->widgetById("hints/preview"));
		connect(previewButton, SIGNAL(clicked()), this, SLOT(addHintsPreview()));

		connect(AdvancedWindow, SIGNAL(destroyed()), this, SLOT(advancedDestroyed()));
	}

	AdvancedWindow->show();
}

void HintsConfigurationUiHandler::advancedDestroyed()
{
	AdvancedWindow = 0;
}

void HintsConfigurationUiHandler::addHintsPreview()
{
	Notification *previewNotify = new Notification(QString("Preview"), "protocols/common/message");
	previewNotify->setText(qApp->translate("@default", "Hints position preview"));

	Hint *previewHint = new Hint(previewHintsFrame, previewNotify);
	previewHints.append(previewHint);

	setPreviewLayoutDirection();
	previewHintsLayout->addWidget(static_cast<QWidget *>(previewHint));

	connect(previewHint, SIGNAL(leftButtonClicked(Hint *)), this, SLOT(deleteHintsPreview(Hint *)));
	connect(previewHint, SIGNAL(rightButtonClicked(Hint *)), this, SLOT(deleteAllHintsPreview()));

	updateHintsPreview();

	previewHintsFrame->show();
}

void HintsConfigurationUiHandler::setPreviewLayoutDirection()
{
	QPoint trayPosition;
	QSize desktopSize = QApplication::desktop()->screenGeometry(previewHintsFrame).size();
	emit searchingForTrayPosition(trayPosition);

	switch (newHintUnder->currentIndex())
	{
		case 0:
			if (trayPosition.isNull() || ownPosition->isChecked())
			{
				if (yPosition->value() < desktopSize.height()/2)
					previewHintsLayout->setDirection(QBoxLayout::Down);
				else
					previewHintsLayout->setDirection(QBoxLayout::Up);
			}
			else
			{
				if (trayPosition.y() < desktopSize.height()/2)
					previewHintsLayout->setDirection(QBoxLayout::Down);
				else
					previewHintsLayout->setDirection(QBoxLayout::Up);
			}
			break;
		case 1:
			previewHintsLayout->setDirection(QBoxLayout::Up);
			break;
		case 2:
			previewHintsLayout->setDirection(QBoxLayout::Down);
			break;
	}

}

void HintsConfigurationUiHandler::updateHintsPreview()
{
	QPoint newPosition;
	QPoint trayPosition;

	previewHintsFrame->adjustSize();
	QSize preferredSize = previewHintsFrame->sizeHint();
	QSize desktopSize = QApplication::desktop()->screenGeometry(previewHintsFrame).size();

	emit searchingForTrayPosition(trayPosition);
	if (ownPosition->isChecked() || trayPosition.isNull())
	{
		newPosition = QPoint(xPosition->value(), yPosition->value());

		switch (ownPositionCorner->currentIndex())
		{
			case 1: // "TopRight"
				newPosition -= QPoint(preferredSize.width(), 0);
				break;
			case 2: // "BottomLeft"
				newPosition -= QPoint(0, preferredSize.height());
				break;
			case 3: // "BottomRight"
				newPosition -= QPoint(preferredSize.width(), preferredSize.height());
				break;
			case 0: // "TopLeft"
				break;
		};

		if (newPosition.x() < 0) // when hints go out of the screen (on left)
			newPosition.setX(0);
		if (newPosition.y() < 0) //when hints go out of the screen (on top)
			newPosition.setY(0);

		if (newPosition.x() + preferredSize.width() >= desktopSize.width()) //when hints go out of the screen (on right)
			newPosition.setX(desktopSize.width() - preferredSize.width());
		if (newPosition.y() + preferredSize.height() >= desktopSize.height()) //when hints go out of the screen (on bottom)
			newPosition.setY(desktopSize.height() - preferredSize.height());
	}
	else
	{
		// those "strange" cases happens when "automatic panel hiding" is in use
		if (trayPosition.x() < 0)
			trayPosition.setX(0);
		else if (trayPosition.x() > desktopSize.width())
			trayPosition.setX(desktopSize.width() - 2);
		if (trayPosition.y() < 0)
			trayPosition.setY(0);
		else if (trayPosition.y() > desktopSize.height())
			trayPosition.setY(desktopSize.height() - 2);


		if (trayPosition.x() < desktopSize.width() / 2) // tray is on left
			newPosition.setX(trayPosition.x() + 32);
		else // tray is on right
			newPosition.setX(trayPosition.x() - preferredSize.width());

		if (trayPosition.y() < desktopSize.height() / 2) // tray is on top
			newPosition.setY(trayPosition.y() + 32);
		else // tray is on bottom
			newPosition.setY(trayPosition.y() - preferredSize.height());
	}

	previewHintsFrame->setGeometry(newPosition.x(), newPosition.y(), preferredSize.width(), preferredSize.height());

	previewHintsFrame->setWindowOpacity(hint_manager->opacity());
}

void HintsConfigurationUiHandler::deleteHintsPreview(Hint *hint)
{
	previewHints.removeAll(hint);
	previewHintsLayout->removeWidget(static_cast<QWidget *>(hint));
	hint->deleteLater();

	foreach (Hint *h, previewHints)
	{
		if (h->getNotification()->type() == "Preview")
			return;
	}

	previewHintsFrame->hide();
}

void HintsConfigurationUiHandler::deleteAllHintsPreview()
{
	foreach (Hint *h, previewHints)
	{
		if (h->getNotification()->type() == "Preview")
		{
			previewHints.removeAll(h);
			previewHintsLayout->removeWidget(static_cast<QWidget *>(h));
			h->deleteLater();
		}
	}

	previewHintsFrame->hide();
}

void HintsConfigurationUiHandler::toolTipClassesHighlighted(const QString &value)
{
	configureOverUserHint->setEnabled(value == qApp->translate("@default", "Hints"));
	overUserConfigurationPreview->setEnabled(value == qApp->translate("@default", "Hints"));
}

void HintsConfigurationUiHandler::showOverUserConfigurationWindow()
{
	if (overUserConfigurationWindow)
		_activateWindow(overUserConfigurationWindow);
	else
	{
		overUserConfigurationWindow = new HintOverUserConfigurationWindow(Buddy::dummy());
		connect(overUserConfigurationWindow, SIGNAL(configurationSaved()), this, SLOT(updateOverUserPreview()));
		connect(overUserConfigurationWindow, SIGNAL(destroyed()), this, SLOT(hintOverUserConfigurationWindowDestroyed()));
		overUserConfigurationWindow->show();
	}
}

void HintsConfigurationUiHandler::updateOverUserPreview()
{
	if (!overUserConfigurationPreview)
		return;

	Buddy example = Buddy::dummy();

	if (!example.isNull())
		hint_manager->prepareOverUserHint(overUserConfigurationPreview, overUserConfigurationTipLabel, example);
}

void HintsConfigurationUiHandler::mainConfigurationWindowDestroyed()
{
	deleteAllHintsPreview();
	overUserConfigurationPreview = 0;
}

void HintsConfigurationUiHandler::hintOverUserConfigurationWindowDestroyed()
{
	overUserConfigurationWindow = 0;
}

void HintsConfigurationUiHandler::minimumWidthChanged(int value)
{
	if (value > maximumWidth->value())
		maximumWidth->setValue(value);
}

void HintsConfigurationUiHandler::maximumWidthChanged(int value)
{
	if (value < minimumWidth->value())
		minimumWidth->setValue(value);
}
