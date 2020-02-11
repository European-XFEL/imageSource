/*
 * Author: <parenti>
 *
 * Created on February 11, 2020, 09:14 AM
 *
 * Copyright (c) European XFEL GmbH Hamburg. All rights reserved.
 */

#include "ImageSource.hh"
#include "CameraImageSource.hh"

using namespace std;

USING_KARABO_NAMESPACES;

namespace karabo {

    KARABO_REGISTER_FOR_CONFIGURATION(BaseDevice, Device<>, ImageSource, CameraImageSource)


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