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
#include "version.hh"  // provides IMAGESOURCE_PACKAGE_VERSION

/**
 * The main Karabo namespace
 */
namespace karabo {

    class ImageSource : public karabo::core::Device<> {

    public:

        KARABO_CLASSINFO(ImageSource, "ImageSource", IMAGESOURCE_PACKAGE_VERSION)

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
         * @verbatim embed:rst:leading-asterisk
         *
         * +------+---------------------+
         * | Byte |  Pixel - Data bits  |
         * +======+=====================+
         * |  B0  |      P0 11...4      |
         * +------+---------------------+
         * |  B1  | P1 3...0 | P0 3...0 |
         * +------+---------------------+
         * |  B2  |      P1 11...4      |
         * +------+---------------------+
         * |  ... |         ...         |
         * +------+---------------------+
         * |  Bm  |      Pn 11...4      |
         * +------+---------------------+
         *
         * @endverbatim
         * 
         * @param data The pointer to the input packed data
         * @param width The image width
         * @param height The image height
         * @param unpackedData The pointer to the output unpacked data
         */
        void unpackMono12Packed(const uint8_t* data, const uint32_t width, const uint32_t height, uint16_t* unpackedData);

        /**
         * @brief Unpack the input MonoXXp data to MONO12, where XX is usually
         * 10 or 12.
         * 
         * In MonoXXp pixel data format, XX-bit pixel data are packed, with no
         * padding bits in between. Padding 0s are added to the MSB if needed.
         * For example Mono10p pixels are packed this way:
         *
         * @verbatim embed:rst:leading-asterisk
         *
         * +------+---------------------+
         * | Byte |  Pixel - Data bits  |
         * +======+=====================+
         * |  B0  |      P0 7...0       |
         * +------+---------------------+
         * |  B1  | P1 5...0 | P0 9...8 |
         * +------+---------------------+
         * |  B2  | P2 3...0 | P1 9...6 |
         * +------+---------------------+
         * |  B3  | P4 1...0 | P3 9...4 |
         * +------+---------------------+
         * |  ... |         ...         |
         * +------+---------------------+
         *
         * @endverbatim
         * 
         * @param data The pointer to the input packed data
         * @param width The image width
         * @param height The image height
         * @param bpp The bits-per-pixel, normally 10 or 12
         * @param unpackedData The pointer to the output unpacked data
         */
        void unpackMonoXXp(const uint8_t* data, const uint32_t width, const uint32_t height, const uint8_t bpp, uint16_t* unpackedData);

        // Specialize unpackMonoXXp for XX = 10, 12
        void unpackMono10p(const uint8_t* data, const uint32_t width, const uint32_t height, uint16_t* unpackedData);
        void unpackMono12p(const uint8_t* data, const uint32_t width, const uint32_t height, uint16_t* unpackedData);

    }
}

#endif
