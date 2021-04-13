#
# All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
# its licensors.
#
# For complete copyright and license terms please see the LICENSE at the root of this
# distribution (the "License"). All use of this software is governed by the License,
# or, if provided, by the license below or the license accompanying this file. Do not
# remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#

set(FILES
    Include/Public/AWSCoreBus.h
    Include/Public/Credential/AWSCredentialBus.h
    Include/Public/Framework/AWSApiClientJob.h
    Include/Public/Framework/AWSApiClientJobConfig.h
    Include/Public/Framework/AWSApiJob.h
    Include/Public/Framework/AWSApiJobConfig.h
    Include/Public/Framework/AWSApiRequestJob.h
    Include/Public/Framework/AWSApiRequestJobConfig.h
    Include/Public/Framework/Error.h
    Include/Public/Framework/HttpClientComponent.h
    Include/Public/Framework/HttpRequestJob.h
    Include/Public/Framework/HttpRequestJobConfig.h
    Include/Public/Framework/JobExecuter.h
    Include/Public/Framework/JsonObjectHandler.h
    Include/Public/Framework/JsonWriter.h
    Include/Public/Framework/MultipartFormData.h
    Include/Public/Framework/RequestBuilder.h
    Include/Public/Framework/ServiceClientJob.h
    Include/Public/Framework/ServiceClientJobConfig.h
    Include/Public/Framework/ServiceJob.h
    Include/Public/Framework/ServiceJobConfig.h
    Include/Public/Framework/ServiceJobUtil.h
    Include/Public/Framework/ServiceRequestJob.h
    Include/Public/Framework/ServiceRequestJobConfig.h
    Include/Public/Framework/Util.h
    Include/Public/ResourceMapping/AWSResourceMappingBus.h
    Include/Public/ScriptCanvas/AWSScriptBehaviorBase.h
    Include/Public/ScriptCanvas/AWSScriptBehaviorDynamoDB.h
    Include/Public/ScriptCanvas/AWSScriptBehaviorLambda.h
    Include/Public/ScriptCanvas/AWSScriptBehaviorS3.h
    Include/Public/ScriptCanvas/AWSScriptBehaviorsComponent.h
    Include/Private/AWSCoreInternalBus.h
    Include/Private/AWSCoreSystemComponent.h
    Include/Private/Configuration/AWSCoreConfiguration.h
    Include/Private/Credential/AWSCredentialManager.h
    Include/Private/Credential/AWSCVarCredentialHandler.h
    Include/Private/Credential/AWSDefaultCredentialHandler.h
    Include/Private/ResourceMapping/AWSResourceMappingConstants.h
    Include/Private/ResourceMapping/AWSResourceMappingManager.h
    Include/Private/ResourceMapping/AWSResourceMappingUtils.h
    Source/AWSCoreSystemComponent.cpp
    Source/Configuration/AWSCoreConfiguration.cpp
    Source/Credential/AWSCredentialManager.cpp
    Source/Credential/AWSCVarCredentialHandler.cpp
    Source/Credential/AWSDefaultCredentialHandler.cpp
    Source/Framework/AWSApiJob.cpp
    Source/Framework/AWSApiJobConfig.cpp
    Source/Framework/Error.cpp
    Source/Framework/HttpRequestJob.cpp
    Source/Framework/HttpRequestJobConfig.cpp
    Source/Framework/JsonObjectHandler.cpp
    Source/Framework/MultipartFormData.cpp
    Source/Framework/RequestBuilder.cpp
    Source/Framework/ServiceJob.cpp
    Source/Framework/ServiceJobConfig.cpp
    Source/ResourceMapping/AWSResourceMappingManager.cpp
    Source/ResourceMapping/AWSResourceMappingUtils.cpp
    Source/ScriptCanvas/AWSScriptBehaviorDynamoDB.cpp
    Source/ScriptCanvas/AWSScriptBehaviorLambda.cpp
    Source/ScriptCanvas/AWSScriptBehaviorS3.cpp
    Source/ScriptCanvas/AWSScriptBehaviorsComponent.cpp
)