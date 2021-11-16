/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *      Nick Dedekind <nick.dedekind@canonical.com>
 */

#include "unitymenumodelstack.h"

UnityMenuModelStack::UnityMenuModelStack(QObject *parent):
    QObject(parent)
{
}

UnityMenuModelStack::~UnityMenuModelStack()
{
}

AyatanaMenuModel *UnityMenuModelStack::head() const
{
    return !m_menuModels.isEmpty() ? m_menuModels.first() : nullptr;
}

void UnityMenuModelStack::setHead(AyatanaMenuModel *model)
{
    if (head() != model) {
        m_menuModels.clear();
        push(model);
        Q_EMIT headChanged(model);
    }
}

AyatanaMenuModel *UnityMenuModelStack::tail() const
{
    return !m_menuModels.isEmpty() ? m_menuModels.last() : nullptr;
}

void UnityMenuModelStack::push(AyatanaMenuModel *model)
{
    m_menuModels << model;
    Q_EMIT tailChanged(model);
}

AyatanaMenuModel *UnityMenuModelStack::pop()
{
    if (m_menuModels.isEmpty()) {
        return nullptr;
    }
    AyatanaMenuModel *model = m_menuModels.takeLast();

    Q_EMIT tailChanged(tail());
    if (m_menuModels.isEmpty()) {
        Q_EMIT headChanged(nullptr);
    }

    return model;
}
