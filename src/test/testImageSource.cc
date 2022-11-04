/*
 * Author: parenti
 *
 * Created on August 24, 2022, 09:41 AM
 *
 * Copyright (c) European XFEL GmbH Hamburg. All rights reserved.
 */

#include "CameraImageSource.hh"
#include "ImageSource.hh"

#include <boost/shared_ptr.hpp>
#include <gtest/gtest.h>
#include <thread>
#include <utility>

#include "karabo/core/DeviceClient.hh"
#include "karabo/core/DeviceServer.hh"
#include "karabo/net/EventLoop.hh"
#include "karabo/util/Hash.hh"
#include "karabo/util/PluginLoader.hh"


#define DEVICE_SERVER_ID "testDeviceSrvCpp"
#define TEST_IMGSRC_ID   "testImageSource"
#define TEST_CAMSRC_ID   "testCameraImageSource"
#define LOG_PRIORITY     "FATAL"  // Can also be "DEBUG", "INFO" or "ERROR"

#define DEV_CLI_TIMEOUT_SEC 2


/**
 * @brief Test fixture for the ImageSource device class.
 */
class ImageSourceFixture: public testing::Test {
protected:

    ImageSourceFixture() = default;

    void SetUp( ) {
        m_eventLoopThread = std::thread(&karabo::net::EventLoop::work);

        // Load the library dynamically
        const karabo::util::Hash& pluginConfig = karabo::util::Hash("pluginDirectory", ".");
        karabo::util::PluginLoader::create("PluginLoader", pluginConfig)->update();

        // Instantiate C++ Device Server.
        karabo::util::Hash config("serverId", DEVICE_SERVER_ID,
                                  "scanPlugins", true,
                                  "Logger.priority", LOG_PRIORITY);
        m_deviceSrv = karabo::core::DeviceServer::create("DeviceServer", config);
        m_deviceSrv->finalizeInternalInitialization();
        // Instantiate Device Client.
        m_deviceCli = boost::make_shared<karabo::core::DeviceClient>();
    }

    void TearDown( ) {
        m_deviceCli.reset();
        m_deviceSrv.reset();
        karabo::net::EventLoop::stop();
        m_eventLoopThread.join();
    }

    void instantiateTestDevice(const karabo::util::Hash& devSpecificCfg) {
        karabo::util::Hash imgSrcCfg("deviceId", TEST_IMGSRC_ID);
        imgSrcCfg.merge(devSpecificCfg);

        std::pair<bool, std::string> success =
            m_deviceCli->instantiate(DEVICE_SERVER_ID, "ImageSource",
                                     imgSrcCfg, DEV_CLI_TIMEOUT_SEC);

        ASSERT_TRUE(success.first)
            << "Error instantiating '" << TEST_IMGSRC_ID << "':\n"
            << success.second;

        karabo::util::Hash camSrcCfg("deviceId", TEST_CAMSRC_ID);
        camSrcCfg.merge(devSpecificCfg);

        success =
            m_deviceCli->instantiate(DEVICE_SERVER_ID, "CameraImageSource",
                                     camSrcCfg, DEV_CLI_TIMEOUT_SEC);

        ASSERT_TRUE(success.first)
            << "Error instantiating '" << TEST_CAMSRC_ID << "':\n"
            << success.second;
    }

    void deinstantiateTestDevice() {
        ASSERT_NO_THROW(
            m_deviceCli->killDevice(TEST_IMGSRC_ID, DEV_CLI_TIMEOUT_SEC))
        << "Failed to deinstantiate device '" << TEST_IMGSRC_ID << "'";

        ASSERT_NO_THROW(
            m_deviceCli->killDevice(TEST_CAMSRC_ID, DEV_CLI_TIMEOUT_SEC))
        << "Failed to deinstantiate device '" << TEST_CAMSRC_ID << "'";
    }

    std::thread m_eventLoopThread;

    karabo::core::DeviceServer::Pointer m_deviceSrv;
    karabo::core::DeviceClient::Pointer m_deviceCli;
};

// TODO: Give the test case a proper name (not "testScaffold")
TEST_F(ImageSourceFixture, testScaffold){

    // TODO: Provide a non-empty config for the device under test.
    instantiateTestDevice(karabo::util::Hash());

    deinstantiateTestDevice();
}

// arguments to TEST are just strings to name your tests
TEST(UnpackTests, Mono12Packed) {
    std::vector<uint8_t> packedData(3);
    std::vector<uint16_t> unpackedData(2);

    packedData = {0xAB, 0xFC, 0xDE};
    ASSERT_NO_THROW(karabo::util::unpackMono12Packed(packedData.data(), 2, 1, unpackedData.data()));
    ASSERT_EQ((uint16_t)0xABC, unpackedData[0]); // pixel0
    ASSERT_EQ((uint16_t)0xDEF, unpackedData[1]); // pixel1

    packedData = {0x00, 0x0F, 0x00};
    ASSERT_NO_THROW(karabo::util::unpackMono12Packed(packedData.data(), 2, 1, unpackedData.data()));
    ASSERT_EQ((uint16_t)0x00F, unpackedData[0]); // pixel0
    ASSERT_EQ((uint16_t)0x000, unpackedData[1]); // pixel1

    packedData = {0x0F, 0x00, 0x00};
    ASSERT_NO_THROW(karabo::util::unpackMono12Packed(packedData.data(), 2, 1, unpackedData.data()));
    ASSERT_EQ((uint16_t)0x0F0, unpackedData[0]); // pixel0
    ASSERT_EQ((uint16_t)0x000, unpackedData[1]); // pixel1

    packedData = {0xF0, 0x00, 0x00};
    ASSERT_NO_THROW(karabo::util::unpackMono12Packed(packedData.data(), 2, 1, unpackedData.data()));
    ASSERT_EQ((uint16_t)0xF00, unpackedData[0]); // pixel0
    ASSERT_EQ((uint16_t)0x000, unpackedData[1]); // pixel1

    packedData = {0x00, 0xF0, 0x00};
    ASSERT_NO_THROW(karabo::util::unpackMono12Packed(packedData.data(), 2, 1, unpackedData.data()));
    ASSERT_EQ((uint16_t)0x000, unpackedData[0]); // pixel0
    ASSERT_EQ((uint16_t)0x00F, unpackedData[1]); // pixel1

    packedData = {0x00, 0x00, 0x0F};
    ASSERT_NO_THROW(karabo::util::unpackMono12Packed(packedData.data(), 2, 1, unpackedData.data()));
    ASSERT_EQ((uint16_t)0x000, unpackedData[0]); // pixel0
    ASSERT_EQ((uint16_t)0x0F0, unpackedData[1]); // pixel1

    packedData = {0x00, 0x00, 0xF0};
    ASSERT_NO_THROW(karabo::util::unpackMono12Packed(packedData.data(), 2, 1, unpackedData.data()));
    ASSERT_EQ((uint16_t)0x000, unpackedData[0]); // pixel0
    ASSERT_EQ((uint16_t)0xF00, unpackedData[1]); // pixel1
}

TEST(UnpackTests, Mono12p) {
    std::vector<uint8_t> packedData(5);
    std::vector<uint16_t> unpackedData(3);

    packedData = {0xBC, 0xFA, 0xDE};
    ASSERT_NO_THROW(karabo::util::unpackMono12p(packedData.data(), 2, 1, unpackedData.data()));
    ASSERT_EQ((uint16_t)0xABC, unpackedData[0]); // pixel0
    ASSERT_EQ((uint16_t)0xDEF, unpackedData[1]); // pixel1

    packedData = {0x0F, 0x00, 0x00};
    ASSERT_NO_THROW(karabo::util::unpackMono12p(packedData.data(), 2, 1, unpackedData.data()));
    ASSERT_EQ((uint16_t)0x00F, unpackedData[0]); // pixel0
    ASSERT_EQ((uint16_t)0x000, unpackedData[1]); // pixel1

    packedData = {0xF0, 0x00, 0x00};
    ASSERT_NO_THROW(karabo::util::unpackMono12p(packedData.data(), 2, 1, unpackedData.data()));
    ASSERT_EQ((uint16_t)0x0F0, unpackedData[0]); // pixel0
    ASSERT_EQ((uint16_t)0x000, unpackedData[1]); // pixel1

    packedData = {0x00, 0x0F, 0x00};
    ASSERT_NO_THROW(karabo::util::unpackMono12p(packedData.data(), 2, 1, unpackedData.data()));
    ASSERT_EQ((uint16_t)0xF00, unpackedData[0]); // pixel0
    ASSERT_EQ((uint16_t)0x000, unpackedData[1]); // pixel1

    packedData = {0x00, 0xF0, 0x00};
    ASSERT_NO_THROW(karabo::util::unpackMono12p(packedData.data(), 2, 1, unpackedData.data()));
    ASSERT_EQ((uint16_t)0x000, unpackedData[0]); // pixel0
    ASSERT_EQ((uint16_t)0x00F, unpackedData[1]); // pixel1

    packedData = {0x00, 0x00, 0x0F};
    ASSERT_NO_THROW(karabo::util::unpackMono12p(packedData.data(), 2, 1, unpackedData.data()));
    ASSERT_EQ((uint16_t)0x000, unpackedData[0]); // pixel0
    ASSERT_EQ((uint16_t)0x0F0, unpackedData[1]); // pixel1

    packedData = {0x00, 0x00, 0xF0};
    ASSERT_NO_THROW(karabo::util::unpackMono12p(packedData.data(), 2, 1, unpackedData.data()));
    ASSERT_EQ((uint16_t)0x000, unpackedData[0]); // pixel0
    ASSERT_EQ((uint16_t)0xF00, unpackedData[1]); // pixel1

    packedData = {0xBC, 0xFA, 0xDE, 0x23, 0x01};
    ASSERT_NO_THROW(karabo::util::unpackMono12p(packedData.data(), 3, 1, unpackedData.data()));
    ASSERT_EQ((uint16_t)0xABC, unpackedData[0]); // pixel0
    ASSERT_EQ((uint16_t)0xDEF, unpackedData[1]); // pixel1
    ASSERT_EQ((uint16_t)0x123, unpackedData[2]); // pixel2
}

TEST(UnpackTests, Mono10p) {
    std::vector<uint8_t> packedData(5);
    std::vector<uint16_t> unpackedData(4);

    packedData = {0xAB, 0xCD, 0xEF, 0x0A, 0xBC};
    ASSERT_NO_THROW(karabo::util::unpackMono10p(packedData.data(), 2, 2, unpackedData.data()));
    ASSERT_EQ((uint16_t)0x1AB, unpackedData[0]); // pixel0
    ASSERT_EQ((uint16_t)0x3F3, unpackedData[1]); // pixel1
    ASSERT_EQ((uint16_t)0x0AE, unpackedData[2]); // pixel2
    ASSERT_EQ((uint16_t)0x2F0, unpackedData[3]); // pixel3
}

TEST(UnpackTests, MonoXXp) {
    std::vector<uint8_t> packedData(3);
    std::vector<uint16_t> unpackedData(2);

    ASSERT_THROW(karabo::util::unpackMonoXXp(packedData.data(), 2, 1, 8, unpackedData.data()),
                 karabo::util::ParameterException);

    ASSERT_THROW(karabo::util::unpackMonoXXp(packedData.data(), 2, 1, 16, unpackedData.data()),
                 karabo::util::ParameterException);
}

TEST(EncodeTests, EncodeJPEG) {
    using namespace karabo::util;
    using namespace karabo::xms;

    const std::string inImage1("../../src/test/gray21.512.raw"); // grayscale data
    const std::string outImage1("../../src/test/gray_test.jpg");

    std::ifstream ifs1(inImage1, std::fstream::binary);
    if (ifs1) {
        // Get length of file
        ifs1.seekg(0, ifs1.end);
        const size_t length = ifs1.tellg();
        ifs1.seekg(0, ifs1.beg);

        char* buffer = new char[length];

        // Read file in memory
        ifs1.read(buffer, length);
        ifs1.close();

        // Create ImageData (no copy)
        Dims dims(512, 512);
        NDArray data((unsigned char*)buffer, length, [](char* ptr) {
            delete[] ptr;
        });
        ImageData imd(data, dims, Encoding::GRAY);

        // Decode JPEG
        ASSERT_NO_THROW(karabo::util::encodeJPEG(imd));

        ASSERT_EQ((int)Encoding::JPEG, imd.getEncoding());
        ASSERT_EQ(false, imd.isIndexable());
        ASSERT_EQ((size_t)2, imd.getDimensions().rank());
        ASSERT_EQ(512ull, imd.getDimensions().x1());
        ASSERT_EQ(512ull, imd.getDimensions().x2());

        // Save encoded image to file for testing with Python
        std::ofstream ofs(outImage1, std::fstream::binary | std::fstream::trunc);
        if (ofs) {
            const NDArray& jpg_data = imd.getData();
            const size_t jpg_length = jpg_data.byteSize();
            ofs.write((char*)jpg_data.getData<unsigned char>(), jpg_length);
            ofs.close();
        }

    } else {
        // Could not open RAW test image
        throw KARABO_IO_EXCEPTION("Could not open test image " + inImage1);
    }

    const std::string inImage2("../../src/test//4.2.03.raw"); // RGB data
    const std::string outImage2("../../src/test/rgb_test.jpg");

    std::ifstream ifs2(inImage2, std::fstream::binary);
    if (ifs2) {
        // Get length of file
        ifs2.seekg(0, ifs2.end);
        const size_t length = ifs2.tellg();
        ifs2.seekg(0, ifs2.beg);

        char* buffer = new char[length];

        // Read file in memory
        ifs2.read(buffer, length);
        ifs2.close();

        // Create ImageData (no copy)
        Dims dims(512, 512, 3);
        NDArray data((unsigned char*)buffer, length, [](char* ptr) {
            delete[] ptr;
        });
        ImageData imd(data, dims, Encoding::RGB);

        // Decode JPEG
        ASSERT_NO_THROW(karabo::util::encodeJPEG(imd));

        ASSERT_EQ((int)Encoding::JPEG, imd.getEncoding());
        ASSERT_EQ(false, imd.isIndexable());
        ASSERT_EQ((size_t)3, imd.getDimensions().rank());
        ASSERT_EQ(512ull, imd.getDimensions().x1());
        ASSERT_EQ(512ull, imd.getDimensions().x2());
        ASSERT_EQ(3ull, imd.getDimensions().x3());

        // Save encoded image to file for testing with Python
        std::ofstream ofs(outImage2, std::fstream::binary | std::fstream::trunc);
        if (ofs) {
            const NDArray& jpg_data = imd.getData();
            const size_t jpg_length = jpg_data.byteSize();
            ofs.write((char*)jpg_data.getData<unsigned char>(), jpg_length);
            ofs.close();
        }

    } else {
        // Could not open RAW test image
        throw KARABO_IO_EXCEPTION("Could not open test image " + inImage2);
    }

    const std::string inImage3("../../src/test/rgb16_test.raw"); // RGB16 data
    const std::string outImage3("../../src/test/rgb16_test.jpg");

    std::ifstream ifs3(inImage3, std::fstream::binary);
    if (ifs3) {
        // Get length of file
        ifs3.seekg(0, ifs3.end);
        const size_t length = ifs3.tellg();
        ifs3.seekg(0, ifs3.beg);

        char* buffer = new char[length];

        // Read file in memory
        ifs3.read(buffer, length);
        ifs3.close();

        // Create ImageData (no copy)
        Dims dims(512, 512, 3);
        NDArray data((unsigned short*)buffer, length / sizeof(unsigned short),
                     [](char* ptr) {
                         delete[] ptr;
                     },
                     dims);
        ImageData imd(data, Encoding::RGB);

        // Decode JPEG
        ASSERT_NO_THROW(karabo::util::encodeJPEG(imd));

        ASSERT_EQ((int)Encoding::JPEG, imd.getEncoding());
        ASSERT_EQ(false, imd.isIndexable());
        ASSERT_EQ((size_t)3, imd.getDimensions().rank());
        ASSERT_EQ(512ull, imd.getDimensions().x1());
        ASSERT_EQ(512ull, imd.getDimensions().x2());
        ASSERT_EQ(3ull, imd.getDimensions().x3());

        // Save encoded image to file for testing with Python
        std::ofstream ofs(outImage3, std::fstream::binary | std::fstream::trunc);
        if (ofs) {
            const NDArray& jpg_data = imd.getData();
            const size_t jpg_length = jpg_data.byteSize();
            ofs.write((char*)jpg_data.getData<unsigned char>(), jpg_length);
            ofs.close();
        }

    } else {
        // Could not open RAW test image
        throw KARABO_IO_EXCEPTION("Could not open test image " + inImage3);
    }
}

TEST(EncodeTests, DecodeJPEG) {
    using namespace karabo::util;
    using namespace karabo::xms;

    const std::string inImage1("../../src/test/gray_test.jpg"); // created by previous test
    const std::string outImage1("../../src/test/gray_test.raw");

    std::ifstream ifs1(inImage1, std::fstream::binary);
    if (ifs1) {
        // Get length of file
        ifs1.seekg(0, ifs1.end);
        const size_t length = ifs1.tellg();
        ifs1.seekg(0, ifs1.beg);

        char* buffer = new char[length];

        // Read file in memory
        ifs1.read(buffer, length);
        ifs1.close();

        // Create ImageData (no copy)
        Dims dims(512, 512);
        NDArray data((unsigned char*)buffer, length, [](char* ptr) {
            delete[] ptr;
        });
        ImageData imd(data, dims, Encoding::JPEG);

        // Decode JPEG
        ASSERT_NO_THROW(karabo::util::decodeJPEG(imd));

        ASSERT_EQ((int)Encoding::GRAY, imd.getEncoding());
        ASSERT_EQ(true, imd.isIndexable());
        ASSERT_EQ((size_t)2, imd.getDimensions().rank());
        ASSERT_EQ(512ull, imd.getDimensions().x1());
        ASSERT_EQ(512ull, imd.getDimensions().x2());

        // Save decoded image to file for testing with Python
        std::ofstream ofs(outImage1, std::fstream::binary | std::fstream::trunc);
        if (ofs) {
            const NDArray& rgb_data = imd.getData();
            const size_t rgb_length = rgb_data.byteSize();
            ofs.write((char*)rgb_data.getData<unsigned char>(), rgb_length);
            ofs.close();
        }

    } else {
        // Could not open JPEG test image
        throw KARABO_IO_EXCEPTION("Could not open test image " + inImage1);
    }

    const std::string inImage2("../../src/test/rgb_test.jpg"); // created by previous test
    const std::string outImage2("../../src/test/rgb_test.raw");

    std::ifstream ifs2(inImage2, std::fstream::binary);
    if (ifs1) {
        // Get length of file
        ifs2.seekg(0, ifs2.end);
        const size_t length = ifs2.tellg();
        ifs2.seekg(0, ifs2.beg);

        char* buffer = new char[length];

        // Read file in memory
        ifs2.read(buffer, length);
        ifs2.close();

        // Create ImageData (no copy)
        Dims dims(512, 512, 3);
        NDArray data((unsigned char*)buffer, length, [](char* ptr) {
            delete[] ptr;
        });
        ImageData imd(data, dims, Encoding::JPEG);

        // Decode JPEG
        ASSERT_NO_THROW(karabo::util::decodeJPEG(imd));

        // ASSERT_EQ((int)Encoding::RGB, imd.getEncoding());
        ASSERT_EQ(true, imd.isIndexable());
        ASSERT_EQ((size_t)3, imd.getDimensions().rank());
        ASSERT_EQ(512ull, imd.getDimensions().x1());
        ASSERT_EQ(512ull, imd.getDimensions().x2());
        ASSERT_EQ(3ull, imd.getDimensions().x3());

        // Save decoded image to file for testing with Python
        std::ofstream ofs(outImage2, std::fstream::binary | std::fstream::trunc);
        if (ofs) {
            const NDArray& rgb_data = imd.getData();
            const size_t rgb_length = rgb_data.byteSize();
            ofs.write((char*)rgb_data.getData<unsigned char>(), rgb_length);
            ofs.close();
        }

    } else {
        // Could not open JPEG test image
        throw KARABO_IO_EXCEPTION("Could not open test image " + inImage2);
    }
}

TEST(RotateTests, Rotate) {
    using namespace karabo::util;
    using namespace karabo::xms;

    // Test 90 degrees rotation
    {
        uint8_t data_in[] = {
            0x01, 0x02, 0x03, 0x04,
            0x05, 0x06, 0x07, 0x08,
            0x09, 0x0A, 0x0B, 0x0C};

        uint8_t expected_data[] = {
            0x09, 0x05, 0x01,
            0x0A, 0x06, 0x02,
            0x0B, 0x07, 0x03,
            0x0C, 0x08, 0x04};

        const Dims shape(3, 4);
        NDArray arr_in(data_in, shape.size(), NDArray::NullDeleter(), shape);
        ImageData imd(arr_in);

        karabo::util::rotateImage(imd, 90);

        const NDArray& arr_out = imd.getData();
        const uint8_t* data_out = arr_out.getData<uint8_t>();
        ASSERT_EQ(shape.x2(), arr_out.getShape().x1());
        ASSERT_EQ(shape.x1(), arr_out.getShape().x2());
        ASSERT_EQ(shape.rank(), arr_out.getShape().rank());
        ASSERT_EQ(int(Rotation::ROT_90), imd.getRotation());
        for (size_t i = 0; i < 12; ++i) {
            ASSERT_EQ(expected_data[i], data_out[i]);
        }
    }

    // Test 180 degrees rotation
    {
        uint16_t data_in[] = {
            0x01, 0x02, 0x03, 0x04,
            0x05, 0x06, 0x07, 0x08,
            0x09, 0x0A, 0x0B, 0x0C};

        uint16_t expected_data[] = {
            0x0C, 0x0B, 0x0A, 0x09,
            0x08, 0x07, 0x06, 0x05,
            0x04, 0x03, 0x02, 0x01};

        const Dims shape(3, 4);
        NDArray arr_in(data_in, shape.size(), NDArray::NullDeleter(), shape);
        ImageData imd(arr_in);
        imd.setRotation(Rotation::ROT_90);

        karabo::util::rotateImage(imd, 180);

        const NDArray& arr_out = imd.getData();
        const uint16_t* data_out = arr_out.getData<uint16_t>();
        ASSERT_EQ(shape.x1(), arr_out.getShape().x1());
        ASSERT_EQ(shape.x2(), arr_out.getShape().x2());
        ASSERT_EQ(shape.rank(), arr_out.getShape().rank());
        ASSERT_EQ(int(Rotation::ROT_270), imd.getRotation());
        for (size_t i = 0; i < 12; ++i) {
            ASSERT_EQ(expected_data[i], data_out[i]);
        }
    }

    // Test 270 degrees rotation
    {
        uint32_t data_in[] = {
            0x01, 0x02, 0x03, 0x04,
            0x05, 0x06, 0x07, 0x08,
            0x09, 0x0A, 0x0B, 0x0C};

        uint32_t expected_data[] = {
            0x04, 0x08, 0x0C,
            0x03, 0x07, 0x0B,
            0x02, 0x06, 0x0A,
            0x01, 0x05, 0x09};

        const Dims shape(3, 4);
        uint32_t buffer[shape.size()];
        NDArray arr_in(data_in, shape.size(), NDArray::NullDeleter(), shape);
        ImageData imd(arr_in);
        imd.setRotation(Rotation::ROT_180);

        karabo::util::rotateImage(imd, 270, reinterpret_cast<void*>(buffer));

        const NDArray& arr_out = imd.getData();
        const uint32_t* data_out = arr_out.getData<uint32_t>();
        ASSERT_EQ(shape.x2(), arr_out.getShape().x1());
        ASSERT_EQ(shape.x1(), arr_out.getShape().x2());
        ASSERT_EQ(shape.rank(), arr_out.getShape().rank());
        ASSERT_EQ(int(Rotation::ROT_90), imd.getRotation());
        for (size_t i = 0; i < 12; ++i) {
            ASSERT_EQ(expected_data[i], data_out[i]);
        }
    }

    {
        uint8_t data_in[] = {0x01, 0x02, 0x03, 0x04,
                             0x05, 0x06, 0x07, 0x08,
                             0x09, 0x0A, 0x0B, 0x0C};
        const Dims shape(3, 4);
        NDArray arr_in(data_in, shape.size(), NDArray::NullDeleter(), shape);
        ImageData imd(arr_in);

        ASSERT_THROW(karabo::util::rotateImage(imd, 1), karabo::util::ParameterException);
    }
}

TEST(FlipTests, Flip) {
    using namespace karabo::util;
    using namespace karabo::xms;

    // Test horizontal flip
    {
        uint8_t data_in[] = {
            0x01, 0x02, 0x03, 0x04,
            0x05, 0x06, 0x07, 0x08,
            0x09, 0x0A, 0x0B, 0x0C};

        uint8_t expected_data[] = {
            0x04, 0x03, 0x02, 0x01,
            0x08, 0x07, 0x06, 0x05,
            0x0C, 0x0B, 0x0A, 0x09};

        const Dims shape(3, 4);
        NDArray arr_in(data_in, shape.size(), NDArray::NullDeleter(), shape);
        ImageData imd(arr_in);

        karabo::util::flipImage(imd, true, false);

        const NDArray& arr_out = imd.getData();
        const uint8_t* data_out = arr_out.getData<uint8_t>();
        ASSERT_EQ(shape.x1(), arr_out.getShape().x1());
        ASSERT_EQ(shape.x2(), arr_out.getShape().x2());
        ASSERT_EQ(shape.rank(), arr_out.getShape().rank());
        ASSERT_EQ(true, imd.getFlipX());
        ASSERT_EQ(false, imd.getFlipY());

        for (size_t i = 0; i < 12; ++i) {
            ASSERT_EQ(expected_data[i], data_out[i]);
        }
    }

    // Test vertical flip
    {
        uint16_t data_in[] = {
            0x01, 0x02, 0x03, 0x04,
            0x05, 0x06, 0x07, 0x08,
            0x09, 0x0A, 0x0B, 0x0C};

        uint16_t expected_data[] = {
            0x09, 0x0A, 0x0B, 0x0C,
            0x05, 0x06, 0x07, 0x08,
            0x01, 0x02, 0x03, 0x04};

        const Dims shape(3, 4);
        NDArray arr_in(data_in, shape.size(), NDArray::NullDeleter(), shape);
        ImageData imd(arr_in);

        karabo::util::flipImage(imd, false, true);

        const NDArray& arr_out = imd.getData();
        const uint16_t* data_out = arr_out.getData<uint16_t>();
        ASSERT_EQ(shape.x1(), arr_out.getShape().x1());
        ASSERT_EQ(shape.x2(), arr_out.getShape().x2());
        ASSERT_EQ(shape.rank(), arr_out.getShape().rank());
        ASSERT_EQ(false, imd.getFlipX());
        ASSERT_EQ(true, imd.getFlipY());

        for (size_t i = 0; i < 12; ++i) {
            ASSERT_EQ(expected_data[i], data_out[i]);
        }
    }

    // Test horizonzal + vertical flip
    {
        uint32_t data_in[] = {
            0x01, 0x02, 0x03, 0x04,
            0x05, 0x06, 0x07, 0x08,
            0x09, 0x0A, 0x0B, 0x0C};

        uint32_t expected_data[] = {
            0x0C, 0x0B, 0x0A, 0x09,
            0x08, 0x07, 0x06, 0x05,
            0x04, 0x03, 0x02, 0x01};

        const Dims shape(3, 4);
        NDArray arr_in(data_in, shape.size(), NDArray::NullDeleter(), shape);
        ImageData imd(arr_in);

        karabo::util::flipImage(imd, true, true);

        const NDArray& arr_out = imd.getData();
        const uint32_t* data_out = arr_out.getData<uint32_t>();
        ASSERT_EQ(shape.x1(), arr_out.getShape().x1());
        ASSERT_EQ(shape.x2(), arr_out.getShape().x2());
        ASSERT_EQ(shape.rank(), arr_out.getShape().rank());
        ASSERT_EQ(true, imd.getFlipX());
        ASSERT_EQ(true, imd.getFlipY());

        for (size_t i = 0; i < 12; ++i) {
            ASSERT_EQ(expected_data[i], data_out[i]);
        }
    }
}
