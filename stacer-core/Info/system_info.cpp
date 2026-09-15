#include "system_info.h"

#include <QDebug>
#include <QObject>
#include <QRegularExpression>

SystemInfo::SystemInfo()
{
    QString unknown(QObject::tr("Unknown"));
    QString model;
    QString speed;

    try {
        const QStringList lines = CommandUtil::exec("bash", { "-c", LSCPU_COMMAND }).split('\n');

        QRegularExpression regexp("\\s+");
        QString space(" ");

        QStringList filterModel = lines.filter(QRegularExpression("^Model name"));
        QString modelLine = filterModel.isEmpty() ? "error: unknown" : filterModel.first();

        QStringList filterSpeed = lines.filter(QRegularExpression("^CPU max MHz"));
        if (filterSpeed.isEmpty()) {
            // fallback to CPU MHz (old lscpu versions)
            filterSpeed = lines.filter(QRegularExpression("^CPU MHz"));
        }
        if (filterSpeed.isEmpty()) {
            // fallback to /proc/cpuinfo (no frequency in lscpu)
            filterSpeed = FileUtil::readListFromFile(PROC_CPUINFO)
                              .filter(QRegularExpression("^cpu MHz"));
        }
        QString speedLine = filterSpeed.isEmpty() ? "error: unknown" : filterSpeed.first();

        model = modelLine.split(":").last();
        speed = speedLine.split(":").last().replace(",", ".");

        model = model.contains('@') ? model.split("@").first() : model;
        speed = !speed.contains("unknown") ? QString::number(speed.toDouble() / 1000.0, 'g', 3).append(" GHz") : speed;

        this->cpuModel = model.trimmed().replace(regexp, space);
        this->cpuSpeed = speed.trimmed().replace(regexp, space);
    } catch (const QString &ex) {
        this->cpuModel = unknown;
        this->cpuSpeed = unknown;
    }

    CpuInfo ci;
    this->cpuCore = QString::number(ci.getCpuPhysicalCoreCount());

    // get username
    QString name = qgetenv("USER");

    if (name.isEmpty())
        name = qgetenv("USERNAME");

    try {
        if (name.isEmpty())
            name = CommandUtil::exec("whoami").trimmed();
    } catch (const QString &ex) {
        qCritical() << ex;
    }

    this->username = name;
}

QString SystemInfo::getUsername() const
{
    return username;
}

QString SystemInfo::getHostname() const
{
    return QSysInfo::machineHostName();
}

QStringList SystemInfo::getUserList() const
{
    QStringList passwdUsers = FileUtil::readListFromFile("/etc/passwd");
    QStringList users;

    for (QString &row : passwdUsers) {
        users.append(row.split(":").at(0));
    }

    return users;
}

QStringList SystemInfo::getGroupList() const
{
    QStringList groupFile = FileUtil::readListFromFile("/etc/group");
    QStringList groups;

    for (QString &row : groupFile) {
        groups.append(row.split(":").at(0));
    }

    return groups;
}

QString SystemInfo::getPlatform() const
{
    return QString("%1 %2")
        .arg(QSysInfo::kernelType())
        .arg(QSysInfo::currentCpuArchitecture());
}

QString SystemInfo::getDistribution() const
{
    return QSysInfo::prettyProductName();
}

QString SystemInfo::getKernel() const
{
    return QSysInfo::kernelVersion();
}

QString SystemInfo::getCpuModel() const
{
    return this->cpuModel;
}

QString SystemInfo::getCpuSpeed() const
{
    return this->cpuSpeed;
}

QString SystemInfo::getCpuCore() const
{
    return this->cpuCore;
}

QFileInfoList SystemInfo::getCrashReports() const
{
    QDir reports("/var/crash");

    return reports.entryInfoList(QDir::Files);
}

QFileInfoList SystemInfo::getAppLogs() const
{
    QDir logs("/var/log");

    // remove only files not directory ex. apache2 (log directory)
    return logs.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
}

QFileInfoList SystemInfo::getAppCaches() const
{
    QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

    // Main cache location (only files and folders)
    QList cacheExclusions = {
        homePath + "/.cache/p10k"
    };
    QFileInfoList cacheFiles = QDir(homePath + "/.cache").entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    // Common cache locations
    QList cacheLocations = {
        homePath + "/.npm/_cacache",
        homePath + "/.bun/install/cache",
        homePath + "/.m2/repository",
        homePath + "/.gradle/caches",
        homePath + "/.cargo/registry",
        homePath + "/.expo/versions-cache",
        homePath + "/.expo/native-modules-cache",
    };

    // Find .config/<folder>/Cache and .config/<folder>/GPUCache
    QDir configDir(homePath + "/.config");
    if (configDir.exists()) {
        QStringList configFolders = configDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString &folder : configFolders) {
            QString cachePath = configDir.filePath(folder + "/Cache");
            QString gpuCachePath = configDir.filePath(folder + "/GPUCache");
            QString codeCachePath = configDir.filePath(folder + "/Code Cache");
            QString dawnCachePath = configDir.filePath(folder + "/DawnCache");
            if (QDir(cachePath).exists()) {
                cacheLocations.append(cachePath);
            }
            if (QDir(gpuCachePath).exists()) {
                cacheLocations.append(gpuCachePath);
            }
            if (QDir(codeCachePath).exists()) {
                cacheLocations.append(codeCachePath);
            }
            if (QDir(dawnCachePath).exists()) {
                cacheLocations.append(dawnCachePath);
            }
        }
    }

    for (const QString &location : cacheLocations) {
        if (QDir(location).exists()) {
            // cacheFiles.append(QDir(location).entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot));
            cacheFiles.append(QFileInfo(location));
        }
    }

    for (const QString &exclusion : cacheExclusions) {
        for (int i = cacheFiles.size() - 1; i >= 0; --i) {
            if (cacheFiles[i].absoluteFilePath().contains(exclusion)) {
                cacheFiles.removeAt(i);
            }
        }
    }

    return cacheFiles;
}
