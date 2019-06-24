/*
 *  alarmlistview.cpp  -  widget showing list of alarms
 *  Program:  kalarm
 *  Copyright © 2007,2008,2010,2019 David Jarvie <djarvie@kde.org>
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

#include "kalarm.h"
#include "alarmlistview.h"

#include <ksharedconfig.h>
#include <kconfiggroup.h>

#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QApplication>


AlarmListView::AlarmListView(const QByteArray& configGroup, QWidget* parent)
    : EventListView(parent),
      mConfigGroup(configGroup)
{
    setEditOnSingleClick(true);
    connect(header(), &QHeaderView::sectionMoved, this, &AlarmListView::sectionMoved);
}

void AlarmListView::setModel(QAbstractItemModel* model)
{
    EventListView::setModel(model);
    KConfigGroup config(KSharedConfig::openConfig(), mConfigGroup.constData());
    QByteArray settings = config.readEntry("ListHead", QByteArray());
    if (!settings.isEmpty())
        header()->restoreState(settings);
    header()->setSectionsMovable(true);
    header()->setStretchLastSection(false);
    header()->setSectionResizeMode(AlarmListModel::TimeColumn, QHeaderView::ResizeToContents);
    header()->setSectionResizeMode(AlarmListModel::TimeToColumn, QHeaderView::ResizeToContents);
    header()->setSectionResizeMode(AlarmListModel::RepeatColumn, QHeaderView::ResizeToContents);
    header()->setSectionResizeMode(AlarmListModel::ColourColumn, QHeaderView::Fixed);
    header()->setSectionResizeMode(AlarmListModel::TypeColumn, QHeaderView::Fixed);
    header()->setSectionResizeMode(AlarmListModel::TextColumn, QHeaderView::Stretch);
    header()->setStretchLastSection(true);   // necessary to ensure ResizeToContents columns do resize to contents!
    const int margin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin);
    header()->resizeSection(AlarmListModel::ColourColumn, viewOptions().fontMetrics.lineSpacing() * 3 / 4);
    header()->resizeSection(AlarmListModel::TypeColumn, AlarmListModel::iconWidth() + 2*margin + 2);
    header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(header(), &QWidget::customContextMenuRequested, this, &AlarmListView::headerContextMenuRequested);
}

QList<bool> AlarmListView::columnsVisible() const
{
    if (!model())
        return {};
    return { !header()->isSectionHidden(AlarmListModel::TimeColumn),
             !header()->isSectionHidden(AlarmListModel::TimeToColumn),
             !header()->isSectionHidden(AlarmListModel::RepeatColumn),
             !header()->isSectionHidden(AlarmListModel::ColourColumn),
             !header()->isSectionHidden(AlarmListModel::TypeColumn) };
}

void AlarmListView::setColumnsVisible(const QList<bool>& show)
{
    if (!model()  ||  show.size() < 5)
        return;
    header()->setSectionHidden(AlarmListModel::TimeColumn,   !show[0]);
    header()->setSectionHidden(AlarmListModel::TimeToColumn, !show[1]);
    header()->setSectionHidden(AlarmListModel::RepeatColumn, !show[2]);
    header()->setSectionHidden(AlarmListModel::ColourColumn, !show[3]);
    header()->setSectionHidden(AlarmListModel::TypeColumn,   !show[4]);
    sortByColumn(show[0] ? AlarmListModel::TimeColumn : AlarmListModel::TimeToColumn, Qt::AscendingOrder);
}

/******************************************************************************
* Called when the column order is changed.
* Save the new order for restoration on program restart.
*/
void AlarmListView::sectionMoved()
{
    KConfigGroup config(KSharedConfig::openConfig(), mConfigGroup.constData());
    config.writeEntry("ListHead", header()->saveState());
    config.sync();
}

/******************************************************************************
* Called when a context menu is requested for the header.
* Allow the user to choose which columns to display.
*/
void AlarmListView::headerContextMenuRequested(const QPoint& pt)
{
    QAbstractItemModel* almodel = model();
    int count = header()->count();
    QMenu menu;
    for (int col = 0;  col < count;  ++col)
    {
        const QString title = almodel->headerData(col, Qt::Horizontal, AkonadiModel::ColumnTitleRole).toString();
        if (!title.isEmpty())
        {
            QAction* act = menu.addAction(title);
            act->setData(col);
            act->setCheckable(true);
            act->setChecked(!header()->isSectionHidden(col));
            if (col == AlarmListModel::TextColumn)
                act->setEnabled(false);    // don't allow text column to be hidden
            else
                QObject::connect(act, &QAction::triggered,
                                 this, [this, &menu, act] { showHideColumn(menu, act); });
        }
    }
    enableTimeColumns(&menu);
    menu.exec(header()->mapToGlobal(pt));
}

/******************************************************************************
* Show or hide a column according to the header context menu.
*/
void AlarmListView::showHideColumn(QMenu& menu, QAction* act)
{
    int col = act->data().toInt();
    if (col < 0  ||  col >= header()->count())
        return;
    bool show = act->isChecked();
    header()->setSectionHidden(col, !show);
    if (col == AlarmListModel::TimeColumn  ||  col == AlarmListModel::TimeToColumn)
        enableTimeColumns(&menu);
    Q_EMIT columnsVisibleChanged();
}

/******************************************************************************
* Disable Time or Time To in the context menu if the other one is not
* selected to be displayed, to ensure that at least one is always shown.
*/
void AlarmListView::enableTimeColumns(QMenu* menu)
{
    bool timeShown   = !header()->isSectionHidden(AlarmListModel::TimeColumn);
    bool timeToShown = !header()->isSectionHidden(AlarmListModel::TimeToColumn);
    QList<QAction*> actions = menu->actions();
    if (!timeToShown)
    {
        header()->setSectionHidden(AlarmListModel::TimeColumn, false);
        for (QAction* act : qAsConst(actions))
        {
            if (act->data().toInt() == AlarmListModel::TimeColumn)
            {
                act->setEnabled(false);
                break;
            }
        }
    }
    else if (!timeShown)
    {
        header()->setSectionHidden(AlarmListModel::TimeToColumn, false);
        for (QAction* act : qAsConst(actions))
        {
            if (act->data().toInt() == AlarmListModel::TimeToColumn)
            {
                act->setEnabled(false);
                break;
            }
        }
    }
}

// vim: et sw=4:
