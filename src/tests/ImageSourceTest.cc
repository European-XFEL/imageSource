/*
 * File:   ImageSourceTest.cc
 * Author: parenti
 *
 * Created on Mar 12, 2020, 6:11:23 PM
 */

#include "ImageSourceTest.hh"
#include "../ImageSource.hh"
#include "../CameraImageSource.hh"

using namespace std;
USING_KARABO_NAMESPACES;

CPPUNIT_TEST_SUITE_REGISTRATION(ImageSourceTest);

ImageSourceTest::ImageSourceTest() {
}

ImageSourceTest::~ImageSourceTest() {
}

void ImageSourceTest::setUp() {
    m_config.set("deviceId", "testdevice");
}

void ImageSourceTest::tearDown() {
}

void ImageSourceTest::shouldCreateImageSource() {
    BaseDevice::Pointer device = BaseDevice::create("ImageSource", m_config);
    CPPUNIT_ASSERT_EQUAL(string("ImageSource"), (device->getClassInfo()).getClassName());
}

void ImageSourceTest::shouldCreateCameraImageSource() {
    BaseDevice::Pointer device = BaseDevice::create("CameraImageSource", m_config);
    CPPUNIT_ASSERT_EQUAL(string("CameraImageSource"), (device->getClassInfo()).getClassName());
}

void ImageSourceTest::shouldUnpackMono12Packed() {
    using namespace karabo::util;

    std::vector<uint8_t> packedData(3);
    std::vector<uint16_t> unpackedData(2);

    packedData = {0xAB, 0xFC, 0xDE};
    CPPUNIT_ASSERT_NO_THROW(unpackMono12Packed(packedData.data(), 2, 1, unpackedData.data()));
    CPPUNIT_ASSERT_EQUAL((uint16_t)0xABC, unpackedData[0]); // pixel0
    CPPUNIT_ASSERT_EQUAL((uint16_t)0xDEF, unpackedData[1]); // pixel1

    packedData = {0x00, 0x0F, 0x00};
    CPPUNIT_ASSERT_NO_THROW(unpackMono12Packed(packedData.data(), 2, 1, unpackedData.data()));
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x00F, unpackedData[0]); // pixel0
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x000, unpackedData[1]); // pixel1

    packedData = {0x0F, 0x00, 0x00};
    CPPUNIT_ASSERT_NO_THROW(unpackMono12Packed(packedData.data(), 2, 1, unpackedData.data()));
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x0F0, unpackedData[0]); // pixel0
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x000, unpackedData[1]); // pixel1

    packedData = {0xF0, 0x00, 0x00};
    CPPUNIT_ASSERT_NO_THROW(unpackMono12Packed(packedData.data(), 2, 1, unpackedData.data()));
    CPPUNIT_ASSERT_EQUAL((uint16_t)0xF00, unpackedData[0]); // pixel0
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x000, unpackedData[1]); // pixel1

    packedData = {0x00, 0xF0, 0x00};
    CPPUNIT_ASSERT_NO_THROW(unpackMono12Packed(packedData.data(), 2, 1, unpackedData.data()));
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x000, unpackedData[0]); // pixel0
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x00F, unpackedData[1]); // pixel1

    packedData = {0x00, 0x00, 0x0F};
    CPPUNIT_ASSERT_NO_THROW(unpackMono12Packed(packedData.data(), 2, 1, unpackedData.data()));
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x000, unpackedData[0]); // pixel0
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x0F0, unpackedData[1]); // pixel1

    packedData = {0x00, 0x00, 0xF0};
    CPPUNIT_ASSERT_NO_THROW(unpackMono12Packed(packedData.data(), 2, 1, unpackedData.data()));
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x000, unpackedData[0]); // pixel0
    CPPUNIT_ASSERT_EQUAL((uint16_t)0xF00, unpackedData[1]); // pixel1
}

void ImageSourceTest::shouldUnpackMono12p() {
    using namespace karabo::util;

    std::vector<uint8_t> packedData(5);
    std::vector<uint16_t> unpackedData(3);

    packedData = {0xBC, 0xFA, 0xDE};
    CPPUNIT_ASSERT_NO_THROW(unpackMono12p(packedData.data(), 2, 1, unpackedData.data()));
    CPPUNIT_ASSERT_EQUAL((uint16_t)0xABC, unpackedData[0]); // pixel0
    CPPUNIT_ASSERT_EQUAL((uint16_t)0xDEF, unpackedData[1]); // pixel1

    packedData = {0x0F, 0x00, 0x00};
    CPPUNIT_ASSERT_NO_THROW(unpackMono12p(packedData.data(), 2, 1, unpackedData.data()));
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x00F, unpackedData[0]); // pixel0
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x000, unpackedData[1]); // pixel1

    packedData = {0xF0, 0x00, 0x00};
    CPPUNIT_ASSERT_NO_THROW(unpackMono12p(packedData.data(), 2, 1, unpackedData.data()));
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x0F0, unpackedData[0]); // pixel0
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x000, unpackedData[1]); // pixel1

    packedData = {0x00, 0x0F, 0x00};
    CPPUNIT_ASSERT_NO_THROW(unpackMono12p(packedData.data(), 2, 1, unpackedData.data()));
    CPPUNIT_ASSERT_EQUAL((uint16_t)0xF00, unpackedData[0]); // pixel0
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x000, unpackedData[1]); // pixel1

    packedData = {0x00, 0xF0, 0x00};
    CPPUNIT_ASSERT_NO_THROW(unpackMono12p(packedData.data(), 2, 1, unpackedData.data()));
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x000, unpackedData[0]); // pixel0
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x00F, unpackedData[1]); // pixel1

    packedData = {0x00, 0x00, 0x0F};
    CPPUNIT_ASSERT_NO_THROW(unpackMono12p(packedData.data(), 2, 1, unpackedData.data()));
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x000, unpackedData[0]); // pixel0
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x0F0, unpackedData[1]); // pixel1

    packedData = {0x00, 0x00, 0xF0};
    CPPUNIT_ASSERT_NO_THROW(unpackMono12p(packedData.data(), 2, 1, unpackedData.data()));
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x000, unpackedData[0]); // pixel0
    CPPUNIT_ASSERT_EQUAL((uint16_t)0xF00, unpackedData[1]); // pixel1

    packedData = {0xBC, 0xFA, 0xDE, 0x23, 0x01};
    CPPUNIT_ASSERT_NO_THROW(unpackMono12p(packedData.data(), 3, 1, unpackedData.data()));
    CPPUNIT_ASSERT_EQUAL((uint16_t)0xABC, unpackedData[0]); // pixel0
    CPPUNIT_ASSERT_EQUAL((uint16_t)0xDEF, unpackedData[1]); // pixel1
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x123, unpackedData[2]); // pixel2
}

void ImageSourceTest::shouldUnpackMono10p() {
    using namespace karabo::util;

    std::vector<uint8_t> packedData(5);
    std::vector<uint16_t> unpackedData(4);

    packedData = {0xAB, 0xCD, 0xEF, 0x0A, 0xBC};
    CPPUNIT_ASSERT_NO_THROW(unpackMono10p(packedData.data(), 2, 2, unpackedData.data()));
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x1AB, unpackedData[0]); // pixel0
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x3F3, unpackedData[1]); // pixel1
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x0AE, unpackedData[2]); // pixel2
    CPPUNIT_ASSERT_EQUAL((uint16_t)0x2F0, unpackedData[3]); // pixel3
}

void ImageSourceTest::shouldNotUnpackMonoXXp() {
    using namespace karabo::util;

    std::vector<uint8_t> packedData(3);
    std::vector<uint16_t> unpackedData(2);

    CPPUNIT_ASSERT_THROW(unpackMonoXXp(packedData.data(), 2, 1, 8, unpackedData.data()),
        karabo::util::ParameterException);

    CPPUNIT_ASSERT_THROW(unpackMonoXXp(packedData.data(), 2, 1, 16, unpackedData.data()),
        karabo::util::ParameterException);
}
