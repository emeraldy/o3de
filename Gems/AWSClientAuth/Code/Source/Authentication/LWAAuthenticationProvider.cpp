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

#include <AzCore/std/smart_ptr/make_shared.h>
#include <Authentication/LWAAuthenticationProvider.h>
#include <Authentication/AuthenticationProviderBus.h>
#include <Authentication/OAuthConstants.h>
#include <HttpRequestor/HttpRequestorBus.h>
#include <HttpRequestor/HttpTypes.h>

#include <aws/core/http/HttpResponse.h>

namespace AWSClientAuth
{
    constexpr char LwaSettingsPath[] = "/AWS/LoginWithAmazon";    
    constexpr char LwaVerificationUrlResponseKey[] = "verification_uri";

    LWAAuthenticationProvider::LWAAuthenticationProvider()
    {
        m_settings = AZStd::make_unique<LWAProviderSetting>();
    }

    LWAAuthenticationProvider::~LWAAuthenticationProvider()
    {
        m_settings.reset();
    }

    bool LWAAuthenticationProvider::Initialize(AZStd::weak_ptr<AZ::SettingsRegistryInterface> settingsRegistry)
    {
        if (!settingsRegistry.lock()->GetObject(m_settings.get(), azrtti_typeid(m_settings.get()), LwaSettingsPath))
        {
            AZ_Warning("AWSCognitoAuthenticationProvider", true, "Failed to get login with Amazon settings object for path %s", LwaSettingsPath);
            return false;
        }
        return true;
    }

    void LWAAuthenticationProvider::PasswordGrantSingleFactorSignInAsync(const AZStd::string& username, const AZStd::string& password)
    {
        AZ_UNUSED(username);
        AZ_UNUSED(password);
        AZ_Assert(true, "Not supported");
    }

    void LWAAuthenticationProvider::PasswordGrantMultiFactorSignInAsync(const AZStd::string& username, const AZStd::string& password)
    {
        AZ_UNUSED(username);
        AZ_UNUSED(password);
        AZ_Assert(true, "Not supported");
    }

    void LWAAuthenticationProvider::PasswordGrantMultiFactorConfirmSignInAsync(const AZStd::string& username, const AZStd::string& confirmationCode)
    {
        AZ_UNUSED(username);
        AZ_UNUSED(confirmationCode);
        AZ_Assert(true, "Not supported");
    }

    // Call LWA authentication provider device code end point.
    // Refer https://developer.amazon.com/docs/login-with-amazon/retrieve-code-other-platforms-cbl-docs.html.
    void LWAAuthenticationProvider::DeviceCodeGrantSignInAsync()
    {
        // Set headers and body for device sign in http requests.
        AZStd::string body = AZStd::string::format("%s=%s&%s=%s&%s=%s", OAuthResponseTypeBodyKey, m_settings->m_responseType.c_str()
            , OAuthClientIdBodyKey, m_settings->m_appClientId.c_str(), OAuthScopeBodyKey, OAuthScopeBodyValue);

        AZStd::map<AZStd::string, AZStd::string> headers;
        headers[OAuthContentTypeHeaderKey] = OAuthContentTypeHeaderValue;
        headers[OAuthContentLengthHeaderKey] = AZStd::to_string(body.length());
        
        HttpRequestor::HttpRequestorRequestBus::Broadcast(&HttpRequestor::HttpRequestorRequests::AddRequestWithHeadersAndBody, m_settings->m_oAuthCodeURL
            , Aws::Http::HttpMethod::HTTP_POST, headers, body
            , [this](const Aws::Utils::Json::JsonView& jsonView, Aws::Http::HttpResponseCode responseCode)
            {
                if (responseCode == Aws::Http::HttpResponseCode::OK)
                {
                    m_cachedUserCode = jsonView.GetString(OAuthUserCodeResponseKey).c_str();
                    m_cachedDeviceCode = jsonView.GetString(OAuthDeviceCodeBodyKey).c_str();
                    AuthenticationProviderNotificationBus::Broadcast(&AuthenticationProviderNotifications::OnDeviceCodeGrantSignInSuccess
                        , jsonView.GetString(OAuthUserCodeResponseKey).c_str()
                        , jsonView.GetString(LwaVerificationUrlResponseKey).c_str()
                        , jsonView.GetInteger(OAuthExpiresInResponseKey));
                }
                else
                {
                    AuthenticationProviderNotificationBus::Broadcast(&AuthenticationProviderNotifications::OnDeviceCodeGrantSignInFail
                        , jsonView.GetString(OAuthErrorResponseKey).c_str());
                }
            }
        );
    }

    // Call LWA authentication provider OAuth tokens endpoint
    // Refer https://developer.amazon.com/docs/login-with-amazon/retrieve-token-other-platforms-cbl-docs.html
    void LWAAuthenticationProvider::DeviceCodeGrantConfirmSignInAsync()
    {
        // Set headers and body for device confirm sign in http requests.
        AZStd::string body = AZStd::string::format("%s=%s&%s=%s&%s=%s", OAuthUserCodeResponseKey, m_cachedUserCode.c_str()
            , OAuthGrantTypeBodyKey, m_settings->m_grantType.c_str(), OAuthDeviceCodeBodyKey, m_cachedDeviceCode.c_str());

        AZStd::map<AZStd::string, AZStd::string> headers;
        headers[OAuthContentTypeHeaderKey] = OAuthContentTypeHeaderValue;
        headers[OAuthContentLengthHeaderKey] = AZStd::to_string(body.length());

        HttpRequestor::HttpRequestorRequestBus::Broadcast(&HttpRequestor::HttpRequestorRequests::AddRequestWithHeadersAndBody, m_settings->m_oAuthTokensURL
            , Aws::Http::HttpMethod::HTTP_POST, headers, body
            , [this](const Aws::Utils::Json::JsonView& jsonView, Aws::Http::HttpResponseCode responseCode)
            {
                if (responseCode == Aws::Http::HttpResponseCode::OK)
                {
                    // Id and access token are the same.
                    UpdateTokens(jsonView);
                    AuthenticationProviderNotificationBus::Broadcast(
                        &AuthenticationProviderNotifications::OnDeviceCodeGrantConfirmSignInSuccess, m_authenticationTokens);
                    m_cachedUserCode = "";
                    m_cachedDeviceCode = "";
                }
                else
                {
                    AuthenticationProviderNotificationBus::Broadcast(&AuthenticationProviderNotifications::OnDeviceCodeGrantConfirmSignInFail
                        , jsonView.GetString("error").c_str());
                }
            }
        );
    }

    void LWAAuthenticationProvider::RefreshTokensAsync()
    {
        // Set headers and body for device confirm sign in http requests.
        AZStd::string body = AZStd::string::format("%s=%s&%s=%s&%s=%s", OAuthClientIdBodyKey, m_settings->m_appClientId.c_str(), OAuthGrantTypeBodyKey,
            OAuthRefreshTokenBodyValue, OAuthRefreshTokenBodyKey, m_authenticationTokens.GetRefreshToken().c_str());

        AZStd::map<AZStd::string, AZStd::string> headers;
        headers[OAuthContentTypeHeaderKey] = OAuthContentTypeHeaderValue;
        headers[OAuthContentLengthHeaderKey] = AZStd::to_string(body.length());

        HttpRequestor::HttpRequestorRequestBus::Broadcast(&HttpRequestor::HttpRequestorRequests::AddRequestWithHeadersAndBody, m_settings->m_oAuthTokensURL
            , Aws::Http::HttpMethod::HTTP_POST, headers, body
            , [this](const Aws::Utils::Json::JsonView& jsonView, Aws::Http::HttpResponseCode responseCode)
        {
            if (responseCode == Aws::Http::HttpResponseCode::OK)
            {
                // Id and access token are the same.
                UpdateTokens(jsonView);
                AuthenticationProviderNotificationBus::Broadcast(&AuthenticationProviderNotifications::OnRefreshTokensSuccess, m_authenticationTokens);
            }
            else
            {
                AuthenticationProviderNotificationBus::Broadcast(&AuthenticationProviderNotifications::OnRefreshTokensFail
                    , jsonView.GetString("error").c_str());
            }
        }
        );
    }

    void LWAAuthenticationProvider::UpdateTokens(const Aws::Utils::Json::JsonView& jsonView)
    {
        // For Login with Amazon openId and access tokens are the same.
        m_authenticationTokens = AuthenticationTokens(jsonView.GetString(OAuthAccessTokenResponseKey).c_str(), jsonView.GetString(OAuthRefreshTokenResponseKey).c_str(),
            jsonView.GetString(OAuthAccessTokenResponseKey).c_str(), ProviderNameEnum::LoginWithAmazon
            , jsonView.GetInteger(OAuthExpiresInResponseKey));
    }

} // namespace AWSClientAuth