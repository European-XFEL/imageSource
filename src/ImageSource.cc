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
    KARABO_REGISTER_FOR_CONFIGURATION(BaseDevice, Device<>, ImageSource, CameraImageSource)

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

    void ImageSource::updateOutputSchema(const std::vector<int>& shape, const EncodingType& encoding,
            const Types::ReferenceType& kType) {
        Schema schemaUpdate;
        this->schema_update_helper(schemaUpdate, "output", "Output", shape, encoding, kType);

        std::vector<int> daqShape = shape;
        std::reverse(daqShape.begin(), daqShape.end()); // NB DAQ wants shape in CImg order, eg (width, height)
        this->schema_update_helper(schemaUpdate, "daqOutput", "DAQ Output", daqShape, encoding, kType);

        this->appendSchema(schemaUpdate);
    }


    void ImageSource::schema_update_helper(Schema& schemaUpdate, const std::string& nodeKey,
            const std::string& displayedName, const std::vector<int>& shape, const EncodingType& encoding,
            const Types::ReferenceType& kType) {
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


    void ImageSource::writeChannels(NDArray& data, const Dims& binning,
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

        // NB DAQ wants shape in CImg order, e.g. (width, height)
        Dims daqShape = data.getShape();
        daqShape.reverse();

        imageData.setDimensions(daqShape);
        this->writeChannel("daqOutput", Hash("data.image", imageData), timestamp);
    }


    void CameraImageSource::expectedParameters(Schema& expected) {
        VECTOR_STRING_ELEMENT(expected).key("availableScenes")
                .setSpecialDisplayType(KARABO_SCHEMA_DISPLAY_TYPE_SCENES)
                .readOnly().initialValue(std::vector<std::string>({"scene"}))
                .commit();
    }


    CameraImageSource::CameraImageSource(const karabo::util::Hash& config) : ImageSource(config) {
        KARABO_SLOT(requestScene, karabo::util::Hash)
        this->registerScene(boost::bind(&Self::scene, this), "scene");
    }


    CameraImageSource::~CameraImageSource() {
    }


    void CameraImageSource::requestScene(const karabo::util::Hash& params) {
        /* Fulfill a scene request from another device.
         */
        KARABO_LOG_FRAMEWORK_INFO << "received requestScene" << params;
        const std::string& sceneName = params.get<std::string>("name");
        Hash result("type", "deviceScene", "origin", getInstanceId());
        Hash& payload = result.bindReference<Hash>("payload");

        // look up sceneName in m_scenes
        auto it = m_scenes.find(sceneName);

        payload.set("success", false);

        // see if the scene requested is in the map of available scenes
        // NOTE: This is overkill for the moment since there is only one scene.
        if (it != m_scenes.end()) {
            const SceneFunction& f = it->second;
            if (!f.empty()) {
                payload.set("success", true);
                payload.set("name", it->first);
                payload.set("data", f());
            } else {
                KARABO_LOG_FRAMEWORK_ERROR << it->first << " has no valid function subscribed to it";
            }
        }
        reply(result);
    }


    void CameraImageSource::registerScene(const SceneFunction& sceneFunction, const std::string& funcName) {
        // add funcName as key and sceneFunction as value in m_scenes
        m_scenes[funcName] = sceneFunction;
    }
}
