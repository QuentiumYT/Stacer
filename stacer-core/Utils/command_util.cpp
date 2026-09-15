#include "command_util.h"

#include <QDebug>
#include <QProcess>
#include <QStandardPaths>
#include <QTextStream>

#include <memory>

QString CommandUtil::sudoExec(const QString &cmd, QStringList args, QByteArray data)
{
    args.push_front(cmd);

    QString result("");

    qDebug() << "Executing sudo command:" << cmd << "with arguments:" << args;

    result = CommandUtil::exec("pkexec", args, data, true);

    return result;
}

QString CommandUtil::exec(const QString &cmd, QStringList args, QByteArray data, bool checkExitCode)
{
    std::unique_ptr<QProcess> process(new QProcess());
    process->start(cmd, args);

    if (!data.isEmpty()) {
        process->write(data);
        process->waitForBytesWritten();
        process->closeWriteChannel();
    }

    // 10 minutes
    process->waitForFinished(600 * 1000);

    QTextStream stdOut(process->readAllStandardOutput());

    QString err = process->errorString();

    process->kill();
    process->close();

    if (process->error() != QProcess::UnknownError)
        throw err;

    if (checkExitCode && process->exitCode() != 0)
        throw QString("Command exited with code %1").arg(process->exitCode());

    return stdOut.readAll().trimmed();
}

bool CommandUtil::isAptRpm()
{
    if (!isExecutable("apt-get") || !isExecutable("rpm")) {
        return false;
    }

    if (isExecutable("apt-repo")) {
        return true;
    }

    try {
        QString version = exec("apt-get", { "--version" });
        return version.contains("rpm interface", Qt::CaseInsensitive);
    } catch (const QString &) {
        return false;
    }
}

bool CommandUtil::isExecutable(const QString &cmd)
{
    return !QStandardPaths::findExecutable(cmd).isEmpty();
}
