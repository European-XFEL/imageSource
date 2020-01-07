/*
 * Author: <parenti>
 *
 * Created on January  7, 2020, 05:11 PM
 *
 * Copyright (c) European XFEL GmbH Hamburg. All rights reserved.
 */

#ifndef KARABO_IMAGESOURCE_HH
#define KARABO_IMAGESOURCE_HH

#include <karabo/karabo.hpp>

/**
 * The main Karabo namespace
 */
namespace karabo {

    class ImageSource : public karabo::core::Device<> {

    public:

        KARABO_CLASSINFO(ImageSource, "ImageSource", "2.7")

        static void expectedParameters(karabo::util::Schema& expected);

        ImageSource(const karabo::util::Hash& config);

        virtual ~ImageSource();

        virtual void preReconfigure(karabo::util::Hash& incomingReconfiguration);

    protected:

        void updateOutputSchema(const std::vector<int>& shape,
                const karabo::xms::EncodingType& encoding,
                const karabo::util::Types::ReferenceType& kType);

        void writeChannels(karabo::util::NDArray& data, const karabo::util::Dims& binning,
                const unsigned short bpp, const karabo::xms::EncodingType& encoding,
                const karabo::util::Dims& roiOffsets, const karabo::util::Timestamp& timestamp);

    private:

        void schema_update_helper(karabo::util::Schema& schemaUpdate,
                const std::string& nodeKey, const std::string& displayedName,
                const std::vector<int>& shape, const karabo::xms::EncodingType& encoding,
                const karabo::util::Types::ReferenceType& kType);

        void write_channel(const std::string& nodeKey, karabo::util::NDArray& data, const karabo::util::Dims& binning,
                const unsigned short bpp, const karabo::xms::EncodingType& encoding,
                const karabo::util::Dims& roiOffsets, const karabo::util::Timestamp& timestamp);

        // TODO implement scene map protocol
    };
}

#endif
