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
        size_t idx = 0, px = 0;
        while (px+1 < width*height) {
            unpackedData[px] = (data[idx] << 4) | (data[idx+1] & 0xF);
            unpackedData[px+1] = (data[idx+2] << 4) | (data[idx+1] >> 4);
            idx += 3;
            px += 2;
        }
    }

}
