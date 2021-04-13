/*
 * All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
 * its licensors.
 *
 * For complete copyright and license terms please see the LICENSE at the root of this
 * distribution (the "License"). All use of this software is governed by the License,
 * or, if provided, by the license below or the license accompanying this file. Do not
 * remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 */

#include <AzToolsFramework/Application/EditorEntityManager.h>

#include <AzToolsFramework/API/ToolsApplicationAPI.h>

namespace AzToolsFramework
{
    void EditorEntityManager::Start()
    {
        m_prefabPublicInterface = AZ::Interface<Prefab::PrefabPublicInterface>::Get();
        AZ_Assert(m_prefabPublicInterface, "EditorEntityManager - Could not retrieve instance of PrefabPublicInterface");

        AZ::Interface<EditorEntityAPI>::Register(this);
    }

    EditorEntityManager::~EditorEntityManager()
    {
        // Attempting to Unregister if we never registerd (e.g. if Start() was never called) throws an error, so check that first
        EditorEntityAPI* editorEntityInterface = AZ::Interface<EditorEntityAPI>::Get();
        if (editorEntityInterface == this)
        {
            AZ::Interface<EditorEntityAPI>::Unregister(this);
        }
    }

    void EditorEntityManager::DeleteSelected()
    {
        EntityIdList selectedEntities;
        ToolsApplicationRequestBus::BroadcastResult(selectedEntities, &ToolsApplicationRequests::GetSelectedEntities);
        m_prefabPublicInterface->DeleteEntitiesInInstance(selectedEntities);
    }

    void EditorEntityManager::DeleteEntityById(AZ::EntityId entityId)
    {
        DeleteEntities({entityId});
    }

    void EditorEntityManager::DeleteEntities(const EntityIdList& entities)
    {
        m_prefabPublicInterface->DeleteEntitiesInInstance(entities);
    }

    void EditorEntityManager::DeleteEntityAndAllDescendants(AZ::EntityId entityId)
    {
        DeleteEntitiesAndAllDescendants({entityId});
    }

    void EditorEntityManager::DeleteEntitiesAndAllDescendants(const EntityIdList& entities)
    {
        m_prefabPublicInterface->DeleteEntitiesAndAllDescendantsInInstance(entities);
    }
}
