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

void ImageSourceTest::shouldUnpackData() {
    using namespace karabo::util;

    std::vector<uint8_t> packedData(3);
    std::vector<uint16_t> unpackedData(2);

    packedData = {0xAB, 0xFC, 0xDE};
    CPPUNIT_ASSERT_NO_THROW(unpackMono12Packed(packedData.data(), 2, 1, unpackedData.data()));
    CPPUNIT_ASSERT(unpackedData[0] == 0xABC); // pixel0
    CPPUNIT_ASSERT(unpackedData[1] == 0xDEF); // pixel1

    packedData = {0x00, 0x0F, 0x00};
    CPPUNIT_ASSERT_NO_THROW(unpackMono12Packed(packedData.data(), 2, 1, unpackedData.data()));
    CPPUNIT_ASSERT(unpackedData[0] == 0x00F); // pixel0
    CPPUNIT_ASSERT(unpackedData[1] == 0x000); // pixel1

    packedData = {0x0F, 0x00, 0x00};
    CPPUNIT_ASSERT_NO_THROW(unpackMono12Packed(packedData.data(), 2, 1, unpackedData.data()));
    CPPUNIT_ASSERT(unpackedData[0] == 0x0F0); // pixel0
    CPPUNIT_ASSERT(unpackedData[1] == 0x000); // pixel1

    packedData = {0xF0, 0x00, 0x00};
    CPPUNIT_ASSERT_NO_THROW(unpackMono12Packed(packedData.data(), 2, 1, unpackedData.data()));
    CPPUNIT_ASSERT(unpackedData[0] == 0xF00); // pixel0
    CPPUNIT_ASSERT(unpackedData[1] == 0x000); // pixel1

    packedData = {0x00, 0xF0, 0x00};
    CPPUNIT_ASSERT_NO_THROW(unpackMono12Packed(packedData.data(), 2, 1, unpackedData.data()));
    CPPUNIT_ASSERT(unpackedData[0] == 0x000); // pixel0
    CPPUNIT_ASSERT(unpackedData[1] == 0x00F); // pixel1

    packedData = {0x00, 0x00, 0x0F};
    CPPUNIT_ASSERT_NO_THROW(unpackMono12Packed(packedData.data(), 2, 1, unpackedData.data()));
    CPPUNIT_ASSERT(unpackedData[0] == 0x000); // pixel0
    CPPUNIT_ASSERT(unpackedData[1] == 0x0F0); // pixel1

    packedData = {0x00, 0x00, 0xF0};
    CPPUNIT_ASSERT_NO_THROW(unpackMono12Packed(packedData.data(), 2, 1, unpackedData.data()));
    CPPUNIT_ASSERT(unpackedData[0] == 0x000); // pixel0
    CPPUNIT_ASSERT(unpackedData[1] == 0xF00); // pixel1
}