/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef AUTOAWAY_H
#define AUTOAWAY_H

#include <QtCore/QTimer>
#include <QtCore/QObject>

#include "gui/windows/main-configuration-window.h"
#include "status/status-changer.h"

#include "autoaway-status-changer.h"

class QLineEdit;
class QSpinBox;
class QCheckBox;

/**
 * @defgroup autoaway Autoaway
 * @{
 */
class AutoAway : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	AutoAwayStatusChanger *autoAwayStatusChanger;
	QTimer *timer;

	unsigned int checkInterval;

	unsigned int autoAwayTime;
	unsigned int autoDisconnectTime;
	unsigned int autoInvisibleTime;

	bool autoAwayEnabled;
	bool autoInvisibleEnabled;
	bool autoDisconnectEnabled;
	bool parseAutoStatus;

	bool updateDescripion;

	unsigned int idleTime;
	unsigned int refreshStatusTime;
	unsigned int refreshStatusInterval;

	QSpinBox *autoAwaySpinBox;
	QSpinBox *autoInvisibleSpinBox;
	QSpinBox *autoOfflineSpinBox;
	QSpinBox *autoRefreshSpinBox;

	QLineEdit *descriptionTextLineEdit;

	QCheckBox *parseStatusCheckBox;

	QString autoStatusText;

	AutoAwayStatusChanger::ChangeDescriptionTo changeTo;

	QString parseDescription(const QString &parseDescription);

	void createDefaultConfiguration();

private slots:
	void checkIdleTime();

	void autoAwaySpinBoxValueChanged(int value);
	void autoInvisibleSpinBoxValueChanged(int value);
	void autoOfflineSpinBoxValueChanged(int value);

	void descriptionChangeChanged(int index);

protected:
	virtual void configurationUpdated();

public:
	AutoAway();
	virtual ~AutoAway();

	QString changeDescription(const QString &oldDescription);

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

};

/** @} */

#endif
