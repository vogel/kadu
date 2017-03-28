/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QFile>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtXml/QDomElement>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/injected-factory.h"
#include "icons/kadu-icon.h"
#include "misc/paths-provider.h"
#include "widgets/configuration/config-action-button.h"
#include "widgets/configuration/config-check-box.h"
#include "widgets/configuration/config-color-button.h"
#include "widgets/configuration/config-combo-box.h"
#include "widgets/configuration/config-gg-password-edit.h"
#include "widgets/configuration/config-group-box.h"
#include "widgets/configuration/config-hot-key-edit.h"
#include "widgets/configuration/config-label.h"
#include "widgets/configuration/config-line-edit.h"
#include "widgets/configuration/config-line-separator.h"
#include "widgets/configuration/config-list-widget.h"
#include "widgets/configuration/config-path-list-edit.h"
#include "widgets/configuration/config-preview.h"
#include "widgets/configuration/config-radio-button.h"
#include "widgets/configuration/config-section.h"
#include "widgets/configuration/config-select-file.h"
#include "widgets/configuration/config-select-font.h"
#include "widgets/configuration/config-slider.h"
#include "widgets/configuration/config-spin-box.h"
#include "widgets/configuration/config-syntax-editor.h"
#include "widgets/configuration/config-widget.h"
#include "widgets/configuration/configuration-widget.h"
#include "windows/configuration-window.h"

ConfigurationWidget::ConfigurationWidget(ConfigurationWindowDataManager *dataManager, QWidget *parent)
        : QWidget(parent), DataManager(dataManager), CurrentSection(0)
{
    QHBoxLayout *center_layout = new QHBoxLayout(this);
    center_layout->setMargin(0);
    center_layout->setSpacing(0);

    LeftWidget = new QWidget(this);
    LeftWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    LeftWidget->hide();
    QVBoxLayout *left_layout = new QVBoxLayout(LeftWidget);
    left_layout->setMargin(0);
    left_layout->setSpacing(0);

    ContainerWidget = new QWidget(this);
    new QHBoxLayout(ContainerWidget);

    SectionsListWidget = new QListWidget(LeftWidget);
    SectionsListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    SectionsListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    SectionsListWidget->setIconSize(QSize(32, 32));
    connect(
        SectionsListWidget, SIGNAL(currentTextChanged(const QString &)), this, SLOT(changeSection(const QString &)));
    left_layout->addWidget(SectionsListWidget);

    center_layout->addWidget(LeftWidget);
    center_layout->addWidget(ContainerWidget);
}

ConfigurationWidget::~ConfigurationWidget()
{
    if (SectionsListWidget->currentItem())
        m_configuration->deprecatedApi()->writeEntry(
            "General", "ConfigurationWindow_" + Name, SectionsListWidget->currentItem()->text());

    disconnect(SectionsListWidget, 0, this, 0);

    // qDeleteAll() won't work here because of connection to destroyed() signal
    foreach (const ConfigSection *cs, ConfigSections)
    {
        disconnect(cs, SIGNAL(destroyed(QObject *)), this, SLOT(configSectionDestroyed(QObject *)));
        delete cs;
    }
}

void ConfigurationWidget::setConfiguration(Configuration *configuration)
{
    m_configuration = configuration;
}

void ConfigurationWidget::setIconsManager(IconsManager *iconsManager)
{
    m_iconsManager = iconsManager;
}

void ConfigurationWidget::setInjectedFactory(InjectedFactory *injectedFactory)
{
    m_injectedFactory = injectedFactory;
}

void ConfigurationWidget::setPathsProvider(PathsProvider *pathsProvider)
{
    m_pathsProvider = pathsProvider;
}

void ConfigurationWidget::beforeShow()
{
    auto lastSection = m_configuration->deprecatedApi()->readEntry("General", "ConfigurationWindow_" + Name);
    auto section = ConfigSections.value(lastSection);
    if (section)
        section->activate();
    else if (SectionsListWidget->count() > 0)
    {
        auto firstSection = ConfigSections.value(SectionsListWidget->item(0)->text());
        if (firstSection)
            firstSection->activate();
    }
}

QList<ConfigWidget *> ConfigurationWidget::appendUiFile(const QString &fileName, bool load)
{
    QList<ConfigWidget *> widgets = processUiFile(fileName, true);

    foreach (ConfigWidget *widget, widgets)
    {
        if (!widget)
            continue;

        QWidget *currentWidget = widgetById(widget->currentWidgetId());
        QWidget *parentWidget = widgetById(widget->parentWidgetId());

        if (parentWidget && currentWidget)
        {
            const char *slot = widget->isStateDependentDirectly() ? SLOT(setEnabled(bool)) : SLOT(setDisabled(bool));

            connect(parentWidget, SIGNAL(toggled(bool)), currentWidget, slot);
        }

        if (load)
            widget->loadConfiguration();
    }

    return widgets;
}

void ConfigurationWidget::removeUiFile(const QString &fileName)
{
    processUiFile(fileName, false);
}

QList<ConfigWidget *> ConfigurationWidget::processUiFile(const QString &fileName, bool append)
{
    QList<ConfigWidget *> result;
    QFile file(fileName);

    QDomDocument uiFile;
    if (!file.open(QIODevice::ReadOnly))
        return result;

    if (!uiFile.setContent(&file))
    {
        file.close();
        return result;
    }

    file.close();

    QDomElement kaduConfigurationUi = uiFile.documentElement();
    if (kaduConfigurationUi.tagName() != "configuration-ui")
    {
        return result;
    }

    QDomNodeList children = kaduConfigurationUi.childNodes();
    uint length = children.length();
    for (uint i = 0; i < length; i++)
        result += processUiSectionFromDom(children.item(static_cast<int>(i)), append);

    return result;
}

QList<ConfigWidget *> ConfigurationWidget::processUiSectionFromDom(QDomNode sectionNode, bool append)
{
    QList<ConfigWidget *> result;
    if (!sectionNode.isElement())
    {
        return result;
    }

    const QDomElement &sectionElement = sectionNode.toElement();
    if (sectionElement.tagName() != "section")
    {
        return result;
    }

    const QString &sectionName = sectionElement.attribute("name");
    if (sectionName.isEmpty())
    {
        return result;
    }

    QString iconPath = sectionElement.attribute("icon");
    // Additional slash is needed so that QUrl would treat the rest as _path_, which is desired here.
    if (iconPath.startsWith("datapath:///"))
        iconPath = m_pathsProvider->dataPath() + iconPath.mid(static_cast<int>(qstrlen("datapath:///")));
    configSection(KaduIcon(iconPath), QCoreApplication::translate("@default", sectionName.toUtf8().constData()), true);

    const QDomNodeList children = sectionElement.childNodes();
    uint length = children.length();
    for (uint i = 0; i < length; i++)
        result += processUiTabFromDom(children.item(static_cast<int>(i)), sectionName, append);

    return result;
}

QList<ConfigWidget *>
ConfigurationWidget::processUiTabFromDom(QDomNode tabNode, const QString &sectionName, bool append)
{
    QList<ConfigWidget *> result;
    if (!tabNode.isElement())
    {
        return result;
    }

    const QDomElement &tabElement = tabNode.toElement();
    if (tabElement.tagName() != "tab")
    {
        return result;
    }

    const QString tabName = tabElement.attribute("name");
    if (tabName.isEmpty())
    {
        return result;
    }

    const QDomNodeList &children = tabElement.childNodes();
    uint length = children.length();
    for (uint i = 0; i < length; i++)
        result += processUiGroupBoxFromDom(children.item(static_cast<int>(i)), sectionName, tabName, append);

    return result;
}

QList<ConfigWidget *> ConfigurationWidget::processUiGroupBoxFromDom(
    QDomNode groupBoxNode, const QString &sectionName, const QString &tabName, bool append)
{
    QList<ConfigWidget *> result;
    if (!groupBoxNode.isElement())
    {
        return result;
    }

    const QDomElement &groupBoxElement = groupBoxNode.toElement();
    if (groupBoxElement.tagName() != "group-box")
    {
        return result;
    }

    const QString groupBoxName = groupBoxElement.attribute("name");
    if (groupBoxName.isEmpty())
    {
        return result;
    }

    const QString groupBoxId = groupBoxElement.attribute("id");

    ConfigGroupBox *configGroupBoxWidget = configGroupBox(sectionName, tabName, groupBoxName, append);
    if (!configGroupBoxWidget)
    {
        return result;
    }

    if (append)
        configGroupBoxWidget->ref();

    if (!groupBoxId.isEmpty())
        Widgets.insert(groupBoxId, configGroupBoxWidget->widget());

    const QDomNodeList &children = groupBoxElement.childNodes();
    uint length = children.length();
    for (uint i = 0; i < length; i++)
        if (append)
            result.append(appendUiElementFromDom(children.item(static_cast<int>(i)), configGroupBoxWidget));
        else
            removeUiElementFromDom(children.item(static_cast<int>(i)), configGroupBoxWidget);

    // delete unused even if length == 0
    if (!append)
        if (!configGroupBoxWidget->deref())
            delete configGroupBoxWidget;

    return result;
}

ConfigWidget *ConfigurationWidget::appendUiElementFromDom(QDomNode uiElementNode, ConfigGroupBox *configGroupBox)
{
    if (!uiElementNode.isElement())
    {
        return 0;
    }

    const QDomElement &uiElement = uiElementNode.toElement();
    const QString &tagName = uiElement.tagName();
    ConfigWidget *widget = 0;

    if (tagName == "line-edit")
        widget = m_injectedFactory->makeInjected<ConfigLineEdit>(configGroupBox, DataManager);
    else if (tagName == "gg-password-edit")
        widget = m_injectedFactory->makeInjected<ConfigGGPasswordEdit>(configGroupBox, DataManager);
    else if (tagName == "check-box")
        widget = m_injectedFactory->makeInjected<ConfigCheckBox>(configGroupBox, DataManager);
    else if (tagName == "radio-button")
        widget = m_injectedFactory->makeInjected<ConfigRadioButton>(configGroupBox, DataManager);
    else if (tagName == "spin-box")
        widget = m_injectedFactory->makeInjected<ConfigSpinBox>(configGroupBox, DataManager);
    else if (tagName == "combo-box")
        widget = m_injectedFactory->makeInjected<ConfigComboBox>(configGroupBox, DataManager);
    else if (tagName == "hot-key-edit")
        widget = m_injectedFactory->makeInjected<ConfigHotKeyEdit>(configGroupBox, DataManager);
    else if (tagName == "path-list-edit")
        widget = m_injectedFactory->makeInjected<ConfigPathListEdit>(configGroupBox, DataManager);
    else if (tagName == "color-button")
        widget = m_injectedFactory->makeInjected<ConfigColorButton>(configGroupBox, DataManager);
    else if (tagName == "select-font")
        widget = m_injectedFactory->makeInjected<ConfigSelectFont>(configGroupBox, DataManager);
    else if (tagName == "syntax-editor")
        widget = m_injectedFactory->makeInjected<ConfigSyntaxEditor>(configGroupBox, DataManager);
    else if (tagName == "action-button")
        widget = m_injectedFactory->makeInjected<ConfigActionButton>(configGroupBox, DataManager);
    else if (tagName == "select-file")
        widget = m_injectedFactory->makeInjected<ConfigSelectFile>(configGroupBox, DataManager);
    else if (tagName == "preview")
        widget = m_injectedFactory->makeInjected<ConfigPreview>(configGroupBox, DataManager);
    else if (tagName == "proxy-combo-box")
        widget = m_injectedFactory->makeInjected<ConfigPreview>(configGroupBox, DataManager);
    else if (tagName == "slider")
        widget = m_injectedFactory->makeInjected<ConfigSlider>(configGroupBox, DataManager);
    else if (tagName == "label")
        widget = m_injectedFactory->makeInjected<ConfigLabel>(configGroupBox, DataManager);
    else if (tagName == "list-box")
        widget = m_injectedFactory->makeInjected<ConfigListWidget>(configGroupBox, DataManager);
    else if (tagName == "line-separator")
        widget = m_injectedFactory->makeInjected<ConfigLineSeparator>(configGroupBox, DataManager);
    else
    {
        return 0;
    }

    if (!widget->fromDomElement(uiElement))
    {
        delete widget;
        return 0;
    }

    QString id = uiElement.attribute("id");
    if (!id.isEmpty())
        Widgets.insert(id, dynamic_cast<QWidget *>(widget));

    return widget;
}

void ConfigurationWidget::removeUiElementFromDom(QDomNode uiElementNode, ConfigGroupBox *configGroupBox)
{
    if (!uiElementNode.isElement())
    {
        return;
    }

    const QDomElement &uiElement = uiElementNode.toElement();
    const QString &caption = uiElement.attribute("caption");

    foreach (QObject *child, configGroupBox->widget()->children())
    {
        ConfigWidget *configWidget = dynamic_cast<ConfigWidget *>(child);
        if (!configWidget)
            continue;

        if (configWidget->widgetCaption == caption)
        {
            delete configWidget;
            break;
        }
    }
}

QWidget *ConfigurationWidget::widgetById(const QString &id)
{
    if (Widgets.contains(id))
        return Widgets.value(id);

    return 0;
}

ConfigGroupBox *
ConfigurationWidget::configGroupBox(const QString &section, const QString &tab, const QString &groupBox, bool create)
{
    ConfigSection *s = configSection(section);
    if (!s)
        return 0;

    return s->configGroupBox(
        QCoreApplication::translate("@default", tab.toUtf8().constData()),
        QCoreApplication::translate("@default", groupBox.toUtf8().constData()), create);
}

ConfigSection *ConfigurationWidget::configSection(const QString &name)
{
    return ConfigSections.value(QCoreApplication::translate("@default", name.toUtf8().constData()));
}

ConfigSection *ConfigurationWidget::configSection(const KaduIcon &icon, const QString &name, bool create)
{
    if (ConfigSections.contains(name))
        return ConfigSections.value(name);

    if (!create)
        return 0;

    QListWidgetItem *newConfigSectionListWidgetItem =
        new QListWidgetItem(m_iconsManager->iconByPath(icon), name, SectionsListWidget);

    QFontMetrics fontMetrics = SectionsListWidget->fontMetrics();
    // TODO: 48 = margins + scrollbar - get real scrollbar width
    int width = fontMetrics.width(name) + 80;

    auto newConfigSection = m_injectedFactory->makeInjected<ConfigSection>(
        name, this, newConfigSectionListWidgetItem, ContainerWidget, icon);
    ConfigSections.insert(name, newConfigSection);
    connect(newConfigSection, SIGNAL(destroyed(QObject *)), this, SLOT(configSectionDestroyed(QObject *)));

    if (ConfigSections.count() == 1)
        SectionsListWidget->setFixedWidth(width);

    if (ConfigSections.count() > 1)
    {
        if (SectionsListWidget->width() < width)
            SectionsListWidget->setFixedWidth(width);
        LeftWidget->show();
    }

    return newConfigSection;
}

void ConfigurationWidget::loadConfiguration(QObject *object)
{
    if (!object)
        return;

    const QObjectList children = object->children();
    foreach (QObject *child, children)
        loadConfiguration(child);

    ConfigWidget *configWidget = dynamic_cast<ConfigWidget *>(object);
    if (configWidget)
        configWidget->loadConfiguration();
}

void ConfigurationWidget::loadConfiguration()
{
    loadConfiguration(this);
}

void ConfigurationWidget::saveConfiguration(QObject *object)
{
    if (!object)
        return;

    const QObjectList children = object->children();
    foreach (QObject *child, children)
        saveConfiguration(child);

    ConfigWidget *configWidget = dynamic_cast<ConfigWidget *>(object);
    if (configWidget)
        configWidget->saveConfiguration();
}

void ConfigurationWidget::saveConfiguration()
{
    saveConfiguration(this);
}

void ConfigurationWidget::changeSection(const QString &newSectionName)
{
    if (!ConfigSections.contains(newSectionName))
        return;

    ConfigSection *newSection = ConfigSections.value(newSectionName);
    if (newSection == CurrentSection)
        return;

    if (CurrentSection)
        CurrentSection->hide();

    CurrentSection = newSection;

    if (CurrentSection)
    {
        CurrentSection->show();
        CurrentSection->activate();
    }
}

void ConfigurationWidget::configSectionDestroyed(QObject *obj)
{
    // see ConfigSection::~ConfigSection()
    disconnect(obj, SIGNAL(destroyed(QObject *)), this, SLOT(configSectionDestroyed(QObject *)));

    ConfigSections.remove(static_cast<ConfigSection *>(obj)->name());

    if (CurrentSection == obj)
        CurrentSection = 0;
}

#include "moc_configuration-widget.cpp"
