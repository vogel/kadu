/*
 * %kadu copyright begin%
 * Copyright 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QPalette>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextEdit>

#include "accounts/account.h"
#include "buddies/buddy-preferred-manager.h"
#include "configuration/config-file-data-manager.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact.h"
#include "core/application.h"
#include "gui/widgets/configuration/config-color-button.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/config-select-font.h"
#include "gui/widgets/configuration/config-spin-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"
#include "parser/parser.h"

#include "hint_manager.h"
#include "hints-plugin.h"

#include "hint-over-user-configuration-window.h"

HintOverUserConfigurationWindow::HintOverUserConfigurationWindow(Buddy exampleBuddy) :
	ConfigurationWindow("HintOverUser", tr("Hint Over Buddy Configuration"), "Hints", MainConfigurationWindow::instanceDataManager()),
	ExampleBuddy(exampleBuddy)
{
	connect(this, SIGNAL(configurationWindowApplied()), this, SLOT(configurationWindowApplied()));
	widget()->appendUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/hint-over-user.ui"));

	connect(static_cast<ConfigSelectFont *>(widget()->widgetById("font")), SIGNAL(fontChanged(QFont)),
			this, SLOT(fontChanged(QFont)));
	connect(static_cast<ConfigColorButton *>(widget()->widgetById("fgcolor")), SIGNAL(changed(const QColor &)), this, SLOT(foregroundColorChanged(const QColor &)));
	connect(static_cast<ConfigColorButton *>(widget()->widgetById("bgcolor")), SIGNAL(changed(const QColor &)), this, SLOT(backgroundColorChanged(const QColor &)));
	connect(static_cast<ConfigColorButton *>(widget()->widgetById("bdcolor")), SIGNAL(changed(const QColor &)), this, SLOT(borderColorChanged(const QColor &)));
	connect(static_cast<ConfigSpinBox *>(widget()->widgetById("bdwidth")), SIGNAL(valueChanged(int)), this, SLOT(borderWidthChanged(int)));

	ConfigGroupBox *groupBox = widget()->configGroupBox("Look", "Buddy List", "Hint Over Buddy");

	previewFrame = new QFrame;
	previewFrame->setObjectName("tip_frame");

	QHBoxLayout *lay = new QHBoxLayout(previewFrame);
	lay->setMargin(10);
	lay->setSizeConstraint(QLayout::SetFixedSize);

	previewTipLabel = new QLabel(previewFrame);
	previewTipLabel->setTextFormat(Qt::RichText);
	previewTipLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	previewTipLabel->setContentsMargins(10, 10, 10, 10);

	lay->addWidget(previewTipLabel);

	groupBox->insertWidget(0, previewFrame, true);

	QWidget *syntaxWidget = new QWidget;
	lay = new QHBoxLayout(syntaxWidget);
	hintSyntax = new QTextEdit;
	hintSyntax->setAcceptRichText(true);
	hintSyntax->setPlainText(Application::instance()->configuration()->deprecatedApi()->readEntry("Hints", "MouseOverUserSyntax"));
	hintSyntax->setToolTip(tr(MainConfigurationWindow::SyntaxText));

	QPushButton *syntaxChangedButton = new QPushButton(tr("Update preview"));
	connect(syntaxChangedButton, SIGNAL(clicked()), this, SLOT(syntaxChanged()));

	lay->addWidget(new QLabel(tr("Syntax") + ':'));
	lay->addWidget(hintSyntax);
	lay->addWidget(syntaxChangedButton);
	groupBox->addWidget(syntaxWidget, true);

	HintsPlugin::instance()->hintsManger()->prepareOverUserHint(previewFrame, previewTipLabel, ExampleBuddy);

	bgcolor = Application::instance()->configuration()->deprecatedApi()->readColorEntry("Hints", "HintOverUser_bgcolor").name();
	fgcolor = Application::instance()->configuration()->deprecatedApi()->readColorEntry("Hints", "HintOverUser_fgcolor").name();
	bdcolor = Application::instance()->configuration()->deprecatedApi()->readColorEntry("Hints", "HintOverUser_bdcolor").name();
	bdwidth = Application::instance()->configuration()->deprecatedApi()->readNumEntry("Hints", "HintOverUser_borderWidth", 1);
}

void HintOverUserConfigurationWindow::fontChanged(QFont font)
{
	previewTipLabel->setFont(font);
}

void HintOverUserConfigurationWindow::foregroundColorChanged(const QColor &color)
{
	fgcolor = color.name();
	QString style = QString("QFrame#tip_frame {border-width: %1px; border-style: solid; border-color: %2;"
				"border-radius: %3px; background-color: %4} QFrame { color: %5}")
			.arg(bdwidth).arg(bdcolor).arg(0).arg(bgcolor).arg(fgcolor);

	previewFrame->setStyleSheet(style);
}

void HintOverUserConfigurationWindow::backgroundColorChanged(const QColor &color)
{
	bgcolor = color.name();
	QString style = QString("QFrame#tip_frame {border-width: %1px; border-style: solid; border-color: %2;"
				"border-radius: %3px; background-color: %4} QFrame { color: %5}")
			.arg(bdwidth).arg(bdcolor).arg(0).arg(bgcolor).arg(fgcolor);

	previewFrame->setStyleSheet(style);
}

void HintOverUserConfigurationWindow::borderColorChanged(const QColor &color)
{
	bdcolor = color.name();
	QString style = QString("QFrame#tip_frame {border-width: %1px; border-style: solid; border-color: %2;"
				"border-radius: %3px; background-color: %4} QFrame { color: %5}")
			.arg(bdwidth).arg(bdcolor).arg(0).arg(bgcolor).arg(fgcolor);

	previewFrame->setStyleSheet(style);
}

void HintOverUserConfigurationWindow::borderWidthChanged(int width)
{
	bdwidth = width;
	QString style = QString("QFrame#tip_frame {border-width: %1px; border-style: solid; border-color: %2;"
				"border-radius: %3px; background-color: %4} QFrame { color: %5}")
			.arg(bdwidth).arg(bdcolor).arg(0).arg(bgcolor).arg(fgcolor);

	previewFrame->setStyleSheet(style);
}

void HintOverUserConfigurationWindow::syntaxChanged()
{
	if (!hintSyntax->document()->isModified())
		return;

	QString text = Parser::parse(hintSyntax->toPlainText(), Talkable(ExampleBuddy), ParserEscape::HtmlEscape);

	/* Dorr: the file:// in img tag doesn't generate the image on hint.
	 * for compatibility with other syntaxes we're allowing to put the file://
	 * so we have to remove it here */
	text = text.remove("file://");

	while (text.endsWith(QLatin1String("<br/>")))
		text.resize(text.length() - 5 /* 5 == QString("<br/>").length()*/);
	while (text.startsWith(QLatin1String("<br/>")))
		text = text.right(text.length() - 5 /* 5 == QString("<br/>").length()*/);

	previewTipLabel->setText(text);
}

void HintOverUserConfigurationWindow::configurationWindowApplied()
{
	Application::instance()->configuration()->deprecatedApi()->writeEntry("Hints", "MouseOverUserSyntax", hintSyntax->toPlainText());
}

#include "moc_hint-over-user-configuration-window.cpp"
