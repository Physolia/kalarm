/*
 *  desktop.h  -  desktop functions
 *  Program:  kalarm
 *  Copyright © 2008-2019 by David Jarvie <djarvie@kde.org>
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

#ifndef DESKTOP_H
#define DESKTOP_H

#include <QRect>

namespace Desktop
{

/** Desktop identity, obtained from XDG_CURRENT_DESKTOP. */
enum Type
{
    Kde,      //!< KDE (KDE 4 and Plasma both identify as "KDE")
    Unity,    //!< Unity
    Other
};

Type currentIdentity();
QString currentIdentityName();

QRect workArea(int screen = -1);

} // namespace Desktop

#endif // DESKTOP_H

// vim: et sw=4:
