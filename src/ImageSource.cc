/*
 * Author: <parenti>
 *
 * Created on January, 2020, 05:11 PM
 *
 * Copyright (c) European XFEL GmbH Hamburg. All rights reserved.
 */

#include "ImageSource.hh"

using namespace std;

USING_KARABO_NAMESPACES;

namespace karabo {


    KARABO_REGISTER_FOR_CONFIGURATION(BaseDevice, Device<>, ImageSource)

    void ImageSource::expectedParameters(Schema& expected) {
    }


    ImageSource::ImageSource(const karabo::util::Hash& config) : Device<>(config) {
    }


    ImageSource::~ImageSource() {
    }


    void ImageSource::preReconfigure(karabo::util::Hash& incomingReconfiguration) {
    }


    void ImageSource::postReconfigure() {
    }
}
