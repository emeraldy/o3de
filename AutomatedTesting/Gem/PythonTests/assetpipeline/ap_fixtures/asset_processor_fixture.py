"""
All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
its licensors.

For complete copyright and license terms please see the LICENSE at the root of this
distribution (the "License"). All use of this software is governed by the License,
or, if provided, by the license below or the license accompanying this file. Do not
remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

A fixture for using the Asset Processor in lmbr_test, this will stop the asset processor after every test via
the teardown. Using the fixture at class level will stop the asset processor after the suite completes.
Using the fixture at test level will stop asset processor after the test completes. Calling this fixture as a test argument will still run the teardown to stop the Asset Processor.
"""

# Import Builtins
import pytest
import logging

# Import LyTestTools
import ly_test_tools.lumberyard.asset_processor as asset_processor_commands

logger = logging.getLogger(__name__)


@pytest.fixture
def asset_processor(request: pytest.fixture, workspace: pytest.fixture) -> asset_processor_commands.AssetProcessor:
    """
    Sets up usage of the asset proc
    :param request:
    :return: ly_test_tools.lumberyard.asset_processor.AssetProcessor
    """

    # Initialize the Asset Processor
    ap = asset_processor_commands.AssetProcessor(workspace)

    # Custom teardown method for this fixture
    def teardown():
        logger.info("Running asset_processor_fixture teardown to stop the AP")
        ap.stop()

    request.addfinalizer(teardown)

    return ap