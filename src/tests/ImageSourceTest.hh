/*
 * File:   ImageSourceTest.hh
 * Author: parenti
 *
 * Created on Mar 12, 2020, 6:11:23 PM
 */

#ifndef IMAGESOURCETEST_HH
#define IMAGESOURCETEST_HH

#include <cppunit/extensions/HelperMacros.h>

#include <karabo/karabo.hpp>

class ImageSourceTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(ImageSourceTest);

    CPPUNIT_TEST(shouldCreateImageSource);
    CPPUNIT_TEST(shouldCreateCameraImageSource);
    CPPUNIT_TEST(shouldUnpackMono12Packed);
    CPPUNIT_TEST(shouldUnpackMono12p);
    CPPUNIT_TEST(shouldUnpackMono10p);
    CPPUNIT_TEST(shouldNotUnpackMonoXXp);
    CPPUNIT_TEST(shouldEncodeJpeg);
    CPPUNIT_TEST(shouldDecodeJpeg);
    CPPUNIT_TEST(shouldRotate);
    CPPUNIT_TEST(shouldFlip);

    CPPUNIT_TEST_SUITE_END();

public:
    ImageSourceTest();
    virtual ~ImageSourceTest();
    void setUp();

private:
    void shouldCreateImageSource();
    void shouldCreateCameraImageSource();
    void shouldUnpackMono12Packed();
    void shouldUnpackMono12p();
    void shouldUnpackMono10p();
    void shouldNotUnpackMonoXXp();
    void shouldEncodeJpeg();
    void shouldDecodeJpeg();
    void shouldRotate();
    void shouldFlip();

private:
    karabo::util::Hash m_config;
};

#endif /* IMAGESOURCETEST_HH */

