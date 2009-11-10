/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPalette>
#include <QtGui/QSpinBox>
#include <QtGui/QTextEdit>

#include "accounts/account.h"
#include "configuration/configuration-file.h"
#include "configuration/config-file-data-manager.h"
#include "contacts/contact.h"
#include "gui/widgets/configuration/config-color-button.h"
#include "gui/widgets/configuration/config-combo-box.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/config-select-font.h"
#include "gui/widgets/configuration/config-spin-box.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/misc.h"
#include "parser/parser.h"

#include "hint_manager.h"

#include "hint-over-user-configuration-window.h"

HintOverUserConfigurationWindow::HintOverUserConfigurationWindow(Buddy exampleContact) :
	ConfigurationWindow("hint-over-user", tr("Hint over user configuration"), MainConfigurationWindow::instanceDataManager()),
	ExampleContact(exampleContact)
{
	connect(this, SIGNAL(configurationWindowApplied()), this, SLOT(configurationWindowApplied()));
	widget()->appendUiFile(dataPath("kadu/modules/configuration/hint-over-user.ui"));

	connect(dynamic_cast<ConfigSelectFont *>(widget()->widgetById("font")), SIGNAL(fontChanged(QFont)),
			this, SLOT(fontChanged(QFont)));
	connect(dynamic_cast<ConfigColorButton *>(widget()->widgetById("fgcolor")), SIGNAL(changed(const QColor &)), this, SLOT(foregroundColorChanged(const QColor &)));
	connect(dynamic_cast<ConfigColorButton *>(widget()->widgetById("bgcolor")), SIGNAL(changed(const QColor &)), this, SLOT(backgroundColorChanged(const QColor &)));
	connect(dynamic_cast<ConfigColorButton *>(widget()->widgetById("bdcolor")), SIGNAL(changed(const QColor &)), this, SLOT(borderColorChanged(const QColor &)));
	connect(dynamic_cast<ConfigSpinBox *>(widget()->widgetById("bdwidth")), SIGNAL(valueChanged(int)), this, SLOT(borderWidthChanged(int)));
	connect(dynamic_cast<ConfigComboBox *>(widget()->widgetById("iconsize")), SIGNAL(currentIndexChanged(int)), this, SLOT(iconSizeChanged(int)));

	ConfigGroupBox *groupBox = widget()->configGroupBox("Look", "Userbox", "Hint over user");

	previewFrame = new QFrame;
	previewFrame->setObjectName("tip_frame");

	QHBoxLayout *lay = new QHBoxLayout(previewFrame);
	lay->setMargin(10);
	lay->setSizeConstraint(QLayout::SetFixedSize);

	previewIconLabel = new QLabel(previewFrame);
	previewIconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	previewTipLabel = new QLabel(previewFrame);
	previewTipLabel->setTextFormat(Qt::RichText);
	previewTipLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	previewTipLabel->setContentsMargins(10, 0, 0, 0);

	lay->addWidget(previewIconLabel, Qt::AlignTop);
	lay->addWidget(previewTipLabel);

	groupBox->insertWidget(0, previewFrame, true);

	QWidget *syntaxWidget = new QWidget;
	lay = new QHBoxLayout(syntaxWidget);
	hintSyntax = new QTextEdit;
	hintSyntax->setAcceptRichText(true);
	hintSyntax->setPlainText(config_file.readEntry("Hints", "MouseOverUserSyntax"));
	hintSyntax->setToolTip(tr(MainConfigurationWindow::SyntaxText));

	QPushButton *syntaxChangedButton = new QPushButton(tr("Update preview"));
	connect(syntaxChangedButton, SIGNAL(clicked()), this, SLOT(syntaxChanged()));

	lay->addWidget(new QLabel(tr("Syntax") + ":"));
	lay->addWidget(hintSyntax);
	lay->addWidget(syntaxChangedButton);
	groupBox->addWidget(syntaxWidget, true);

	hint_manager->prepareOverUserHint(previewFrame, previewIconLabel, previewTipLabel, ExampleContact);

	bgcolor = config_file.readColorEntry("Hints", "HintOverUser_bgcolor").name();
	fgcolor = config_file.readColorEntry("Hints", "HintOverUser_fgcolor").name();
	bdcolor = config_file.readColorEntry("Hints", "HintOverUser_bdcolor").name();
	bdwidth = config_file.readNumEntry("Hints", "HintOverUser_borderWidth", 1);
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
//TODO 0.6.6:
void HintOverUserConfigurationWindow::iconSizeChanged(int index)
{
	previewIconLabel->setPixmap(ExampleContact.prefferedAccount().statusContainer()->statusPixmap(ExampleContact.contact(ExampleContact.prefferedAccount())->currentStatus()));
}

void HintOverUserConfigurationWindow::syntaxChanged()
{
	if (!hintSyntax->document()->isModified())
		return;

	QString text = Parser::parse(hintSyntax->toPlainText(), ExampleContact.prefferedAccount(), ExampleContact);

	/* Dorr: the file:// in img tag doesn't generate the image on hint.
	 * for compatibility with other syntaxes we're allowing to put the file://
	 * so we have to remove it here */
	text = text.replace("file://", "");

	while (text.endsWith("<br/>"))
		text.resize(text.length() - 5 /* 5 == QString("<br/>").length()*/);
	while (text.startsWith("<br/>"))
		text = text.right(text.length() - 5 /* 5 == QString("<br/>").length()*/);

	previewTipLabel->setText(text);
}

void HintOverUserConfigurationWindow::configurationWindowApplied()
{
	config_file.writeEntry("Hints", "MouseOverUserSyntax", hintSyntax->toPlainText());
}
