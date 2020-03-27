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
        void updateOutputSchema(const std::vector<unsigned long long>& shape,
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
         * @param header Any additional information to be written in the image header.
         */
        void writeChannels(const karabo::util::NDArray& data, const karabo::util::Dims& binning,
                const unsigned short bpp, const karabo::xms::EncodingType& encoding,
                const karabo::util::Dims& roiOffsets, const karabo::util::Timestamp& timestamp,
                const karabo::util::Hash& header);


        /**
         * @brief Send an end-of-stream signal to 'output' and 'daqOutput' channels
         * 
         */
        void signalEOS();

    private:

        void schema_update_helper(karabo::util::Schema& schemaUpdate,
                const std::string& nodeKey, const std::string& displayedName,
                const std::vector<unsigned long long>& shape, const karabo::xms::EncodingType& encoding,
                const karabo::util::Types::ReferenceType& kType);

    };

    namespace util {
        /**
         * @brief Unpack the input MONO12PACKED data to MONO12.
         * 
         * In MONO12PACKED pixel data format, every 3 bytes contain data for 2 pixels,
         * according to the following table:
         * 
         * +------+---------------------+
         * | Byte |  Pixel - Data bits  |
         * +------+---------------------+
         * |  B0  |      P0 11...4      |
         * |  B1  | P1 3...0 | P0 3...0 |
         * |  B2  |      P1 11...4      |
         * |  ... |         ...         |
         * |  Bm  |      Pn 11...4      |
         * +------+---------------------+
         * 
         * @param data The pointer to the input packed data
         * @param width The image width
         * @param heigth The image height
         * @param unpackedData The pointer to the output unpacked data
         */
        void unpackMono12Packed(const uint8_t* data, const uint32_t width, const uint32_t height, uint16_t* unpackedData);
    }
}

#endif
