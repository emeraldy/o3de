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

#pragma once

#include <AzCore/Interface/Interface.h>
#include <AzCore/IO/Path/Path.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/Serialization/SerializeContext.h>

namespace AzToolsFramework
{
    using EntityIdList = AZStd::vector<AZ::EntityId>;

    namespace UndoSystem
    {
        class URSequencePoint;
    }

    namespace Prefab
    {
        typedef AZ::Outcome<void, AZStd::string> PrefabOperationResult;
        typedef AZ::Outcome<bool, AZStd::string> PrefabRequestResult;
        typedef AZ::Outcome<AZ::EntityId, AZStd::string> PrefabEntityResult;

        /*!
         * PrefabPublicInterface
         * Interface to expose Prefab functionality directly to UI and Scripting.
         * Functions will correctly call the Undo/Redo system under the hood.
         */
        class PrefabPublicInterface
        {
        public:
            AZ_RTTI(PrefabPublicInterface, "{931AAE9D-C775-4818-9070-A2DA69489CBE}");

            /**
             * Create a prefab out of the entities provided, at the path provided.
             * Automatically detects descendants of entities, and discerns between entities and child instances.
             * @param entityIds The entities that should form the new prefab (along with their descendants).
             * @param filePath The path for the new prefab file.
             * @return An outcome object; on failure, it comes with an error message detailing the cause of the error.
             */
            virtual PrefabOperationResult CreatePrefab(const AZStd::vector<AZ::EntityId>& entityIds, AZStd::string_view filePath) = 0;

            /**
             * Instantiate a prefab from a prefab file.
             * @param filePath The path to the prefab file to instantiate.
             * @param parent The entity the prefab should be a child of in the transform hierarchy.
             * @param position The position in world space the prefab should be instantiated in.
             * @return An outcome object; on failure, it comes with an error message detailing the cause of the error.
             */
            virtual PrefabOperationResult InstantiatePrefab(AZStd::string_view filePath, AZ::EntityId parent, AZ::Vector3 position) = 0;

            /**
             * Saves changes to prefab to disk.
             * @param filePath The path to the prefab to save.
             * @return Returns Success if the file was saved, or an error message otherwise.
             */
            virtual PrefabOperationResult SavePrefab(AZ::IO::Path filePath) = 0;

            /**
             * Creates a new entity under the entity with id 'parentId' and propagates a change to the template
             * of the owning instance of parentId.
             *
             * @param parentId The id of the parent entity to parent the newly added entity under.
             * @param position The transform position of the entity being added.
             * @return Returns the entityId of the newly created entity, or an error message if the operation failed.
             */
            virtual PrefabEntityResult CreateEntity(AZ::EntityId parentId, const AZ::Vector3& position) = 0;

            /**
             * Store the changes between the current entity state and its last cached state into undo/redo commands.
             * These changes are stored as patches to the owning prefab instance template, as appropriate.
             * The function also triggers the redo() of the nodes it creates, triggering propagation on the next tick.
             * 
             * @param entityId The entity to patch.
             * @param parentUndoBatch The undo batch the undo nodes should be parented to.
             */
            virtual void GenerateUndoNodesForEntityChangeAndUpdateCache(
                AZ::EntityId entityId, UndoSystem::URSequencePoint* parentUndoBatch) = 0;
            
            /**
             * Detects if an entity is the container entity for its owning prefab instance.
             * @param entityId The entity to query.
             * @return True if the entity is the container entity for its owning prefab instance, false otherwise.
             */
            virtual bool IsInstanceContainerEntity(AZ::EntityId entityId) const = 0;
            
            /**
             * Detects if an entity is the container entity for the level prefab instance.
             * @param entityId The entity to query.
             * @return True if the entity is the container entity for the level prefab instance, false otherwise.
             */
            virtual bool IsLevelInstanceContainerEntity(AZ::EntityId entityId) const = 0;
            
            /**
             * Gets the entity id for the instance container of the owning instance.
             * @param entityId The id of the entity to query.
             * @return The entity id of the instance container owning the queried entity.
             */
            virtual AZ::EntityId GetInstanceContainerEntityId(AZ::EntityId entityId) const = 0;
            
            /**
             * Gets the entity id for the instance container of the level instance.
             * @return The entity id of the instance container for the currently loaded level.
             */
            virtual AZ::EntityId GetLevelInstanceContainerEntityId() const = 0;

            /**
             * Get the file path to the prefab file for the prefab instance owning the entity provided.
             * @param entityId The id for the entity being queried.
             * @return Returns the path to the prefab, or an empty path if the entity is owned by the level.
             */
            virtual AZ::IO::Path GetOwningInstancePrefabPath(AZ::EntityId entityId) const = 0;

            /**
             * Gets whether the prefab has unsaved changes.
             * @param filePath The path to the prefab to query.
             * @return Returns true if the prefab has unsaved changes, false otherwise. If path is invalid, returns an error message.
             */
            virtual PrefabRequestResult HasUnsavedChanges(AZ::IO::Path prefabFilePath) const = 0;

            /**
             * Deletes all entities from the owning instance. Bails if the entities don't all belong to the same instance.
             * @param entities The entities to delete.
             * @return An outcome object; on failure, it comes with an error message detailing the cause of the error.
             */
            virtual PrefabOperationResult DeleteEntitiesInInstance(const EntityIdList& entityIds) = 0;
            
            /**
             * Deletes all entities and their descendants from the owning instance. Bails if the entities don't all belong to the same
             * instance.
             * @param entities The entities to delete. Their descendants will be discovered by this function.
             * @return An outcome object; on failure, it comes with an error message detailing the cause of the error.
             */
            virtual PrefabOperationResult DeleteEntitiesAndAllDescendantsInInstance(const EntityIdList& entityIds) = 0;
        };

    } // namespace Prefab
} // namespace AzToolsFramework
