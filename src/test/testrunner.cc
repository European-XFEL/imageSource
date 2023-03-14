/*
 * Author: smithm
 *
 * Created on August 22, 2022, 10:36 AM
 *
 * Copyright (c) European XFEL GmbH Hamburg. All rights reserved.
 */

#include "testrunner.hh"

using namespace karabo::core;


KaraboDeviceFixture::KaraboDeviceFixture() {

    m_eventLoopThread = std::thread(&karabo::net::EventLoop::work);

    // Instantiate C++ Device Server.
    karabo::util::Hash config("serverId", DEVICE_SERVER_ID,
                                "scanPlugins", false,
                                "Logger.priority", LOG_PRIORITY);
    m_deviceSrv = DeviceServer::create("DeviceServer", config);
    m_deviceSrv->finalizeInternalInitialization();
    // Instantiate C++ Device Client.
    m_deviceCli = boost::make_shared<DeviceClient>();
}

void KaraboDeviceFixture::instantiateAndGetPointer(const std::string& classId, const std::string& instanceId, 
        const karabo::util::Hash& devCfg, karabo::core::BaseDevice::Pointer& base_device) {

    std::string errorMsg, errorDetails;

    try {
        // instantiate the device to be tested
        base_device = BaseDevice::create(classId, devCfg);
        // TODO: explain this magic incantation
        base_device->finalizeInternalInitialization(
            m_deviceSrv->getConnection()->clone(instanceId),
            false,
            "");
    } catch (const karabo::util::Exception& e) {
        errorMsg = e.userFriendlyMsg();
        if (errorMsg.empty()) errorMsg = "Unexpected karabo::util::Exception";
        errorDetails = e.detailedMsg();
    } catch (const std::exception& e) {
        errorMsg = e.what();
        if (errorMsg.empty()) errorMsg = "Unexpected std::exception";
    }

    ASSERT_TRUE(errorMsg.empty())
        << "Failure instantiating '" << instanceId << "':" << std::endl
        << errorMsg << std::endl
        << "Failure details :" << std::endl
        << errorDetails << std::endl;
}

KaraboDeviceFixture::~KaraboDeviceFixture( ) {
    m_deviceCli.reset();
    m_deviceSrv.reset();
    karabo::net::EventLoop::stop();
    m_eventLoopThread.join();
}


/*
 * @brief GoogleTest entry point
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}