/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef HINTS_CONFIGURATION_WIDGET_H
#define HINTS_CONFIGURATION_WIDGET_H

#include "../notify/notify.h"

class QLineEdit;
class QSpinBox;

struct HintProperties
{
	QString eventName;

	QFont font;
	QColor foregroundColor;
	QColor backgroundColor;
	unsigned int timeout;
	QString syntax;
};

class HintsConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT

	QMap<QString, HintProperties> hintProperties;
	HintProperties currentProperties;
	QString currentNotifyEvent;

	QLabel *warning;
	QLabel *preview;
	SelectFont *font;
	ColorButton *foregroundColor;
	ColorButton *backgroundColor;
	QSpinBox *timeout;
	QLineEdit *syntax;

private slots:
	void fontChanged(QFont font);
	void foregroundColorChanged(const QColor &color);
	void backgroundColorChanged(const QColor &color);
	void timeoutChanged(int timeout);
	void syntaxChanged(const QString &syntax);

public:
	HintsConfigurationWidget(QWidget *parent = 0, char *name = 0);

	virtual void loadNotifyConfigurations() {}
	virtual void saveNotifyConfigurations();

	virtual void switchToEvent(const QString &event);

public slots:
	void setAllEnabled(bool enabled);

};

#endif // HINTS_CONFIGURATION_WIDGET_H
