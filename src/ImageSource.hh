/*
 * Author: <parenti>
 *
 * Created on January 7, 2020, 05:11 PM
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

    protected:

        /**
         * @brief Update the device output schema according to the image properties.
         * 
         * @param shape The shape of the image, e.g. (height, width) for monochromatic- or (height, width, channel) for RGB-images .
         * @param encoding The encoding of the image, e.g. Encoding::GRAY or Encoding::RGB.
         * @param kType The image data type, e.g. Types::UINT16.
         */
        void updateOutputSchema(const std::vector<int>& shape,
                const karabo::xms::EncodingType& encoding,
                const karabo::util::Types::ReferenceType& kType);

        /**
         * @brief Write the image and its metadata to the output channels.
         * 
         * @param data The image data.
         * @param binning The image binning, e.g. (binY, binX).
         * @param bpp The pixel depth (bits-per-pixel).
         * @param encoding The image encoding, e.g. Encoding::GRAY.
         * @param roiOffsets The offset of the Region-of-Interest, e.g. (roiY, roiX).
         * @param timestamp The image timestamp.
         * @parame header Any additional information to be written in the image header.
         */
        void writeChannels(karabo::util::NDArray& data, const karabo::util::Dims& binning,
                const unsigned short bpp, const karabo::xms::EncodingType& encoding,
                const karabo::util::Dims& roiOffsets, const karabo::util::Timestamp& timestamp,
                const karabo::util::Hash& header);

    private:

        void schema_update_helper(karabo::util::Schema& schemaUpdate,
                const std::string& nodeKey, const std::string& displayedName,
                const std::vector<int>& shape, const karabo::xms::EncodingType& encoding,
                const karabo::util::Types::ReferenceType& kType);

    };

}

#endif
