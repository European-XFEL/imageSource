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
#include "version.hh" // provides IMAGESOURCE_PACKAGE_VERSION

/**
 * The main Karabo namespace
 */
namespace karabo {

    class CameraImageSource : public ImageSource {

    public:
        KARABO_CLASSINFO(CameraImageSource, "CameraImageSource", IMAGESOURCE_PACKAGE_VERSION)

        static void expectedParameters(karabo::util::Schema& expected);

        explicit CameraImageSource(const karabo::util::Hash& config);

        virtual ~CameraImageSource();

    private:
        // implement scene map protocol
        using SceneFunction = boost::function<std::string()>;
        using SceneMap = std::map<std::string, SceneFunction>;
        SceneMap m_scenes;

    protected:
        void requestScene(const karabo::util::Hash& params);
        void registerScene(const SceneFunction& sceneFunction, const std::string& funcName);
        // the main scene
        std::string scene();
    };


} // namespace karabo

#endif

