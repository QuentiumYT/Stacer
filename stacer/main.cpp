#include "app.h"

#include <QApplication>
#include <QDebug>
#include <QFontDatabase>
#include <QLocalServer>
#include <QLocalSocket>
#include <QSplashScreen>

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    Q_UNUSED(context)

    QString level;

    switch (type) {
        case QtDebugMsg:
            level = "DEBUG";
            break;
        case QtInfoMsg:
            level = "INFO";
            break;
        case QtWarningMsg:
            level = "WARNING";
            break;
        case QtCriticalMsg:
            level = "CRITICAL";
            break;
        case QtFatalMsg:
            level = "FATAL";
            break;
        default:
            level = "UNDEFINED";
            break;
    }

    if (type != QtWarningMsg) {

        QString text = QString("[%1] [%2] %3")
                           .arg(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss"))
                           .arg(level)
                           .arg(message);

        static QString logPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

        QFile file(logPath + "/stacer.log");

        QIODevice::OpenMode openMode = file.size() > (1L << 20) ? QIODevice::Truncate : QIODevice::Append;

        if (file.open(QIODevice::WriteOnly | openMode)) {
            QTextStream stream(&file);
            stream << text << Qt::endl;

            file.close();
        }
    }
}

const QString serverName = "StacerSingleInstanceServer";

void focusExistingInstance()
{
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if (socket.waitForConnected(100)) {
        socket.write("focus");
        socket.flush();
        socket.waitForBytesWritten(100);
        socket.disconnectFromServer();
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    qputenv("QT_ENABLE_HIGHDPI_SCALING", QByteArray("1"));
    QApplication app(argc, argv);

    focusExistingInstance();

    QLocalServer server;
    server.removeServer(serverName);
    if (!server.listen(serverName)) {
        qCritical("Unable to start single instance server.");
        return 1;
    }

    qApp->setApplicationName("stacer");
    qApp->setApplicationDisplayName("Stacer");
    qApp->setApplicationVersion("1.6.0");
    qApp->setWindowIcon(QIcon(":/static/logo.png"));

    {
        QCommandLineOption hideOption("hide", "Hide Stacer while launching.");
        QCommandLineOption noSplashOption("nosplash", "Hide splash screen while launching.");
        QCommandLineParser parser;
        parser.addVersionOption();
        parser.addHelpOption();
        parser.addOption(hideOption);
        parser.addOption(noSplashOption);
        parser.process(app);
    }

    bool isHide = false;
    bool isNoSplash = false;

    QLatin1String hideOption("--hide");
    QLatin1String noSplashOption("--nosplash");

    for (size_t i = 1; i < argc; ++i) {
        if (QString(argv[i]) == hideOption) {
            isHide = true;
        } else if (QString(argv[i]) == noSplashOption) {
            isNoSplash = true;
        }
    }

    QFontDatabase::addApplicationFont(":/static/font/Ubuntu-R.ttf");

    QPixmap pixSplash(":/static/splashscreen.png");

    QSplashScreen *splash = new QSplashScreen(pixSplash);

    if (!isNoSplash) {
        splash->show();
    }

    app.processEvents();

    App w;

    QObject::connect(&server, &QLocalServer::newConnection, [&]() {
        QLocalSocket *socket = server.nextPendingConnection();
        if (socket && socket->waitForReadyRead(100)) {
            QByteArray msg = socket->readAll();
            if (msg == "focus") {
                w.focusWindow();
            }
        }
        socket->disconnectFromServer();
        socket->close();
        socket->deleteLater();
    });

    if (argc < 2 || !isHide) {
        w.show();
    }

    splash->finish(&w);

    delete splash;

    return app.exec();
}
