/*
 * Author: <parenti>
 *
 * Created on January  7, 2020, 05:11 PM
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

        // TODO: availableScenes
    }


    ImageSource::ImageSource(const karabo::util::Hash& config) : Device<>(config) {
    }


    ImageSource::~ImageSource() {
    }

    // TODO doc
    void ImageSource::updateOutputSchema(const std::vector<int>& shape, const karabo::xms::EncodingType& encoding,
            const karabo::util::Types::ReferenceType& kType) {
        Schema schemaUpdate;
        this->schema_update_helper(schemaUpdate, "output", "Output", shape, encoding, kType);

        std::vector<int> daqShape = shape;
        std::reverse(daqShape.begin(), daqShape.end()); // NB DAQ wants shape in CImg order, eg (width, height)
        this->schema_update_helper(schemaUpdate, "daqOutput", "DAQ Output", daqShape, encoding, kType);

        this->appendSchema(schemaUpdate);
    }


    void ImageSource::schema_update_helper(karabo::util::Schema& schemaUpdate, const std::string& nodeKey,
            const std::string& displayedName, const std::vector<int>& shape, const karabo::xms::EncodingType& encoding,
            const karabo::util::Types::ReferenceType& kType) {
        Schema dataSchema;
        NODE_ELEMENT(dataSchema).key("data")
                .displayedName("Data")
                .setDaqDataType(DaqDataType::TRAIN)
                .commit();

        IMAGEDATA(dataSchema).key("data.image")
                .displayedName(displayedName)
                .setDimensions(karabo::util::toString(shape))
                .setType(kType)
                .setEncoding(encoding)
                .commit();

        OUTPUT_CHANNEL(schemaUpdate).key(nodeKey)
                .displayedName(displayedName)
                .dataSchema(dataSchema)
                .commit();
    }


    // TODO docs
    void ImageSource::writeChannels(karabo::util::NDArray& data, const karabo::util::Dims& binning,
                const unsigned short bpp, const karabo::xms::EncodingType& encoding,
                const karabo::util::Dims& roiOffsets, const karabo::util::Timestamp& timestamp) {

        this->write_channel("output", data, binning, bpp, encoding, roiOffsets, timestamp);

        // Reshaped image for DAQ
        // NB DAQ wants shape in CImg order, e.g. (width, height)
        Dims daqShape = data.getShape();
        daqShape.reverse();
        NDArray daqData = data;
        daqData.setShape(daqShape);
        this->write_channel("daqOutput", daqData, binning, bpp, encoding, roiOffsets, timestamp);
    }


    void ImageSource::write_channel(const std::string& nodeKey, karabo::util::NDArray& data, const karabo::util::Dims& binning,
                const unsigned short bpp, const karabo::xms::EncodingType& encoding,
                const karabo::util::Dims& roiOffsets, const karabo::util::Timestamp& timestamp) {

        karabo::xms::ImageData imageData(data, encoding);
        imageData.setBitsPerPixel(bpp);
        imageData.setROIOffsets(roiOffsets);
        imageData.setBinning(binning);

        this->writeChannel(nodeKey, Hash("data.image", imageData), timestamp);
    }
}
