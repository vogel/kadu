/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qstringlist.h>

#include "config_file.h"
#include "configuration_window_widgets.h"
#include "debug.h"
#include "kadu.h"
#include "kadu_parser.h"
#include "message_box.h"
#include "misc.h"
#include "xosd_notify.h"
#include "../notify/notification.h"
#include "../notify/notify.h"

extern "C" int xosd_notify_init()
{
	xosd_notify = new XOSDNotify();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/xosd_notify.ui"), xosd_notify);
	return 0;
}

extern "C" void xosd_notify_close()
{
	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/xosd_notify.ui"), xosd_notify);
	delete xosd_notify;
}

TestConfig::TestConfig() : type(), position(0), font_color(),
	outline_color(), shadow_color(), font(), timeout(0), shadow_offset(0),
	outline_offset(0), font_size(-1)
{
}

int XOSDConfigurationWidget::getFontSize(const QString &xlfd)
{
	kdebugf();

	QStringList font = QStringList::split("-", xlfd, true);
	if (font.size() < 8)
	{
		kdebugf2();
		return -1;
	}
	else if (font[7] == "*" || font[7].isEmpty())
	{
		kdebugf2();
		return -1;
	}
	else
	{
		kdebugf2();
		return font[7].toInt();
	}
}

void XOSDConfigurationWidget::clicked_ChangeFont()
{
	kdebugf();

	main_xlfd_chooser->getFont(this, SLOT(fontSelected(const QString &)), currentConfig.font);

	kdebugf2();
}

void XOSDConfigurationWidget::fontSelected(const QString &font)
{
	kdebugf();
	kdebugm(KDEBUG_INFO, "%s %d %d\n", font.local8Bit().data(), font.isEmpty(), font.isNull());

	int size = getFontSize(font);
	if (size < 0)
	{
		MessageBox::msg(tr("Please select font size! (pxlsz)"));
		kdebugf2();
		return;
	}
	currentConfig.font = font;
	currentConfig.font_size = size;

	kdebugf2();
}

XOSDConfigurationWidget::XOSDConfigurationWidget(QWidget *parent, char *name)
	: NotifierConfigurationWidget(parent, name), currentNotifyEvent("")
{
	QStringList positions;
	positions << tr("Top left") << tr("Top") << tr("Top right")<<tr("Left")<<tr("Center")<<tr("Right")<<tr("Bottom left")<<tr("Bottom")<<tr("Bottom right");
	
	position = new QComboBox(this);
	position->insertStringList(positions);
	QPushButton *testButton = new QPushButton(tr("Test"), this);
	QPushButton *selectFontButton = new QPushButton(tr("Select font"), this);
	fontColor = new ColorButton(this);
	outlineColor = new ColorButton(this);
	shadowColor = new ColorButton(this);
	timeout = new QSpinBox(1,2048,1,this);
	OutlineOffset = new QSpinBox(0,2048,1,this);
	shadowOffset = new QSpinBox(0,2048,1,this);

	connect(testButton, SIGNAL(clicked()), this, SLOT(test()));
	connect(selectFontButton, SIGNAL(clicked()), this, SLOT(clicked_ChangeFont()));
	connect(fontColor, SIGNAL(changed(const QColor &)), this, SLOT(clicked_ChangeFgColor(const QColor &)));
	connect(shadowColor, SIGNAL(changed(const QColor &)), this, SLOT(clicked_ChangeOutlineColor(const QColor &)));
	connect(outlineColor, SIGNAL(changed(const QColor &)), this, SLOT(clicked_ChangeShadowColor(const QColor &)));
	connect(timeout, SIGNAL(valueChanged(int)), this, SLOT(changed_Timeout(int)));
	connect(OutlineOffset, SIGNAL(valueChanged(int)), this, SLOT(changed_OutlineOffset(int)));
	connect(shadowOffset, SIGNAL(valueChanged(int)), this, SLOT(changed_ShadowOffset(int)));
	connect(position, SIGNAL(activated(int)), this, SLOT(clicked_Positions(int)));

	QGridLayout *gridLayout = new QGridLayout(this, 0, 0, 0, 5);
	gridLayout->addWidget(new QLabel(tr("Positions") + ":", this), 0, 0, Qt::AlignRight);
	gridLayout->addWidget(position, 0, 1);
	gridLayout->addWidget(new QLabel(tr("Timeout") + ":", this), 1, 0, Qt::AlignRight);
	gridLayout->addWidget(timeout, 1, 1);
	gridLayout->addWidget(new QLabel(tr("Outline offset") + ":", this), 2, 0, Qt::AlignRight);
	gridLayout->addWidget(OutlineOffset, 2, 1);
	gridLayout->addWidget(new QLabel(tr("Shadow offset") + ":", this), 3, 0, Qt::AlignRight);
	gridLayout->addWidget(shadowOffset, 3, 1);
	gridLayout->addWidget(new QLabel(tr("Font color") + ":", this), 4, 0, Qt::AlignRight);
	gridLayout->addWidget(fontColor, 4, 1);
	gridLayout->addWidget(new QLabel(tr("Outline color") + ":", this), 5, 0, Qt::AlignRight);
	gridLayout->addWidget(outlineColor, 5, 1);
	gridLayout->addWidget(new QLabel(tr("Shadow color") + ":", this), 6, 0, Qt::AlignRight);
	gridLayout->addWidget(shadowColor, 6, 1);
	gridLayout->addMultiCellWidget(selectFontButton, 7, 7, 0, 1);
	gridLayout->addMultiCellWidget(testButton, 8, 8, 0, 1);

	main_xlfd_chooser = new XLFDChooser(this, "main_xlfd_chooser");
}

XOSDConfigurationWidget::~XOSDConfigurationWidget()
{
	main_xlfd_chooser->deleteLater();
	main_xlfd_chooser = NULL;
}

void XOSDConfigurationWidget::test()
{
	kdebugf();

	xosd_notify->test(tr("Testing configuration"), currentConfig);

	kdebugf2();
}

void XOSDConfigurationWidget::saveNotifyConfigurations()
{
	if (currentNotifyEvent != "")
		configs[currentNotifyEvent] = currentConfig;

	CONST_FOREACH(config, configs)
	{
		const QString &eventName = config.key();

		config_file.writeEntry("XOSD", eventName + "Position", (*config).position);
		config_file.writeEntry("XOSD", eventName + "FontColor", (*config).font_color);
		config_file.writeEntry("XOSD", eventName + "OutlineColor", (*config).outline_color);
		config_file.writeEntry("XOSD", eventName + "ShadowColor", (*config).shadow_color);
		config_file.writeEntry("XOSD", eventName + "Font", (*config).font);
		config_file.writeEntry("XOSD", eventName + "Timeout", (*config).timeout);
		config_file.writeEntry("XOSD", eventName + "OutlineOffset", (*config).shadow_offset);
		config_file.writeEntry("XOSD", eventName + "ShadowOffset", (*config).outline_offset);

	}
}

void XOSDConfigurationWidget::switchToEvent(const QString &event)
{
	if (currentNotifyEvent != "")
		configs[currentNotifyEvent] = currentConfig;
	currentNotifyEvent = event;

	if (configs.contains(event))
		currentConfig = configs[event];
	else
	{
		currentConfig.type = event;
		currentConfig.position = config_file.readNumEntry("XOSD", event+"Position");
		currentConfig.font_color = config_file.readColorEntry("XOSD", event+"FontColor");
		currentConfig.outline_color = config_file.readColorEntry("XOSD", event+"OutlineColor");
		currentConfig.shadow_color = config_file.readColorEntry("XOSD", event+"ShadowColor");
		currentConfig.font = config_file.readEntry("XOSD", event+"Font");
		currentConfig.font_size = getFontSize(currentConfig.font);
		currentConfig.timeout = config_file.readNumEntry("XOSD", event+"Timeout");
		currentConfig.shadow_offset = config_file.readNumEntry("XOSD", event+"ShadowOffset");
		currentConfig.outline_offset = config_file.readNumEntry("XOSD", event+"OutlineOffset");
	}
	fontColor->setColor(currentConfig.font_color);
	outlineColor->setColor(currentConfig.outline_color);
	shadowColor->setColor(currentConfig.shadow_color);
	timeout->setValue(currentConfig.timeout);
	OutlineOffset->setValue(currentConfig.outline_offset);
	shadowOffset->setValue(currentConfig.shadow_offset);
	position->setCurrentItem(currentConfig.position);
}

void XOSDConfigurationWidget::clicked_Positions(int pos)
{
	kdebugf();

	currentConfig.position = pos;

	kdebugf2();
}

void XOSDConfigurationWidget::changed_Timeout(int value)
{
	kdebugf();
		
	currentConfig.timeout = value;

	kdebugf2();
}

void XOSDConfigurationWidget::changed_OutlineOffset(int value)
{
	kdebugf();
	currentConfig.outline_offset=value;
	kdebugf2();
}

void XOSDConfigurationWidget::changed_ShadowOffset(int value)
{
	kdebugf();
	currentConfig.shadow_offset = value;
	kdebugf2();
}

void XOSDConfigurationWidget::clicked_ChangeFgColor(const QColor &color)
{
	kdebugf();

	if (color.isValid())
	{
		currentConfig.font_color = color;
	}

	kdebugf2();
}

void XOSDConfigurationWidget::clicked_ChangeOutlineColor(const QColor &color)
{
	kdebugf();

	if (color.isValid())
	{
		currentConfig.outline_color = color;
	}

	kdebugf2();
}

void XOSDConfigurationWidget::clicked_ChangeShadowColor(const QColor &color)
{
	kdebugf();

	if (color.isValid())
	{
		currentConfig.shadow_color = color;
	}

	kdebugf2();
}

XOSDNotify::OSDLine::OSDLine() : handle(0), text(), timeout(0), font_color(),
	outline_color(), shadow_color(), font(), font_size(-1)
{
}

XOSDNotify::XOSDNotify(QObject *parent, const char *name) : Notifier(parent, name),	configs(),
	offsets(), lines(),	timer(new QTimer(this, "timer")), currentOptionPrefix(), optionPrefixes(), testXOSD(0)
{
	kdebugf();

	import_0_5_0_configuration();

	CONST_FOREACH(notifyEvent, notification_manager->notifyEvents())
	{
		optionPrefixes << (*notifyEvent).name;
	}

	int val;
	CONST_FOREACH(it, optionPrefixes)
	{
		QString tmp = (*it)+"Position";
		config_file.addVariable("XOSD", tmp, 4);
		val = config_file.readNumEntry("XOSD", tmp);
		if (val < 0 || val >= 9) config_file.writeEntry("XOSD", tmp, 4);

		config_file.addVariable("XOSD", (*it)+"FontColor", QColor("blue"));
		config_file.addVariable("XOSD", (*it)+"OutlineColor", QColor("red"));
		config_file.addVariable("XOSD", (*it)+"ShadowColor", QColor("green"));
		config_file.addVariable("XOSD", (*it)+"Font", "-*-courier-*-r-*-*-14-*-*-*-*-*-*-*");

		tmp = (*it)+"Timeout";
		config_file.addVariable("XOSD", tmp, 10);
		val = config_file.readNumEntry("XOSD", tmp);
		if (val < 0 || val >= 2048) config_file.writeEntry("XOSD", tmp, 10);

		tmp = (*it)+"OutlineOffset";
		config_file.addVariable("XOSD", tmp, 0);
		val = config_file.readNumEntry("XOSD", tmp);
		if (val < 0 || val >= 2048) config_file.writeEntry("XOSD", tmp, 0);

		tmp = (*it)+"ShadowOffset";
		config_file.addVariable("XOSD", tmp, 0);
		val = config_file.readNumEntry("XOSD", tmp);
		if (val < 0 || val >= 2048) config_file.writeEntry("XOSD", tmp, 0);
	}
	for(int i = 0; i < 9; ++i)
	{
		config_file.addVariable("XOSD", QString("OffsetX%1").arg(i), 0);
		config_file.addVariable("XOSD", QString("OffsetY%1").arg(i), 0);
	}

	notification_manager->registerNotifier(QT_TRANSLATE_NOOP("@default", "XOSD"), this);

	connect(timer, SIGNAL(timeout()), this, SLOT(oneSecond()));
	kdebugf2();
}

XOSDNotify::~XOSDNotify()
{
	kdebugf();

	disconnect(timer, SIGNAL(timeout()), this, SLOT(oneSecond()));
	delete timer;
	notification_manager->unregisterNotifier("XOSD");

	kdebugf2();
}

void XOSDNotify::position2PosAlign(int position, xosd_pos &pos, xosd_align &align)
{
	switch (position % 3)
	{
		case 0: align = XOSD_left; break;
		case 1: align = XOSD_center; break;
		case 2: align = XOSD_right; break;
	};
	switch (position / 3)
	{
		case 0: pos = XOSD_top; break;
		case 2: pos = XOSD_bottom; break;
		case 1:
		default:
				pos = XOSD_middle; break;
	};
}

void XOSDNotify::test(const QString &text, const struct TestConfig &config)
{
	kdebugf();
	if (testXOSD)
		destroyTest();

	testXOSD = xosd_create(1);

	xosd_pos pos;
	xosd_align align;
	position2PosAlign(config.position, pos, align);

	xosd_set_pos(testXOSD, pos);
	xosd_set_align(testXOSD, align);

	if (!config.font.isEmpty())
		xosd_set_font(testXOSD, config.font.local8Bit().data());
	xosd_set_colour(testXOSD, config.font_color.name().local8Bit().data());
	xosd_set_shadow_colour(testXOSD, config.shadow_color.name().local8Bit().data());
	xosd_set_outline_colour(testXOSD, config.outline_color.name().local8Bit().data());

	xosd_set_shadow_offset(testXOSD, config.shadow_offset);
	xosd_set_outline_offset(testXOSD, config.outline_offset);

	if (config.position >= 0 && config.position <= 8)
	{
		xosd_set_horizontal_offset(testXOSD, offsets[config.position].x);
		xosd_set_vertical_offset(testXOSD, offsets[config.position].y);
	}
	else
		kdebugm(KDEBUG_WARNING, "config.position: %d\n", config.position);

	xosd_display(testXOSD, 0, XOSD_string, text.local8Bit().data());

	QTimer::singleShot(1000*config.timeout, this, SLOT(destroyTest()));

	kdebugf2();
}

void XOSDNotify::addLine(int position, const QString &text, int timeout,
					const QString &font, const QColor &font_color,
					const QColor &shadow_color, int shadow_offset,
					const QColor &outline_color, int outline_offset)
{
	kdebugf();

	OSDLine line;
	if (position < 0 || position >= 9)
	{
		kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "end: position=%d\n", position);
		return;
	}
	if (timeout <= 0 || timeout > 2048)
	{
		kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "end: timeout: %d<=0\n", timeout);
		return;
	}
	line.font_size = getFontSize(font);
	if (line.font_size <= 0)
	{
		kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "end: font_size: %d<=0\n", timeout);
		return;
	}

	line.handle = xosd_create(1);
	line.text = text;
	line.timeout = timeout;
	line.font_color = font_color;
	line.outline_color = font_color;
	line.shadow_color = shadow_color;
	line.font = font;

	xosd_pos pos;
	xosd_align align;
	position2PosAlign(position, pos, align);

	xosd_set_pos(line.handle, pos);
	xosd_set_align(line.handle, align);

	if (!font.isEmpty())
		xosd_set_font(line.handle, font.local8Bit().data());
	if (font_color.isValid())
		xosd_set_colour(line.handle, font_color.name().local8Bit().data());
	if (shadow_color.isValid())
	{
		xosd_set_shadow_colour(line.handle, shadow_color.name().local8Bit().data());
		xosd_set_shadow_offset(line.handle, shadow_offset);
	}
	if (outline_color.isValid())
	{
		xosd_set_outline_colour(line.handle, outline_color.name().local8Bit().data());
		xosd_set_outline_offset(line.handle, outline_offset);
	}

	int x_offset = config_file.readNumEntry("XOSD", QString("OffsetX%1").arg(position));
	int y_offset = config_file.readNumEntry("XOSD", QString("OffsetY%1").arg(position));

	CONST_FOREACH(line, lines[position])
		y_offset += (*line).font_size + 1;

	xosd_set_horizontal_offset(line.handle, x_offset);
	xosd_set_vertical_offset(line.handle, y_offset);

	lines[position].append(line);
	xosd_display(line.handle, 0, XOSD_string, text.local8Bit().data());

	if (!timer->isActive())
		timer->start(1000);

	kdebugf2();
}

void XOSDNotify::refresh(int position)
{
	kdebugf();

	if (position < 0 || position >= 9)
	{
		kdebugmf(KDEBUG_FUNCTION_END|KDEBUG_WARNING, "end: position: %d<0 || %d>=9\n", position, position);
		return;
	}

	int y_offset = config_file.readNumEntry("XOSD", QString("OffsetY%1").arg(position));
	CONST_FOREACH(line, lines[position])
	{
		xosd_set_vertical_offset((*line).handle, y_offset);
		xosd_hide((*line).handle);
		xosd_show((*line).handle);
		y_offset += (*line).font_size + 1;
	}

	kdebugf2();
}

void XOSDNotify::oneSecond()
{
	kdebugf();

	int sum = 0;
	for (int k = 0; k < 9; ++k)
	{
		QValueList<OSDLine> &linesK = lines[k];

		const unsigned int count = linesK.count();
//		unsigned int i = count - 1;
		bool need_refresh = false;
		if (count == 0)
			continue;

		QValueListIterator<OSDLine> line = linesK.end();
		--line;
		for (unsigned int j = 0; j < count; ++j, --line)
			if ((*line).timeout-- < 0)
			{
				xosd_destroy((*line).handle);
				line = linesK.remove(line);
				need_refresh = true;
			}
		if (need_refresh)
			refresh(k);
		sum += count;
	}

	if (sum == 0)
		timer->stop();

	kdebugf2();
}

void XOSDNotify::destroyTest()
{
	kdebugf();

	if (testXOSD == NULL)
	{
		kdebugf2();
		return;
	}
	xosd_destroy(testXOSD);
	testXOSD = NULL;

	kdebugf2();
}

int XOSDNotify::getFontSize(const QString &xlfd)
{
	kdebugf();

	QStringList font = QStringList::split("-", xlfd, true);
	if (font.size() < 8)
	{
		kdebugf2();
		return -1;
	}
	else if (font[7] == "*" || font[7].isEmpty())
	{
		kdebugf2();
		return -1;
	}
	else
	{
		kdebugf2();
		return font[7].toInt();
	}
}

void XOSDNotify::clicked_Positions2(int id)
{
	kdebugf();
	XOffsetSpinBox->setValue(offsets[id].x);
	YOffsetSpinBox->setValue(offsets[id].y);
	kdebugf2();
}

void XOSDNotify::changed_XOffset(int value)
{
	kdebugf();
	offsets[positionComboBox->currentItem()].x = value;
	kdebugf2();
}

void XOSDNotify::changed_YOffset(int value)
{
	kdebugf();
	offsets[positionComboBox->currentItem()].y = value;
	kdebugf2();
}

void XOSDNotify::notify(Notification * notification)
{
	kdebugf();

	QString text;
	
	QString syntax = config_file.readEntry("XOSD", "Event_" + notification->type() + "_syntax", "");

	if ((config_file.readBoolEntry("XOSD", "NotifyUseSyntax"))&&(syntax!=""))
	{
		UserListElement ule;
		if (notification->userListElements().count())
			ule = notification->userListElements()[0];
		text = KaduParser::parse(syntax, ule, notification);
	}
	else
		if ((notification->title()==tr("Status changed")) && !(config_file.readBoolEntry("XOSD","NotifyWithDescription")))
		{		
			UserListElement ule;			
			if (notification->userListElements().count())
				ule = notification->userListElements()[0];
			text = KaduParser::parse(tr("<b>%a</b> changed status to <i>%s</i>"), ule, notification);
		}		
		else 
			text = notification->text();

	if ((config_file.readBoolEntry("XOSD", "ShowContentMessage"))&&(notification->details() != ""))
	{
		unsigned int citeSign = config_file.readUnsignedNumEntry("XOSD","CiteSign");
		QString cite = notification->details();
		if (cite.length() > citeSign)
			cite = cite.left(citeSign)+"...";
		text = text+ " " +cite;
	}
	
	text.remove("<b>");
	text.remove("</b>");
	text.remove("<i>");
	text.remove("</i>");
	text.remove("</small>");
	text.remove("<small>");
	text.remove("<br/>");
	text.replace("&nbsp;", " ");
	text.replace("&lt;", "<");
	text.replace("&gt;", ">");
	text.replace("&amp;", "&");

	addLine(config_file.readNumEntry("XOSD",notification->type() +"Position"),
			text,
			config_file.readNumEntry("XOSD",notification->type() +"Timeout"),
			config_file.readEntry("XOSD",notification->type() +"Font"),
			config_file.readColorEntry("XOSD",notification->type() +"FontColor"),
			config_file.readColorEntry("XOSD",notification->type() +"ShadowColor"),
			config_file.readNumEntry("XOSD",notification->type() +"ShadowOffset"),
			config_file.readColorEntry("XOSD",notification->type() +"OutlineColor"),
			config_file.readNumEntry("XOSD",notification->type() +"OutlineOffset"));

	kdebugf2();
}

void XOSDNotify::import_0_5_0_ConfigurationFromTo(const QString &from, const QString &to)
{

	config_file.addVariable("XOSD", to + "Position", config_file.readEntry("XOSD", from + "Position"));
	config_file.removeVariable("XOSD", from + "Position");
	config_file.addVariable("XOSD", to + "FontColor", config_file.readEntry("XOSD", from + "FontColor"));
	config_file.removeVariable("XOSD", from + "FontColor");
	config_file.addVariable("XOSD", to + "OutlineColor", config_file.readEntry("XOSD", from + "OutlineColor"));
	config_file.removeVariable("XOSD", from +"OutlineColor");
	config_file.addVariable("XOSD", to + "ShadowColor", config_file.readEntry("XOSD", from + "ShadowColor"));
	config_file.removeVariable("XOSD", from +"ShadowColor");
	config_file.addVariable("XOSD", to + "Font", config_file.readEntry("XOSD", from + "Font"));
	config_file.removeVariable("XOSD", from + "Font");
	config_file.addVariable("XOSD", to + "Timeout", config_file.readEntry("XOSD", from + "Timeout"));
	config_file.removeVariable("XOSD", from+ "Timeout");
	config_file.addVariable("XOSD", to + "OutlineOffset", config_file.readEntry("XOSD", from + "OutlineOffset"));
	config_file.removeVariable("XOSD", from + "OutlineOffset");
	config_file.addVariable("XOSD", to + "ShadowOffset", config_file.readEntry("XOSD", from + "ShadowOffset"));
	config_file.removeVariable("XOSD", from + "ShadowOffset");	
}

void XOSDNotify::import_0_5_0_configuration()
{
	QString entry;
	
	import_0_5_0_ConfigurationFromTo("Online", "StatusChanged/ToOnline");
	import_0_5_0_ConfigurationFromTo("Invisible", "StatusChanged/ToInvisible");
	import_0_5_0_ConfigurationFromTo("Offline", "StatusChanged/ToOffline");
	import_0_5_0_ConfigurationFromTo("Busy", "StatusChanged/ToBusy");
	import_0_5_0_ConfigurationFromTo("Error", "ConnectionError");
	config_file.addVariable("XOSD", "Event_StatusChanged/ToOnline_syntax", config_file.readEntry("XOSD","NotifySyntax"));
	config_file.addVariable("XOSD", "Event_StatusChanged/ToBusy_syntax", config_file.readEntry("XOSD","NotifySyntax"));
	config_file.addVariable("XOSD", "Event_StatusChanged/ToInvisible_syntax", config_file.readEntry("XOSD","NotifySyntax"));
	config_file.addVariable("XOSD", "Event_StatusChanged/ToOffline_syntax", config_file.readEntry("XOSD","NotifySyntax"));

}

void XOSDNotify::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	connect(mainConfigurationWindow->widgetById("XOSD/showContent"), SIGNAL(toggled(bool)), mainConfigurationWindow->widgetById("XOSD/showContentCount"), SLOT(setEnabled(bool)));
  	connect(mainConfigurationWindow->widgetById("XOSD/NotifyUseSyntax"), SIGNAL(toggled(bool)), mainConfigurationWindow->widgetById("XOSD/NotifySyntax"), SLOT(setEnabled(bool)));

	YOffsetSpinBox= dynamic_cast<ConfigSpinBox *>(mainConfigurationWindow->widgetById("XOSD/YOffset"));	
	XOffsetSpinBox= dynamic_cast<ConfigSpinBox *>(mainConfigurationWindow->widgetById("XOSD/XOffset"));
	positionComboBox= dynamic_cast<ConfigComboBox *>(mainConfigurationWindow->widgetById("XOSD/Position2"));

  	connect(positionComboBox, SIGNAL(activated(int)), this, SLOT(clicked_Positions2(int)));  	
	connect(YOffsetSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changed_YOffset(int)));
	connect(XOffsetSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changed_XOffset(int)));

	for(int i = 0; i < 9; ++i)
	{
		offsets[i].x = config_file.readNumEntry("XOSD", QString("OffsetX%1").arg(i));
		offsets[i].y = config_file.readNumEntry("XOSD", QString("OffsetY%1").arg(i));
	}
}

NotifierConfigurationWidget *XOSDNotify::createConfigurationWidget(QWidget *parent , char *name )
{
	return new XOSDConfigurationWidget(parent, name);
}

void XOSDNotify::configurationUpdated()
{
	kdebugf();

	for(int i = 0; i < 9; ++i)
	{
		config_file.writeEntry("XOSD", QString("OffsetX%1").arg(i), offsets[i].x);
		config_file.writeEntry("XOSD", QString("OffsetY%1").arg(i), offsets[i].y);
	}

	QString syntax = config_file.readEntry("XOSD", "NotifySyntax");
	config_file.writeEntry("XOSD",  "Event_StatusChanged/ToOnline_syntax", syntax);
	config_file.writeEntry("XOSD",  "Event_StatusChanged/ToBusy_syntax", syntax);
	config_file.writeEntry("XOSD",  "Event_StatusChanged/ToInvisible_syntax", syntax);
	config_file.writeEntry("XOSD",  "Event_StatusChanged/ToOffline_syntax", syntax);

	kdebugf2();
}

XLFDChooser *main_xlfd_chooser = NULL;
XOSDNotify *xosd_notify = NULL;
