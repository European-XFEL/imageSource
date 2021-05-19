/*
 * Author: <parenti>
 *
 * Created on January 7, 2020, 05:11 PM
 *
 * Copyright (c) European XFEL GmbH Hamburg. All rights reserved.
 */

#include "ImageSource.hh"

using namespace std;

USING_KARABO_NAMESPACES;

namespace karabo {

    KARABO_REGISTER_FOR_CONFIGURATION(BaseDevice, Device<>, ImageSource)


    void ImageSource::expectedParameters(Schema& expected) {
        Schema data;

        NODE_ELEMENT(data).key("data")
                .displayedName("Data")
                .setDaqDataType(DaqDataType::TRAIN)
                .commit();

        IMAGEDATA(data).key("data.image")
                .displayedName("Image")
                // Set initial dummy values for DAQ
                .setDimensions("0, 0")
                .setType(Types::UINT16)
                .setEncoding(Encoding::UNDEFINED)
                .commit();

        OUTPUT_CHANNEL(expected).key("output")
                .displayedName("Output")
                .dataSchema(data)
                .commit();

        // Second output channel for the DAQ
        OUTPUT_CHANNEL(expected).key("daqOutput")
                .displayedName("DAQ Output")
                .dataSchema(data)
                .commit();

    }


    ImageSource::ImageSource(const karabo::util::Hash& config) : Device<>(config) {
    }


    ImageSource::~ImageSource() {
    }

    void ImageSource::updateOutputSchema(const std::vector<unsigned long long>& shape, const EncodingType& encoding,
            const Types::ReferenceType& kType) {
        Schema schemaUpdate;
        this->schema_update_helper(schemaUpdate, "output", "Output", shape, encoding, kType);

        std::vector<unsigned long long> daqShape = shape;
        std::reverse(daqShape.begin(), daqShape.end()); // NB DAQ wants fastest changing index first, e.g. (width, height) or (channel, width, height)
        this->schema_update_helper(schemaUpdate, "daqOutput", "DAQ Output", daqShape, encoding, kType);

        this->appendSchema(schemaUpdate);
    }


    void ImageSource::schema_update_helper(Schema& schemaUpdate, const std::string& nodeKey,
            const std::string& displayedName, const std::vector<unsigned long long>& shape, const EncodingType& encoding,
            const Types::ReferenceType& kType) {
        Schema dataSchema;
        NODE_ELEMENT(dataSchema).key("data")
                .displayedName("Data")
                .setDaqDataType(DaqDataType::TRAIN)
                .commit();

        IMAGEDATA(dataSchema).key("data.image")
                .displayedName("Image")
                .setDimensions(karabo::util::toString(shape))
                .setType(kType)
                .setEncoding(encoding)
                .commit();

        OUTPUT_CHANNEL(schemaUpdate).key(nodeKey)
                .displayedName(displayedName)
                .dataSchema(dataSchema)
                .commit();
    }


    void ImageSource::writeChannels(const NDArray& data, const Dims& binning,
            const unsigned short bpp, const EncodingType& encoding, const Dims& roiOffsets,
            const Timestamp& timestamp, const Hash& header) {

        karabo::xms::ImageData imageData(data, encoding);
        imageData.setBitsPerPixel(bpp);
        imageData.setROIOffsets(roiOffsets);
        imageData.setBinning(binning);
        if (!header.empty()) {
            imageData.setHeader(header);
        }

        this->writeChannel("output", Hash("data.image", imageData), timestamp);

        // NB DAQ wants fastest changing index first, e.g. (width, height) or (channel, width, height)
        Dims daqShape = data.getShape();
        daqShape.reverse();

        imageData.setDimensions(daqShape);
        this->writeChannel("daqOutput", Hash("data.image", imageData), timestamp);
    }


    void ImageSource::signalEOS() {
        this->signalEndOfStream("output");
        this->signalEndOfStream("daqOutput");
    }


    void util::unpackMono12Packed(const uint8_t* data, const uint32_t width, const uint32_t height, uint16_t* unpackedData) {
        size_t idx = 0, px = 0, image_size = width * height;
        while (px+1 < image_size) {
            unpackedData[px] = (data[idx] << 4) | (data[idx+1] & 0xF);
            unpackedData[px+1] = (data[idx+2] << 4) | (data[idx+1] >> 4);
            idx += 3;
            px += 2;
        }
    }


    void util::unpackMonoXXp(const uint8_t* data, const uint32_t width, const uint32_t height, const uint8_t bpp, uint16_t* unpackedData) {
        if (bpp < 9 || bpp > 15) {
            throw KARABO_PARAMETER_EXCEPTION("Invalid bpp value: " + std::to_string(bpp) + ". It must be in [9, 15].");
        }

        const uint16_t mask = 0xFFFF >> (16 - bpp);
        size_t bits = 0, px = 0, image_size = width * height;
        while (px < image_size) {
            const size_t idx = bits / 8;
            const size_t shift = bits % 8;
            unpackedData[px] = (*reinterpret_cast<const uint16_t*>(data + idx) >> shift) & mask;

            bits += bpp;
            px += 1;
        }
    }


    void util::unpackMono10p(const uint8_t* data, const uint32_t width, const uint32_t height, uint16_t* unpackedData) {
        util::unpackMonoXXp(data, width, height, 10, unpackedData);
    }


    void util::unpackMono12p(const uint8_t* data, const uint32_t width, const uint32_t height, uint16_t* unpackedData) {
        util::unpackMonoXXp(data, width, height, 12, unpackedData);
    }
}
