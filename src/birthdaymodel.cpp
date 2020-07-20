/*
 *  birthdaymodel.cpp  -  model class for birthdays from address book
 *  Program:  kalarm
 *  Copyright © 2009 by Tobias Koenig <tokoe@kde.org>
 *  Copyright © 2007-2020 David Jarvie <djarvie@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "birthdaymodel.h"

#include "resourcescalendar.h"

#include <KAlarmCal/KAEvent>

#include <AkonadiCore/ChangeRecorder>
#include <AkonadiCore/EntityDisplayAttribute>
#include <AkonadiCore/ItemFetchScope>
#include <AkonadiCore/Session>
#include <KContacts/Addressee>

#include <QLocale>

using namespace KAlarmCal;


BirthdayModel* BirthdayModel::mInstance = nullptr;

BirthdayModel::BirthdayModel(Akonadi::ChangeRecorder* recorder)
    : Akonadi::ContactsTreeModel(recorder)
{
    setColumns({FullName, Birthday});
}

BirthdayModel::~BirthdayModel()
{
    if (this == mInstance)
        mInstance = nullptr;
}

BirthdayModel* BirthdayModel::instance()
{
    if (!mInstance)
    {
        Akonadi::Session* session = new Akonadi::Session("KAlarm::BirthdayModelSession");

        Akonadi::ItemFetchScope scope;
        scope.fetchFullPayload(true);
        scope.fetchAttribute<Akonadi::EntityDisplayAttribute>();

        Akonadi::ChangeRecorder* recorder = new Akonadi::ChangeRecorder;
        recorder->setSession(session);
        recorder->fetchCollection(true);
        recorder->setItemFetchScope(scope);
        recorder->setCollectionMonitored(Akonadi::Collection::root());
        recorder->setMimeTypeMonitored(KContacts::Addressee::mimeType(), true);

        mInstance = new BirthdayModel(recorder);
    }

    return mInstance;
}

QVariant BirthdayModel::entityData(const Akonadi::Item& item, int column, int role) const
{
    if (columns().at(column) == Birthday  &&  role == Qt::DisplayRole)
    {
        const QDate date = Akonadi::ContactsTreeModel::entityData(item, column, DateRole).toDate();
        if (date.isValid())
            return QLocale().toString(date, QLocale::ShortFormat);
    }
    return Akonadi::ContactsTreeModel::entityData(item, column, role);
}

/*============================================================================*/

BirthdaySortModel::BirthdaySortModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
}

void BirthdaySortModel::setPrefixSuffix(const QString& prefix, const QString& suffix)
{
    mContactsWithAlarm.clear();
    mPrefix = prefix;
    mSuffix = suffix;

    const QVector<KAEvent> events = ResourcesCalendar::instance()->events(CalEvent::ACTIVE);
    for (const KAEvent& event : events)
    {
        if (event.actionSubType() == KAEvent::MESSAGE
        &&  event.recurType() == KARecurrence::ANNUAL_DATE
        &&  (prefix.isEmpty()  ||  event.message().startsWith(prefix)))
            mContactsWithAlarm.append(event.message());
    }

    invalidateFilter();
}

bool BirthdaySortModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    const QModelIndex nameIndex = sourceModel()->index(sourceRow, 0, sourceParent);
    const QModelIndex birthdayIndex = sourceModel()->index(sourceRow, 1, sourceParent);

    // If the birthday is invalid, the second column is empty
    if (birthdayIndex.data(Qt::DisplayRole).toString().isEmpty())
        return false;

    const QString text = mPrefix + nameIndex.data(Qt::DisplayRole).toString() + mSuffix;
    if (mContactsWithAlarm.contains(text))
        return false;

    return true;
}

// vim: et sw=4:
