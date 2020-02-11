/*
 * Author: <parenti>
 *
 * Created on February 11, 2020, 09:18 AM
 *
 * Copyright (c) European XFEL GmbH Hamburg. All rights reserved.
 */

#ifndef KARABO_CAMERAIMAGESOURCE_HH
#define KARABO_CAMERAIMAGESOURCE_HH

#include <karabo/karabo.hpp>

#include "ImageSource.hh"

/**
 * The main Karabo namespace
 */
namespace karabo {

    class CameraImageSource : public ImageSource {

    public:

        KARABO_CLASSINFO(CameraImageSource, "CameraImageSource", "2.7")

        static void expectedParameters(karabo::util::Schema& expected);

        CameraImageSource(const karabo::util::Hash& config);

        virtual ~CameraImageSource();

    private:
        
            // implement scene map protocol
            using SceneFunction = boost::function<std::string()>;
            using SceneMap = std::map<std::string, SceneFunction>;
            SceneMap m_scenes;
            void requestScene(const karabo::util::Hash& update);
            void registerScene(const SceneFunction& sceneFunction, const std::string& funcName);

            // the main scene
            std::string scene();
    };

    
    
}

#endif