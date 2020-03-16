/*
 * File:   ImageSourceTest.hh
 * Author: parenti
 *
 * Created on Mar 12, 2020, 6:11:23 PM
 */

#ifndef IMAGESOURCETEST_HH
#define	IMAGESOURCETEST_HH

#include <cppunit/extensions/HelperMacros.h>

#include <karabo/karabo.hpp>

class ImageSourceTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(ImageSourceTest);

    CPPUNIT_TEST(shouldCreateImageSource);
    CPPUNIT_TEST(shouldCreateCameraImageSource);
    CPPUNIT_TEST(shouldUnpackData);

    CPPUNIT_TEST_SUITE_END();

public:
    ImageSourceTest();
    virtual ~ImageSourceTest();
    void setUp();
    void tearDown();

private:
    void shouldCreateImageSource();
    void shouldCreateCameraImageSource();
    void shouldUnpackData();

private:
    karabo::util::Hash m_config;
};

#endif	/* IMAGESOURCETEST_HH */

