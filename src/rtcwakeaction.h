/*
 *  rtcwakeaction.h  -  KAuth helper application to execute rtcwake commands
 *  Program:  kalarm
 *  SPDX-FileCopyrightText: 2011 David Jarvie <djarvie@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include <QObject>
#pragma once
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <KAuth>
#else
#include <KAuth/KAuth>
#endif

using namespace KAuth;

class RtcWakeAction : public QObject
{
    Q_OBJECT
public:
    RtcWakeAction();

public Q_SLOTS:
    KAuth::ActionReply settimer(const QVariantMap& args);
};

// vim: et sw=4:
