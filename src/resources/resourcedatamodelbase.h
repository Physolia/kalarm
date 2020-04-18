/*
 *  resourcedatamodelbase.h  -  base for models containing calendars and events
 *  Program:  kalarm
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

#ifndef RESOURCEDATAMODELBASE_H
#define RESOURCEDATAMODELBASE_H

#include "resourcetype.h"

#include "preferences.h"

#include <KAlarmCal/KACalendar>

#include <QSize>

class Resource;
class QModelIndex;
class QPixmap;

namespace KAlarmCal { class KAEvent; }

using namespace KAlarmCal;


/*=============================================================================
= Class: ResourceDataModelBase
= Base class for models containing all calendars and events.
=============================================================================*/
class ResourceDataModelBase
{
public:
    /** Data column numbers. */
    enum
    {
        // Item columns
        TimeColumn = 0, TimeToColumn, RepeatColumn, ColourColumn, TypeColumn, TextColumn,
        TemplateNameColumn,
        ColumnCount
    };
    /** Additional model data roles. */
    enum
    {
        UserRole = Qt::UserRole + 500,   // copied from Akonadi::EntityTreeModel
        ItemTypeRole = UserRole,   // item's type: calendar or event
        // Calendar roles
        ResourceIdRole,            // the resource ID
        BaseColourRole,            // background colour ignoring collection colour
        // Event roles
        EventIdRole,               // the event ID
        ParentResourceIdRole,      // the parent resource ID of the event
        EnabledRole,               // true for enabled alarm, false for disabled
        StatusRole,                // KAEvent::ACTIVE/ARCHIVED/TEMPLATE
        AlarmActionsRole,          // KAEvent::Actions
        AlarmSubActionRole,        // KAEvent::Action
        ValueRole,                 // numeric value
        SortRole,                  // the value to use for sorting
        TimeDisplayRole,           // time column value with '~' representing omitted leading zeroes
        ColumnTitleRole,           // column titles (whether displayed or not)
        CommandErrorRole           // last command execution error for alarm (per user)
    };
    /** The type of a model row. */
    enum class Type { Error = 0, Event, Resource };

    virtual ~ResourceDataModelBase();

public:
    /** Return the data storage backend type used by this model. */
    virtual Preferences::Backend dataStorageBackend() const = 0;

    static QSize   iconSize()       { return mIconSize; }

    /** Return a bulleted list of alarm types for inclusion in an i18n message. */
    static QString typeListForDisplay(CalEvent::Types);

    /** Get the tooltip for a resource. The resource's enabled status is
     *  evaluated for specified alarm types. */
    QString tooltip(const Resource&, CalEvent::Types) const;

    /** Return the read-only status tooltip for a resource.
     * A null string is returned if the resource is fully writable. */
    static QString readOnlyTooltip(const Resource&);

    /** Return offset to add to headerData() role, for item models. */
    virtual int headerDataEventRoleOffset() const  { return 0; }

    /** Return whether calendar migration/creation at initialisation has completed. */
    bool isMigrationComplete() const;

protected:
    ResourceDataModelBase();

    static QVariant headerData(int section, Qt::Orientation, int role, bool eventHeaders, bool& handled);

    /** Return whether resourceData() and/or eventData() handle a role. */
    bool roleHandled(int role) const;

    /** Return the model data for a resource.
     *  @param role     may be updated for calling the base model.
     *  @param handled  updated to true if the reply is valid, else set to false.
     */
    QVariant resourceData(int& role, const Resource&, bool& handled) const;

    /** Return the model data for an event.
     *  @param handled  updated to true if the reply is valid, else set to false.
     */
    QVariant eventData(int role, int column, const KAEvent& event, const Resource&, bool& handled) const;

    /** Called when a resource notifies a message to display to the user. */
    void handleResourceMessage(ResourceType::MessageType, const QString& message, const QString& details);

    /** Return whether calendar migration is currently in progress. */
    bool isMigrating() const;

    /** To be called when calendar migration has been initiated (or reset). */
    void setMigrationInitiated(bool started = true);

    /** To be called when calendar migration has been initiated (or reset). */
    void setMigrationComplete();

    static QString  repeatText(const KAEvent&);
    static QString  repeatOrder(const KAEvent&);
    static QString  whatsThisText(int column);
    static QPixmap* eventIcon(const KAEvent&);

private:
    static QPixmap* mTextIcon;
    static QPixmap* mFileIcon;
    static QPixmap* mCommandIcon;
    static QPixmap* mEmailIcon;
    static QPixmap* mAudioIcon;
    static QSize    mIconSize;      // maximum size of any icon

    int  mMigrationStatus {-1};     // migration status, -1 = no, 0 = initiated, 1 = complete
};

#endif // RESOURCEDATAMODELBASE_H

// vim: et sw=4:
