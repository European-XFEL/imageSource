/*
 * File:   ImageSourceTest.cc
 * Author: parenti
 *
 * Created on Mar 12, 2020, 6:11:23 PM
 */

#include "ImageSourceTest.hh"
#include "../CameraImageSource.hh"
#include "../ImageSource.hh"

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

void ImageSourceTest::shouldEncodeJpeg() {
    using namespace karabo::util;
    using namespace karabo::xms;

    const std::string inImage1("src/tests/gray21.512.raw"); // grayscale data
    const std::string outImage1("src/tests/gray_test.jpg");

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
        CPPUNIT_ASSERT_NO_THROW(encodeJPEG(imd));

        CPPUNIT_ASSERT_EQUAL((int)Encoding::JPEG, imd.getEncoding());
        CPPUNIT_ASSERT_EQUAL(false, imd.isIndexable());
        CPPUNIT_ASSERT_EQUAL((size_t)2, imd.getDimensions().rank());
        CPPUNIT_ASSERT_EQUAL(512ull, imd.getDimensions().x1());
        CPPUNIT_ASSERT_EQUAL(512ull, imd.getDimensions().x2());

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

    const std::string inImage2("src/tests/4.2.03.raw"); // RGB data
    const std::string outImage2("src/tests/rgb_test.jpg");

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
        CPPUNIT_ASSERT_NO_THROW(encodeJPEG(imd));

        CPPUNIT_ASSERT_EQUAL((int)Encoding::JPEG, imd.getEncoding());
        CPPUNIT_ASSERT_EQUAL(false, imd.isIndexable());
        CPPUNIT_ASSERT_EQUAL((size_t)3, imd.getDimensions().rank());
        CPPUNIT_ASSERT_EQUAL(512ull, imd.getDimensions().x1());
        CPPUNIT_ASSERT_EQUAL(512ull, imd.getDimensions().x2());
        CPPUNIT_ASSERT_EQUAL(3ull, imd.getDimensions().x3());

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

    const std::string inImage3("src/tests/rgb16_test.raw"); // RGB16 data
    const std::string outImage3("src/tests/rgb16_test.jpg");

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
        CPPUNIT_ASSERT_NO_THROW(encodeJPEG(imd));

        CPPUNIT_ASSERT_EQUAL((int)Encoding::JPEG, imd.getEncoding());
        CPPUNIT_ASSERT_EQUAL(false, imd.isIndexable());
        CPPUNIT_ASSERT_EQUAL((size_t)3, imd.getDimensions().rank());
        CPPUNIT_ASSERT_EQUAL(512ull, imd.getDimensions().x1());
        CPPUNIT_ASSERT_EQUAL(512ull, imd.getDimensions().x2());
        CPPUNIT_ASSERT_EQUAL(3ull, imd.getDimensions().x3());

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

void ImageSourceTest::shouldDecodeJpeg() {
    using namespace karabo::util;
    using namespace karabo::xms;

    const std::string inImage1("src/tests/gray_test.jpg"); // created by previous test
    const std::string outImage1("src/tests/gray_test.raw");

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
        CPPUNIT_ASSERT_NO_THROW(decodeJPEG(imd));

        CPPUNIT_ASSERT_EQUAL((int)Encoding::GRAY, imd.getEncoding());
        CPPUNIT_ASSERT_EQUAL(true, imd.isIndexable());
        CPPUNIT_ASSERT_EQUAL((size_t)2, imd.getDimensions().rank());
        CPPUNIT_ASSERT_EQUAL(512ull, imd.getDimensions().x1());
        CPPUNIT_ASSERT_EQUAL(512ull, imd.getDimensions().x2());

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

    const std::string inImage2("src/tests/rgb_test.jpg"); // created by previous test
    const std::string outImage2("src/tests/rgb_test.raw");

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
        CPPUNIT_ASSERT_NO_THROW(decodeJPEG(imd));

        // CPPUNIT_ASSERT_EQUAL((int)Encoding::RGB, imd.getEncoding());
        CPPUNIT_ASSERT_EQUAL(true, imd.isIndexable());
        CPPUNIT_ASSERT_EQUAL((size_t)3, imd.getDimensions().rank());
        CPPUNIT_ASSERT_EQUAL(512ull, imd.getDimensions().x1());
        CPPUNIT_ASSERT_EQUAL(512ull, imd.getDimensions().x2());
        CPPUNIT_ASSERT_EQUAL(3ull, imd.getDimensions().x3());

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


void ImageSourceTest::shouldRotate() {
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

        rotateImage(imd, 90);

        const NDArray& arr_out = imd.getData();
        const uint8_t* data_out = arr_out.getData<uint8_t>();
        CPPUNIT_ASSERT_EQUAL(shape.x2(), arr_out.getShape().x1());
        CPPUNIT_ASSERT_EQUAL(shape.x1(), arr_out.getShape().x2());
        CPPUNIT_ASSERT_EQUAL(shape.rank(), arr_out.getShape().rank());
        CPPUNIT_ASSERT_EQUAL(int(Rotation::ROT_90), imd.getRotation());
        for (size_t i = 0; i < 12; ++i) {
            CPPUNIT_ASSERT_EQUAL(expected_data[i], data_out[i]);
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

        rotateImage(imd, 180);

        const NDArray& arr_out = imd.getData();
        const uint16_t* data_out = arr_out.getData<uint16_t>();
        CPPUNIT_ASSERT_EQUAL(shape.x1(), arr_out.getShape().x1());
        CPPUNIT_ASSERT_EQUAL(shape.x2(), arr_out.getShape().x2());
        CPPUNIT_ASSERT_EQUAL(shape.rank(), arr_out.getShape().rank());
        CPPUNIT_ASSERT_EQUAL(int(Rotation::ROT_270), imd.getRotation());
        for (size_t i = 0; i < 12; ++i) {
            CPPUNIT_ASSERT_EQUAL(expected_data[i], data_out[i]);
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

        rotateImage(imd, 270, reinterpret_cast<void*>(buffer));

        const NDArray& arr_out = imd.getData();
        const uint32_t* data_out = arr_out.getData<uint32_t>();
        CPPUNIT_ASSERT_EQUAL(shape.x2(), arr_out.getShape().x1());
        CPPUNIT_ASSERT_EQUAL(shape.x1(), arr_out.getShape().x2()); 
        CPPUNIT_ASSERT_EQUAL(shape.rank(), arr_out.getShape().rank());
        CPPUNIT_ASSERT_EQUAL(int(Rotation::ROT_90), imd.getRotation());
        for (size_t i = 0; i < 12; ++i) {
            CPPUNIT_ASSERT_EQUAL(expected_data[i], data_out[i]);
        }
    }

    {
        uint8_t data_in[] = {0x01, 0x02, 0x03, 0x04,
                             0x05, 0x06, 0x07, 0x08,
                             0x09, 0x0A, 0x0B, 0x0C};
        const Dims shape(3, 4);
        NDArray arr_in(data_in, shape.size(), NDArray::NullDeleter(), shape);
        ImageData imd(arr_in);

        CPPUNIT_ASSERT_THROW(rotateImage(imd, 1), karabo::util::ParameterException);
    }
}

void ImageSourceTest::shouldFlip() {
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

        flipImage(imd, true, false);

        const NDArray& arr_out = imd.getData();
        const uint8_t* data_out = arr_out.getData<uint8_t>();
        CPPUNIT_ASSERT_EQUAL(shape.x1(), arr_out.getShape().x1());
        CPPUNIT_ASSERT_EQUAL(shape.x2(), arr_out.getShape().x2());
        CPPUNIT_ASSERT_EQUAL(shape.rank(), arr_out.getShape().rank());
        CPPUNIT_ASSERT_EQUAL(true, imd.getFlipX());
        CPPUNIT_ASSERT_EQUAL(false, imd.getFlipY());

        for (size_t i = 0; i < 12; ++i) {
            CPPUNIT_ASSERT_EQUAL(expected_data[i], data_out[i]);
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

        flipImage(imd, false, true);

        const NDArray& arr_out = imd.getData();
        const uint16_t* data_out = arr_out.getData<uint16_t>();
        CPPUNIT_ASSERT_EQUAL(shape.x1(), arr_out.getShape().x1());
        CPPUNIT_ASSERT_EQUAL(shape.x2(), arr_out.getShape().x2());
        CPPUNIT_ASSERT_EQUAL(shape.rank(), arr_out.getShape().rank());
        CPPUNIT_ASSERT_EQUAL(false, imd.getFlipX());
        CPPUNIT_ASSERT_EQUAL(true, imd.getFlipY());

        for (size_t i = 0; i < 12; ++i) {
            CPPUNIT_ASSERT_EQUAL(expected_data[i], data_out[i]);
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

        flipImage(imd, true, true);

        const NDArray& arr_out = imd.getData();
        const uint32_t* data_out = arr_out.getData<uint32_t>();
        CPPUNIT_ASSERT_EQUAL(shape.x1(), arr_out.getShape().x1());
        CPPUNIT_ASSERT_EQUAL(shape.x2(), arr_out.getShape().x2());
        CPPUNIT_ASSERT_EQUAL(shape.rank(), arr_out.getShape().rank());
        CPPUNIT_ASSERT_EQUAL(true, imd.getFlipX());
        CPPUNIT_ASSERT_EQUAL(true, imd.getFlipY());

        for (size_t i = 0; i < 12; ++i) {
            CPPUNIT_ASSERT_EQUAL(expected_data[i], data_out[i]);
        }
    }

}
