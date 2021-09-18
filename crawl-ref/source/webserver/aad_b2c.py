import os
import msal
import requests
import tornado

b2c_tenant = os.getenv("B2C_TENANT")
signupsignin_user_flow = os.getenv("SIGNUPSIGNUP_USER_FLOW")
editprofile_user_flow = os.getenv("EDITPROFILE_USER_FLOW")
authority_template = "https://{tenant}.b2clogin.com/{tenant}.onmicrosoft.com/{user_flow}"

if not b2c_tenant:
    raise ValueError("Need to define B2C_TENANT environment variable")
if not signupsignin_user_flow:
    raise ValueError("Need to define SIGNUPSIGNUP_USER_FLOW environment variable")
if not editprofile_user_flow:
    raise ValueError("Need to define EDITPROFILE_USER_FLOW environment variable")

CLIENT_ID = os.getenv("B2C_CLIENT_ID") # Application (client) ID of app registration
CLIENT_SECRET = os.getenv("B2C_CLIENT_SECRET") 
if not CLIENT_ID:
    raise ValueError("Need to define B2C_CLIENT_ID environment variable")
if not CLIENT_SECRET:
    raise ValueError("Need to define B2C_CLIENT_SECRET environment variable")

AUTHORITY = authority_template.format(
    tenant=b2c_tenant, user_flow=signupsignin_user_flow)
B2C_PROFILE_AUTHORITY = authority_template.format(
    tenant=b2c_tenant, user_flow=editprofile_user_flow)

URLBASE = os.getenv("URLBASE")
if not URLBASE:
    raise ValueError("Need to define URLBASE environment variable")
REDIRECT_PATH = "/authorize"

# This is the API resource endpoint
ENDPOINT = '' # Application ID URI of app registration in Azure portal

# These are the scopes you've exposed in the web API app registration in the Azure portal
SCOPE = []  # Example with two exposed scopes: ["demo.read", "demo.write"]

def _load_cache(handler):
    cache = msal.SerializableTokenCache()
    if "token_cache" in handler.session:
        cache.deserialize(handler.session["token_cache"])
    return cache

def _save_cache(handler, cache):
    if cache.has_state_changed:
        handler.session["token_cache"] = cache.serialize()

def _build_msal_app(cache=None, authority=None):
    return msal.ConfidentialClientApplication(
        CLIENT_ID, authority=authority or AUTHORITY,
        client_credential=CLIENT_SECRET, token_cache=cache)

def _build_auth_code_flow(authority=None, scopes=None):
    return _build_msal_app(authority=authority).initiate_auth_code_flow(
        scopes or [],
        redirect_uri=URLBASE + REDIRECT_PATH) # DaleK - prob need to fix this to return absolute URL

def _get_token_from_cache(scope=None):
    cache = _load_cache()  # This web app maintains one cache per session
    cca = _build_msal_app(cache=cache)
    accounts = cca.get_accounts()
    if accounts:  # So all account(s) belong to the current signed-in user
        result = cca.acquire_token_silent(scope, account=accounts[0])
        _save_cache(cache)
        return result