/*
 * Author: parenti
 *
 * Created on August 24, 2022, 09:41 AM
 *
 * Copyright (c) European XFEL GmbH Schenefeld. All rights reserved.
 */

#include <gtest/gtest.h>

#include <memory>
#include <thread>
#include <utility>

#include "CameraImageSource.hh"
#include "ImageSource.hh"
#include "karabo/data/types/Hash.hh"
#include "testrunner.hh"


#define DEVICE_SERVER_ID "testDeviceSrvCpp"
#define TEST_DEVICE_ID "testImageSource"

#define LOG_PRIORITY "FATAL" // Can also be "DEBUG", "INFO" or "ERROR"
#define DEV_CLI_TIMEOUT_SEC 2

using namespace ::testing;

/*
 * This is a helper class to test ImageSource::updateOutputSchema. The
 * function cannot be tested directly as it is a protected member of
 * ImageSource.
 */
class ImageSource2 : public karabo::ImageSource {
   public:
    KARABO_CLASSINFO(ImageSource2, "ImageSource2", "1.0")

    explicit ImageSource2(const karabo::data::Hash& config) : karabo::ImageSource(config) {};

    void updateOutputSchemaPublic(const std::vector<unsigned long long>& shape, const karabo::xms::Encoding& encoding,
                                  const karabo::data::Types::ReferenceType& kType) {
        this->updateOutputSchema(shape, encoding, kType);
   }
};


KARABO_REGISTER_FOR_CONFIGURATION(karabo::core::Device, karabo::ImageSource, ImageSource2)


/**
 * @brief Test fixture for the ImageSource device class.
 */
class ImageSourceFixture : public KaraboDeviceFixture {
   protected:
    ImageSourceFixture() = default;

    void SetUp() {
        karabo::data::Hash devCfg("deviceId", TEST_DEVICE_ID);

        /**
         * Add configuration for this 'DefaultCfg' test fixture
         * to the devCfg has here
         */


        // instantiate the device to be tested
        instantiateAndGetPointer("ImageSource2", TEST_DEVICE_ID, devCfg, base_device);
        // cast the Device::Pointer to the derived mocked class Pointer
        imgsrc_device = std::dynamic_pointer_cast<ImageSource2>(base_device);

        /**
         * Add default expectations for this test fixture here
         */
    }

    void TearDown() override {
        // ASSERT_NO_THROW(
        //     m_deviceCli->killDevice(TEST_DEVICE_ID, DEV_CLI_TIMEOUT_SEC))
        //<< "Failed to deinstantiate device '" << TEST_DEVICE_ID << "'";
    }

    karabo::core::Device::Pointer base_device;
    ImageSource2::Pointer imgsrc_device;
};

// test only that device instantiates
TEST_F(ImageSourceFixture, testDeviceInstantiation) {
    karabo::data::Hash result = m_deviceCli->get(TEST_DEVICE_ID);
    std::string cls = result.get<std::string>("classId");
    std::string clsVer = result.get<std::string>("classVersion");

    std::cout << std::endl;
    std::cout << "Device under test is class " << cls;
    std::cout << ", version " << clsVer;
    std::cout << std::endl;
    std::cout << std::endl;

    ASSERT_STREQ(cls.c_str(), "ImageSource2");
}

// arguments to TEST are just strings to name your tests
TEST_F(ImageSourceFixture, testSchemaUpdate) {
    const std::vector<unsigned long long> shape = {1080, 1920};
    const karabo::xms::Encoding encoding = karabo::xms::Encoding::RGB;
    const karabo::data::Types::ReferenceType kType = karabo::data::Types::UINT8;

    const karabo::data::Schema& schema = imgsrc_device->getFullSchema();
    ASSERT_NE(shape, schema.getDefaultValue<std::vector<unsigned long long>>("output.schema.data.image.dims"));
    ASSERT_NE(static_cast<int>(encoding), schema.getDefaultValue<int>("output.schema.data.image.encoding"));
    ASSERT_NE(kType, schema.getDefaultValue<int>("output.schema.data.image.pixels.type"));

    imgsrc_device->updateOutputSchemaPublic(shape, encoding, kType);
    const karabo::data::Schema& updated_schema = imgsrc_device->getFullSchema();
    ASSERT_EQ(shape, updated_schema.getDefaultValue<std::vector<unsigned long long>>("output.schema.data.image.dims"));
    ASSERT_EQ(static_cast<int>(encoding), updated_schema.getDefaultValue<int>("output.schema.data.image.encoding"));
    ASSERT_EQ(kType, updated_schema.getDefaultValue<int>("output.schema.data.image.pixels.type"));
}

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

    ASSERT_THROW(karabo::util::unpackAnyFormat(packedData.data(), 2, 8, unpackedData.data()),
                 karabo::data::ParameterException);

    ASSERT_THROW(karabo::util::unpackAnyFormat(packedData.data(), 2, 16, unpackedData.data()),
                 karabo::data::ParameterException);
}

TEST(UnpackTests, BayerRG10p) {
    std::vector<uint8_t> packedData(20);
    std::vector<uint16_t> unpackedData(16); // 8 x 2 image

    packedData = {
        0b1, 0b100, 0b100000, 0b10000000, 0,
        0b11, 0b1100, 0b1000000, 0, 0b1,
        0b101, 0b10100, 0b1100000, 0b10000000, 0b1,
        0b111, 0b11100, 0b10000000, 0, 0b10};

    ASSERT_NO_THROW(karabo::util::unpackBayerRG10p(packedData.data(), 8, 2, unpackedData.data()));
    ASSERT_EQ((uint16_t)0b1, unpackedData[0]); // pixel0
    ASSERT_EQ((uint16_t)0b1, unpackedData[1]); // pixel1
    ASSERT_EQ((uint16_t)0b10, unpackedData[2]); // pixel2
    ASSERT_EQ((uint16_t)0b10, unpackedData[3]); // pixel3
    ASSERT_EQ((uint16_t)0b11, unpackedData[4]); // pixel4
    ASSERT_EQ((uint16_t)0b11, unpackedData[5]); // pixel5
    ASSERT_EQ((uint16_t)0b100, unpackedData[6]); // pixel6
    ASSERT_EQ((uint16_t)0b100, unpackedData[7]); // pixel7
    ASSERT_EQ((uint16_t)0b101, unpackedData[8]); // pixel8
    ASSERT_EQ((uint16_t)0b101, unpackedData[9]); // pixel9
    ASSERT_EQ((uint16_t)0b110, unpackedData[10]); // pixel10
    ASSERT_EQ((uint16_t)0b110, unpackedData[11]); // pixel11
    ASSERT_EQ((uint16_t)0b111, unpackedData[12]); // pixel12
    ASSERT_EQ((uint16_t)0b111, unpackedData[13]); // pixel13
    ASSERT_EQ((uint16_t)0b1000, unpackedData[14]); // pixel14
    ASSERT_EQ((uint16_t)0b1000, unpackedData[15]); // pixel15
}

TEST(UnpackTests, BayerRG12p) {
    std::vector<uint8_t> packedData(24);
    std::vector<uint16_t> unpackedData(16); // 8 x 2 image

    packedData = {
        0b1, 0b10000, 0, 0b10, 0b100000, 0,
        0b11, 0b110000, 0, 0b100, 0b1000000, 0,
        0b101, 0b1010000, 0, 0b110, 0b1100000, 0,
        0b111, 0b1110000, 0, 0b1000, 0b10000000, 0};

    ASSERT_NO_THROW(karabo::util::unpackBayerRG12p(packedData.data(), 8, 2, unpackedData.data()));
    ASSERT_EQ((uint16_t)0b1, unpackedData[0]); // pixel0
    ASSERT_EQ((uint16_t)0b1, unpackedData[1]); // pixel1
    ASSERT_EQ((uint16_t)0b10, unpackedData[2]); // pixel2
    ASSERT_EQ((uint16_t)0b10, unpackedData[3]); // pixel3
    ASSERT_EQ((uint16_t)0b11, unpackedData[4]); // pixel4
    ASSERT_EQ((uint16_t)0b11, unpackedData[5]); // pixel5
    ASSERT_EQ((uint16_t)0b100, unpackedData[6]); // pixel6
    ASSERT_EQ((uint16_t)0b100, unpackedData[7]); // pixel7
    ASSERT_EQ((uint16_t)0b101, unpackedData[8]); // pixel8
    ASSERT_EQ((uint16_t)0b101, unpackedData[9]); // pixel9
    ASSERT_EQ((uint16_t)0b110, unpackedData[10]); // pixel10
    ASSERT_EQ((uint16_t)0b110, unpackedData[11]); // pixel11
    ASSERT_EQ((uint16_t)0b111, unpackedData[12]); // pixel12
    ASSERT_EQ((uint16_t)0b111, unpackedData[13]); // pixel13
    ASSERT_EQ((uint16_t)0b1000, unpackedData[14]); // pixel14
    ASSERT_EQ((uint16_t)0b1000, unpackedData[15]); // pixel15
}

TEST(EncodeTests, EncodeJPEG) {
    using namespace karabo::util;
    using namespace karabo::data;
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
        NDArray data((unsigned char*)buffer, length, [](char* ptr) { delete[] ptr; });
        ImageData imd(data, dims, Encoding::GRAY);

        // Decode JPEG
        ASSERT_NO_THROW(karabo::util::encodeJPEG(imd));

        ASSERT_EQ(Encoding::JPEG, imd.getEncoding());
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
        NDArray data((unsigned char*)buffer, length, [](char* ptr) { delete[] ptr; });
        ImageData imd(data, dims, Encoding::RGB);

        // Decode JPEG
        ASSERT_NO_THROW(karabo::util::encodeJPEG(imd));

        ASSERT_EQ(Encoding::JPEG, imd.getEncoding());
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
        NDArray data((unsigned short*)buffer, length / sizeof(unsigned short), [](char* ptr) { delete[] ptr; }, dims);
        ImageData imd(data, Encoding::RGB);

        // Decode JPEG
        ASSERT_NO_THROW(karabo::util::encodeJPEG(imd));

        ASSERT_EQ(Encoding::JPEG, imd.getEncoding());
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
    using namespace karabo::data;
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
        NDArray data((unsigned char*)buffer, length, [](char* ptr) { delete[] ptr; });
        ImageData imd(data, dims, Encoding::JPEG);

        // Decode JPEG
        ASSERT_NO_THROW(karabo::util::decodeJPEG(imd));

        ASSERT_EQ(Encoding::GRAY, imd.getEncoding());
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
        NDArray data((unsigned char*)buffer, length, [](char* ptr) { delete[] ptr; });
        ImageData imd(data, dims, Encoding::JPEG);

        // Decode JPEG
        ASSERT_NO_THROW(karabo::util::decodeJPEG(imd));

        // ASSERT_EQ(Encoding::RGB, imd.getEncoding());
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
    using namespace karabo::data;
    using namespace karabo::xms;

    // Test 90 degrees rotation
    {
        uint8_t data_in[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C};

        uint8_t expected_data[] = {0x09, 0x05, 0x01, 0x0A, 0x06, 0x02, 0x0B, 0x07, 0x03, 0x0C, 0x08, 0x04};

        const Dims shape(3, 4);
        NDArray arr_in(data_in, shape.size(), NDArray::NullDeleter(), shape);
        ImageData imd(arr_in);

        karabo::util::rotateImage(imd, 90);

        const NDArray& arr_out = imd.getData();
        const uint8_t* data_out = arr_out.getData<uint8_t>();
        ASSERT_EQ(shape.x2(), arr_out.getShape().x1());
        ASSERT_EQ(shape.x1(), arr_out.getShape().x2());
        ASSERT_EQ(shape.rank(), arr_out.getShape().rank());
        ASSERT_EQ(Rotation::ROT_90, imd.getRotation());
        for (size_t i = 0; i < 12; ++i) {
            ASSERT_EQ(expected_data[i], data_out[i]);
        }
    }

    // Test 90 degress RGB image rotation
    {
        uint8_t data_in[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12};

        uint8_t expected_data[] = {0x0A, 0x0B, 0x0C, 0x01, 0x02, 0x03, 0x0D, 0x0E, 0x0F, 0x04, 0x05, 0x06, 0x10, 0x11, 0x12, 0x07, 0x08, 0x09};

        const Dims shape(2, 3, 3); // height, width, channels
        NDArray arr_in(data_in, shape.size(), NDArray::NullDeleter(), shape);
        ImageData imd(arr_in);

        karabo::util::rotateImage(imd, 90);

        const NDArray& arr_out = imd.getData();
        const uint8_t* data_out = arr_out.getData<uint8_t>();
        ASSERT_EQ(shape.x2(), arr_out.getShape().x1());
        ASSERT_EQ(shape.x1(), arr_out.getShape().x2());
        ASSERT_EQ(shape.x3(), arr_out.getShape().x3());
        ASSERT_EQ(shape.rank(), arr_out.getShape().rank());
        ASSERT_EQ(Rotation::ROT_90, imd.getRotation());
        for (size_t i = 0; i < 18; ++i) {
            ASSERT_EQ(expected_data[i], data_out[i]);
        }
        std::cout << std::endl;
    }

    // Test 180 degrees rotation
    {
        uint16_t data_in[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C};

        uint16_t expected_data[] = {0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};

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
        ASSERT_EQ(Rotation::ROT_270, imd.getRotation());
        for (size_t i = 0; i < 12; ++i) {
            ASSERT_EQ(expected_data[i], data_out[i]);
        }
    }

    // Test 270 degrees rotation
    {
        uint32_t data_in[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C};

        uint32_t expected_data[] = {0x04, 0x08, 0x0C, 0x03, 0x07, 0x0B, 0x02, 0x06, 0x0A, 0x01, 0x05, 0x09};

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
        ASSERT_EQ(Rotation::ROT_90, imd.getRotation());
        for (size_t i = 0; i < 12; ++i) {
            ASSERT_EQ(expected_data[i], data_out[i]);
        }
    }

    {
        uint8_t data_in[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C};
        const Dims shape(3, 4);
        NDArray arr_in(data_in, shape.size(), NDArray::NullDeleter(), shape);
        ImageData imd(arr_in);

        ASSERT_THROW(karabo::util::rotateImage(imd, 1), karabo::data::ParameterException);
    }
}

TEST(FlipTests, Flip) {
    using namespace karabo::util;
    using namespace karabo::data;
    using namespace karabo::xms;

    // Test horizontal flip
    {
        uint8_t data_in[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C};

        uint8_t expected_data[] = {0x04, 0x03, 0x02, 0x01, 0x08, 0x07, 0x06, 0x05, 0x0C, 0x0B, 0x0A, 0x09};

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
        uint16_t data_in[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C};

        uint16_t expected_data[] = {0x09, 0x0A, 0x0B, 0x0C, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04};

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
        uint32_t data_in[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C};

        uint32_t expected_data[] = {0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};

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

    // Test horizonzal + vertical flip, RGB image
    {
        uint32_t data_in[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12};

        uint32_t expected_data[] = {0x10, 0x11, 0x12, 0x0D, 0x0E, 0x0F, 0x0A, 0x0B, 0x0C, 0x07, 0x08, 0x09, 0x04, 0x05, 0x06, 0x01, 0x02, 0x03};

        const Dims shape(2, 3, 3); // height, width, channels
        NDArray arr_in(data_in, shape.size(), NDArray::NullDeleter(), shape);
        ImageData imd(arr_in);

        karabo::util::flipImage(imd, true, true);

        const NDArray& arr_out = imd.getData();
        const uint32_t* data_out = arr_out.getData<uint32_t>();
        ASSERT_EQ(shape.x1(), arr_out.getShape().x1());
        ASSERT_EQ(shape.x2(), arr_out.getShape().x2());
        ASSERT_EQ(shape.x3(), arr_out.getShape().x3());
        ASSERT_EQ(shape.rank(), arr_out.getShape().rank());
        ASSERT_EQ(true, imd.getFlipX());
        ASSERT_EQ(true, imd.getFlipY());

        for (size_t i = 0; i < 18; ++i) {
            ASSERT_EQ(expected_data[i], data_out[i]);
        }
    }
}
