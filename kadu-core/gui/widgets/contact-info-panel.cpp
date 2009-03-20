/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtWebKit/QWebFrame>

#include "config_file.h"
#include "debug.h"
#include "emoticons.h"
#include "html_document.h"
#include "kadu_parser.h"
#include "syntax_editor.h"

#include "contact-info-panel.h"

ContactInfoPanel::ContactInfoPanel(QWidget *parent) : KaduTextBrowser(parent)
{
// TODO: 0.6.5
// 	InfoPanel->setFrameStyle(QFrame::NoFrame);
// 	InfoPanel->setMinimumHeight(int(1.5 * QFontMetrics(InfoPanel->QTextEdit::font()).height()));
//	InfoPanel->resize(InfoPanel->size().width(), int(1.5 * QFontMetrics(InfoPanel->font()).height()));
// 	InfoPanel->setTextFormat(Qt::RichText);
// 	InfoPanel->setAlignment(Qt::AlignVCenter/** | Qt::WordBreak | Qt::DontClip*/);

	configurationUpdated();
}

ContactInfoPanel::~ContactInfoPanel()
{
}

void ContactInfoPanel::configurationUpdated()
{
	QFont font = config_file.readFontEntry("Look", "PanelFont");
	QString fontFamily = font.family();
	QString fontSize;
	if (font.pointSize() > 0)
		fontSize = QString::number(font.pointSize()) + "pt";
	else
		fontSize = QString::number(font.pixelSize()) + "px";

	QString fontStyle = font.italic() ? "italic" : "normal";
	QString fontWeight = font.bold() ? "bold" : "normal";
	QString textDecoration = font.underline() ? "underline" : "none";
	QString backgroundColor = config_file.readColorEntry("Look", "InfoPanelBgColor").name();
	QString fontColor = config_file.readColorEntry("Look", "InfoPanelFgColor").name();

	Template = QString(
		"<html>"
		"	<head>"
		"		<style type='text/css'>"
		"html {"
		"	color: %1;"
		"	font: %2 %3 %4 %5;"
		"	text-decoration: %6;"
		"	margin: 0;"
		"	padding: 0;"
		"	background-color: %7;"
		"}"
		"div {"
		"	color: %1;"
		"	font: %2 %3 %4 %5;"
		"	text-decoration: %6;"
		"	margin: 0;"
		"	padding: 0;"
		"	background-color: %7;"
		"}"
		"		</style>"
		"	</head>"
		"	<body>"
		"		%8"
		"	</body>"
		"</html>"
		).arg(fontColor, fontStyle, fontWeight, fontSize, fontFamily, textDecoration, backgroundColor, "%1");

	Syntax = SyntaxList::readSyntax("infopanel", config_file.readEntry("Look", "InfoPanelSyntaxFile"),
		"<table><tr><td><img width=\"32\" height=\"32\" align=\"left\" valign=\"top\" src=\"file:///@{ManageUsersWindowIcon}\"></td><td> "
		"<div align=\"left\"> [<b>%a</b>][ (%u)] [<br>tel.: %m][<br>IP: %i]</div></td></tr></table> <hr> <b>%s</b> [<br>%d]");
	setHtml("<body bgcolor=\"" + config_file.readEntry("Look", "InfoPanelBgColor") + "\"></body>");
	displayContact(CurrentContact);

	if (config_file.readBoolEntry("Look", "PanelVerticalScrollbar"))
		page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAsNeeded);
	else
		page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
}

void ContactInfoPanel::displayContact(Contact contact)
{
	CurrentContact = contact;

	if (Contact::null == contact || !isVisible())
		return;

	kdebugmf(KDEBUG_INFO, "%s\n", qPrintable(contact.display()));

	HtmlDocument doc;
	doc.parseHtml(KaduParser::parse(Syntax, contact.prefferedAccount(), contact));
	doc.convertUrlsToHtml();
	doc.convertMailToHtml();
	if (EMOTS_NONE != (EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle") &&
			config_file.readBoolEntry("General", "ShowEmotPanel"))
		emoticons->expandEmoticons(doc,
				config_file.readColorEntry("Look", "InfoPanelBgColor"),
				(EmoticonsStyle)config_file.readNumEntry("Chat", "EmoticonsStyle"));

	setHtml(Template.arg(doc.generateHtml()));

	kdebugf2();
}
