/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef HINTS_CONFIGURATION_UI_HANDLER
#define HINTS_CONFIGURATION_UI_HANDLER

#include "gui/windows/main-configuration-window.h"

#include "hint_manager.h"

class QComboBox;
class QSpinBox;

class HintsConfigurationUiHandler : public ConfigurationUiHandler
{
	Q_OBJECT

private:
	QFrame *previewHintsFrame;
	QLayout *previewHintsLayout;

	QSpinBox *minimumWidth;
	QSpinBox *maximumWidth;
	QSpinBox *xPosition;
	QSpinBox *yPosition;
	QCheckBox *ownPosition;
	QComboBox *ownPositionCorner;
	
	HintOverUserConfigurationWindow *overUserConfigurationWindow;
	QFrame *overUserConfigurationPreview;
	QLabel *overUserConfigurationIconLabel;
	QLabel *overUserConfigurationTipLabel;
	
	QPushButton *configureOverUserHint;

private slots:
	void minimumWidthChanged(int value);
	void maximumWidthChanged(int value);
	
	void toolTipClassesHighlighted(const QString &value);

	void mainConfigurationWindowDestroyed();
	void hintOverUserConfigurationWindowDestroyed();
	void showOverUserConfigurationWindow();
	void updateOverUserPreview();
	void addHintsPreview();
	void updateHintsPreview();
	void deleteHintsPreview(Hint *hint);
	void deleteAllHintsPreview();

signals:
	void searchingForTrayPosition(QPoint &pos);

public:
	explicit HintsConfigurationUiHandler(QObject *parent = 0);
	virtual ~HintsConfigurationUiHandler();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
};

#endif
