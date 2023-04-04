/*
 * Author: smithm
 *
 * Created on August 22, 2022, 10:36 AM
 *
 * Copyright (c) European XFEL GmbH Schenefeld. All rights reserved.
 */

#ifndef TESTRUNNER_HH
#define TESTRUNNER_HH

#include <thread>
#include <utility>
#include <gtest/gtest.h>

#include "karabo/core/DeviceClient.hh"
#include "karabo/core/DeviceServer.hh"
#include "karabo/net/EventLoop.hh"
#include "karabo/util/Hash.hh"

#define DEVICE_SERVER_ID "testDeviceSrvCpp"
#define LOG_PRIORITY     "FATAL"  // Can also be "DEBUG", "INFO" or "ERROR"

#define DEV_CLI_TIMEOUT_SEC 2


/**
 * @brief Test fixture for setup/teardown of karabo event loop,
 *        device server, and device client.
 * 
 *        This class is generic and does not need to be modified
 *        to support different device classes.
 */
class KaraboDeviceFixture: public testing::Test {
protected:

    KaraboDeviceFixture();
    ~KaraboDeviceFixture();

    // instanitate an instance of the classID (with the devCfg configuration hash)
    // and return the BaseDevice::Pointer for that instance
    void instantiateAndGetPointer(const std::string& classId, const std::string& instanceId, 
                                  const karabo::util::Hash& devCfg,
                                  karabo::core::BaseDevice::Pointer& base_device);


    std::thread m_eventLoopThread;
    karabo::core::DeviceServer::Pointer m_deviceSrv;
    karabo::core::DeviceClient::Pointer m_deviceCli;
};

#endif
