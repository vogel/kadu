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

#include "widgets/paste-acceptor.h"

#include <QtTest/QtTest>

class PasteAcceptorTest : public QObject
{
    Q_OBJECT

private slots:
    void shouldHandleText();
    void shouldHandleUrlText();
    void shouldHandleLocalFileUrl();
    void shouldIgnoreRemoteFileUrl();
    void shouldIgnoreEmptyUrl();
    void shouldPrefertextOverLocalFileUrl();
    void shouldHandleImageData();
};

class PasteAcceptorStub : public PasteAcceptor
{
public:
    QString plainText;
    QUrl fileUrl;
    QByteArray imageData;

    virtual void acceptPlainText(QString plainText) override
    {
        this->plainText = plainText;
    }
    virtual void acceptFileUrl(QUrl fileUrl) override
    {
        this->fileUrl = fileUrl;
    }
    virtual void acceptImageData(QByteArray imageData) override
    {
        this->imageData = imageData;
    }
};

void PasteAcceptorTest::shouldHandleText()
{
    QMimeData mimeData{};
    mimeData.setData("text/plain", "plain text");
    mimeData.setData("text/html", "html text");

    auto pasteAcceptorStub = PasteAcceptorStub{};
    acceptPasteData(&mimeData, &pasteAcceptorStub);

    QCOMPARE(pasteAcceptorStub.plainText, QLatin1String("plain text"));
    QVERIFY(pasteAcceptorStub.fileUrl.isEmpty());
    QVERIFY(pasteAcceptorStub.imageData.isEmpty());
}

void PasteAcceptorTest::shouldHandleUrlText()
{
    QMimeData mimeData{};
    mimeData.setData("text/plain", "http://random.url.com/example");
    mimeData.setUrls(QList<QUrl>{} << QUrl{"http://random.url.com/example"});

    auto pasteAcceptorStub = PasteAcceptorStub{};
    acceptPasteData(&mimeData, &pasteAcceptorStub);

    QCOMPARE(pasteAcceptorStub.plainText, QLatin1String("http://random.url.com/example"));
    QVERIFY(pasteAcceptorStub.fileUrl.isEmpty());
    QVERIFY(pasteAcceptorStub.imageData.isEmpty());
}

void PasteAcceptorTest::shouldHandleLocalFileUrl()
{
    QMimeData mimeData{};
    mimeData.setUrls(QList<QUrl>{} << QUrl{"file:///local/file/path"});

    auto pasteAcceptorStub = PasteAcceptorStub{};
    acceptPasteData(&mimeData, &pasteAcceptorStub);

    QVERIFY(pasteAcceptorStub.plainText.isEmpty());
    QCOMPARE(pasteAcceptorStub.fileUrl, QUrl{"file:///local/file/path"});
    QVERIFY(pasteAcceptorStub.imageData.isEmpty());
}

void PasteAcceptorTest::shouldIgnoreRemoteFileUrl()
{
    QMimeData mimeData{};
    mimeData.setUrls(QList<QUrl>{} << QUrl{"http:///remote/file/path"});

    auto pasteAcceptorStub = PasteAcceptorStub{};
    acceptPasteData(&mimeData, &pasteAcceptorStub);

    QVERIFY(pasteAcceptorStub.plainText.isEmpty());
    QVERIFY(pasteAcceptorStub.fileUrl.isEmpty());
    QVERIFY(pasteAcceptorStub.imageData.isEmpty());
}

void PasteAcceptorTest::shouldIgnoreEmptyUrl()
{
    QMimeData mimeData{};
    mimeData.setUrls(QList<QUrl>{} << QUrl{});

    auto pasteAcceptorStub = PasteAcceptorStub{};
    acceptPasteData(&mimeData, &pasteAcceptorStub);

    QVERIFY(pasteAcceptorStub.plainText.isEmpty());
    QVERIFY(pasteAcceptorStub.fileUrl.isEmpty());
    QVERIFY(pasteAcceptorStub.imageData.isEmpty());
}

void PasteAcceptorTest::shouldPrefertextOverLocalFileUrl()
{
    QMimeData mimeData{};
    mimeData.setData("text/plain", "file:///local/file/path");
    mimeData.setUrls(QList<QUrl>{} << QUrl{"file:///local/file/path"});

    auto pasteAcceptorStub = PasteAcceptorStub{};
    acceptPasteData(&mimeData, &pasteAcceptorStub);

    QCOMPARE(pasteAcceptorStub.plainText, QLatin1String("file:///local/file/path"));
    QVERIFY(pasteAcceptorStub.fileUrl.isEmpty());
    QVERIFY(pasteAcceptorStub.imageData.isEmpty());
}

void PasteAcceptorTest::shouldHandleImageData()
{
    auto imageData = QByteArray{"imagedata"};
    QMimeData mimeData{};
    mimeData.setData("application/x-qt-image", imageData);

    auto pasteAcceptorStub = PasteAcceptorStub{};
    acceptPasteData(&mimeData, &pasteAcceptorStub);

    QVERIFY(pasteAcceptorStub.plainText.isEmpty());
    QVERIFY(pasteAcceptorStub.fileUrl.isEmpty());
    QCOMPARE(pasteAcceptorStub.imageData, imageData);
}

QTEST_APPLESS_MAIN(PasteAcceptorTest)
#include "paste-acceptor.test.moc"
