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

#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <AzCore/Casting/numeric_cast.h>
#include <AzCore/std/smart_ptr/make_shared.h>
#include <SceneAPI/FbxSceneBuilder/FbxSceneSystem.h>
#include <SceneAPI/FbxSceneBuilder/ImportContexts/AssImpImportContexts.h>
#include <SceneAPI/FbxSceneBuilder/Importers/Utilities/AssImpMeshImporterUtilities.h>
#include <SceneAPI/SceneCore/Utilities/Reporting.h>
#include <SceneAPI/SceneData/GraphData/BlendShapeData.h>
#include <SceneAPI/SceneData/GraphData/BoneData.h>
#include <SceneAPI/SceneData/GraphData/MeshData.h>

namespace AZ::SceneAPI::FbxSceneBuilder
{
    bool BuildSceneMeshFromAssImpMesh(aiNode* currentNode, const aiScene* scene, const FbxSceneSystem& sceneSystem, AZStd::vector<AZStd::shared_ptr<DataTypes::IGraphObject>>& meshes,
        const AZStd::function<AZStd::shared_ptr<SceneData::GraphData::MeshData>()>& makeMeshFunc)
    {
        AZStd::unordered_map<int, int> assImpMatIndexToLYIndex;
        int lyMeshIndex = 0;

        if(!currentNode || !scene)
        {
            return false;
        }

        for (int m = 0; m < currentNode->mNumMeshes; ++m)
        {
            auto newMesh = makeMeshFunc();

            newMesh->SetUnitSizeInMeters(sceneSystem.GetUnitSizeInMeters());
            newMesh->SetOriginalUnitSizeInMeters(sceneSystem.GetOriginalUnitSizeInMeters());

            newMesh->SetSdkMeshIndex(m);

            aiMesh* mesh = scene->mMeshes[currentNode->mMeshes[m]];

            // Lumberyard materials are created in order based on mesh references in the scene
            if (assImpMatIndexToLYIndex.find(mesh->mMaterialIndex) == assImpMatIndexToLYIndex.end())
            {
                assImpMatIndexToLYIndex.insert(AZStd::pair<int, int>(mesh->mMaterialIndex, lyMeshIndex++));
            }

            for (int vertIdx = 0; vertIdx < mesh->mNumVertices; ++vertIdx)
            {
                AZ::Vector3 vertex(mesh->mVertices[vertIdx].x, mesh->mVertices[vertIdx].y, mesh->mVertices[vertIdx].z);

                sceneSystem.SwapVec3ForUpAxis(vertex);
                sceneSystem.ConvertUnit(vertex);
                newMesh->AddPosition(vertex);
                newMesh->SetVertexIndexToControlPointIndexMap(vertIdx, vertIdx);

                if (mesh->HasNormals())
                {
                    AZ::Vector3 normal(mesh->mNormals[vertIdx].x, mesh->mNormals[vertIdx].y, mesh->mNormals[vertIdx].z);
                    sceneSystem.SwapVec3ForUpAxis(normal);
                    normal.NormalizeSafe();
                    newMesh->AddNormal(normal);
                }
            }

            for (int faceIdx = 0; faceIdx < mesh->mNumFaces; ++faceIdx)
            {
                aiFace face = mesh->mFaces[faceIdx];
                AZ::SceneAPI::DataTypes::IMeshData::Face meshFace;
                if (face.mNumIndices != 3)
                {
                    // AssImp should have triangulated everything, so if this happens then someone has
                    // probably changed AssImp's import settings. The engine only supports triangles.
                    AZ_Error(Utilities::ErrorWindow, false,
                        "Mesh on node %s has a face with %d vertices, only 3 vertices are supported per face.",
                        currentNode->mName.C_Str(),
                        face.mNumIndices);
                    continue;
                }
                for (int idx = 0; idx < face.mNumIndices; ++idx)
                {
                    meshFace.vertexIndex[idx] = face.mIndices[idx];
                }

                newMesh->AddFace(meshFace, assImpMatIndexToLYIndex[mesh->mMaterialIndex]);
            }

            meshes.push_back(newMesh);
        }

        return true;
    }

    GetMeshDataFromParentResult GetMeshDataFromParent(AssImpSceneNodeAppendedContext& context)
    {
        const DataTypes::IGraphObject* const parentData =
            context.m_scene.GetGraph().GetNodeContent(context.m_currentGraphPosition).get();

        if (!parentData)
        {
            AZ_Error(Utilities::ErrorWindow, false,
                "GetMeshDataFromParent failed because the parent was null, it should only be called with a valid parent node");
            return AZ::Failure(Events::ProcessingResult::Failure);
        }

        if (!parentData->RTTI_IsTypeOf(SceneData::GraphData::MeshData::TYPEINFO_Uuid()))
        {
            // The parent node may contain bone information and not mesh information, skip it.
            if (parentData->RTTI_IsTypeOf(SceneData::GraphData::BoneData::TYPEINFO_Uuid()))
            {
                // Return the ignore processing result in the failure.
                return AZ::Failure(Events::ProcessingResult::Ignored);
            }
            AZ_Error(Utilities::ErrorWindow, false,
                "Tried to get mesh data from parent for non-mesh parent data");
            return AZ::Failure(Events::ProcessingResult::Failure);
        }

        const SceneData::GraphData::MeshData* const parentMeshData =
            azrtti_cast<const SceneData::GraphData::MeshData* const>(parentData);
        return AZ::Success(parentMeshData);
    }
}