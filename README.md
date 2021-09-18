# dk-crawl

A fork of [crawl](https://github.com/crawl/crawl.git) to explore the feasibility of adding Azure Active Directory B2C support for user authorization.

## Setup Azure Active Directory B2C environment

* Deploy new AAD B2C tenant

* Configure custom user attribute (str)"Crawlhandle"

* Create new application and secret

* Create Signup/signin user flow

* For unique usernames, add API connector to Azure Function running [this](https://github.com/dkirby-ms/dkcrawl-uniqueusernames-b2c) Azure Function, edited as needed

## Setup VSCode/python dev environment

* Install crawl build deps

    ```sh
    sudo apt install build-essential libncursesw5-dev bison flex liblua5.1-0-dev \
    libsqlite3-dev libz-dev pkg-config python3-yaml binutils-gold python-is-python3 \
    libsdl2-image-dev libsdl2-mixer-dev libsdl2-dev \
    libfreetype6-dev libpng-dev fonts-dejavu-core advancecomp pngcrush python3-pip
    ```

* Compile crawl with webtiles and dgamelaunch

    ```sh
    cd ./crawl-ref/source
    make -j4 WEBTILES=y DGAMELAUNCH=y
    ```

* Create a folder for crawldata.

* Edit config.py to point to your crawldata folder.

* Set environment variables for AAD_B2C client id and secret.

    ```shell
    # Env vars
    export URLBASE="http://localhost:8080"
    export B2C_TENANT="<your AADB2C tenant name>"
    export SIGNUPSIGNUP_USER_FLOW="B2C_1_signupsignin (or your signup/signin flow name)"
    export EDITPROFILE_USER_FLOW="B2C_1_profileediting1 (or your profile editing flow name)"

    # Secret env vars
    export B2C_CLIENT_ID="<replace with client id of B2C application>"
    export B2C_CLIENT_SECRET="<replace with client secret of B2C application"
    export KV_CLIENT_ID="<replace with client id of SPN used for Keyvault authentication>"
    export KV_CLIENT_SECRET="<replace with client secret of SPN used for Keyvault authentication>"
    export KV_TENANT_ID="<replace with tenant id of SPN used for Keyvault authentication>"
    export KV_NAME="<replace with name of keyvault>"
    ```

* Set up a Python virtualenv. From ./crawl-ref/source/

    ```shell
    sudo apt install python3-pip
    pip install virtualenv
    python3 -m virtualenv -p python3 webserver/venv
    . ./webserver/venv/bin/activate
    pip install -r webserver/requirements/dev.py3.txt
    pip install azure-identity azure-keyvault-secrets msal
    ```

## Problems to solve

* Open auth in popup instead of parent.relocate()

* finish session management rework
