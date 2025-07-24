/*
 * Author: <parenti>
 *
 * Created on January 7, 2020, 05:11 PM
 *
 * Copyright (c) European XFEL GmbH Schenefeld. All rights reserved.
 */

#ifndef KARABO_IMAGESOURCE_HH
#define KARABO_IMAGESOURCE_HH

#include <karabo/karabo.hpp>

#include <boost/thread/mutex.hpp>

#include "version.hh" // provides IMAGESOURCE_PACKAGE_VERSION

/**
 * The main Karabo namespace
 */
namespace karabo {

    class ImageSource : public karabo::core::Device {
       public:
        KARABO_CLASSINFO(ImageSource, "ImageSource", IMAGESOURCE_PACKAGE_VERSION)

        static void expectedParameters(karabo::data::Schema& expected);

        explicit ImageSource(const karabo::data::Hash& config);

        virtual ~ImageSource();

       protected:
        /**
         * @brief Update the device output schema according to the image properties.
         *
         * @param shape The shape of the image, e.g. (height, width) for
         * monochromatic- or (height, width, channel) for RGB-images .
         * @param encoding The encoding of the image, e.g. Encoding::GRAY or
         * Encoding::RGB.
         * @param kType The image data type, e.g. Types::UINT16.
         */
        void updateOutputSchema(const std::vector<unsigned long long>& shape, const karabo::xms::Encoding& encoding,
                                const karabo::data::Types::ReferenceType& kType);

        /**
         * @brief Write the image and its metadata to the output channels.
         *
         * @param data The image data.
         * @param binning The image binning, e.g. (binY, binX).
         * @param bpp The pixel depth (bits-per-pixel).
         * @param encoding The image encoding, e.g. Encoding::GRAY.
         * @param roiOffsets The offset of the Region-of-Interest, e.g. (roiY, roiX).
         * @param timestamp The image timestamp.
         * @param safeNDArray Boolean that should be set to 'true' if 'data' is not changed after this 'writeChannel'.    
         *                    Otherwise, data will be copied if needed, i.e. when the output channel has to queue or
         *                    serves inner-process receivers.
         */
        void writeChannels(const karabo::data::NDArray& data, const karabo::data::Dims& binning,
                           const unsigned short bpp, const karabo::xms::Encoding& encoding,
                           const karabo::data::Dims& roiOffsets, const karabo::data::Timestamp& timestamp,
                           bool safeNDArray = false);

        /**
         * @brief Send an end-of-stream signal to 'output' and 'daqOutput' channels
         *
         */
        void signalEOS();

       private:
        boost::mutex m_updateSchemaMtx; // Protect from concurrent updateOutputSchema calls

        void schema_update_helper(karabo::data::Schema& schemaUpdate, const std::string& nodeKey,
                                  const std::string& displayedName, const std::vector<unsigned long long>& shape,
                                  const karabo::xms::Encoding& encoding,
                                  const karabo::data::Types::ReferenceType& kType);

        std::future<void> startDataSending(const char* channelName, karabo::xms::ImageData& imageData,
                                           const karabo::data::Timestamp& timestamp, bool safeNDArray);
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
        void unpackMono12Packed(const uint8_t* data, const uint32_t width, const uint32_t height,
                                uint16_t* unpackedData);

        /**
         * @brief Unpack the input image data by adding back the removed
         * padding bits. An example are Mono10p and Mono12p formats, that are
         * unpacked to MONO12. Another one are BayerRG10p and BayerRG12p,
         * unpacked to BayerRG12.
         *
         * In the Mono10p pixel data format - for example - the 10-bit-pixel
         * data are packed as following. Padding 0s are added to the MSB if
         * needed.
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
         * @param image_size The image size (width * height)
         * @param bpp The bits-per-pixel, normally 10 or 12
         * @param unpackedData The pointer to the output unpacked data
         */
        void unpackAnyFormat(const uint8_t* data, const size_t image_size, const uint8_t bpp, uint16_t* unpackedData);

        // Specialize unpackAnyFormat Mono10p and Mono12p
        void unpackMono10p(const uint8_t* data, const uint32_t width, const uint32_t height, uint16_t* unpackedData);
        void unpackMono12p(const uint8_t* data, const uint32_t width, const uint32_t height, uint16_t* unpackedData);

        /**
         * @brief Unpack the input BayerRG10p (BayerGR10p) data to BayerRG10 (BayerGR10).
         *
         * Since in BayerRG10p pixel data format there is no padding, 1.25 bytes/pixel are used,
         * according to the following table:
         *
         * @verbatim embed:rst:leading-asterisk
         *
         * +------+---------------------+
         * | Byte |  Pixel - Data bits  |
         * +======+=====================+
         * |  B0  |       R0 7...0      |
         * +------+---------------------+
         * |  B1  | G0 5...0 |  R0 9-8  |
         * +------+---------------------+
         * |  B2  | R1 3...0 | G0 9...6 |
         * +------+---------------------+
         * |  B3  |  G1 1-0  | R1 9...4 |
         * +------+---------------------+
         * |  B4  |       G1 9...2      |
         * +------+---------------------+
         * |  ... |         ...         |
         * +------+---------------------+
         * |  ... |  Bn 1-0  | Gn 9...4 |
         * +------+---------------------+
         * |  Bm  |       Bl 9...2      |
         * +------+---------------------+
         *
         * @endverbatim
         *
         * BayerGR10p has pretty much the same format, just color channels are swapped.
         *
         * @param data The pointer to the input packed data
         * @param width The image width
         * @param height The image height
         * @param unpackedData The pointer to the output unpacked data
         */
        void unpackBayer10p(const uint8_t* data, const uint32_t width, const uint32_t height, uint16_t* unpackedData);

        /**
         * @brief Unpack the input BayerRG12p (BayerGR12p) data to BayerRG12 (BayerRG12).
         *
         * Since in BayerRG12p pixel data format there is no padding, 1.5 bytes/pixel are used,
         * according to the following table:
         *
         * @verbatim embed:rst:leading-asterisk
         *
         * +------+----------------------+
         * | Byte |   Pixel - Data bits  |
         * +======+======================+
         * |  B0  |       R0 7...0       |
         * +------+----------------------+
         * |  B1  | G0 3...0 | R0 11...8 |
         * +------+----------------------+
         * |  B2  |       G0 11...4      |
         * +------+----------------------+
         * |  ... |          ...         |
         * +------+----------------------+
         * |  ... | Bn 3...0 | Gn 11...8 |
         * +------+----------------------+
         * |  Bm  |       Bl 11...4      |
         * +------+----------------------+
         *
         * @endverbatim
         *
         * BayerGR12p has pretty much the same format, just color channels are swapped.
         *
         * @param data The pointer to the input packed data
         * @param width The image width
         * @param height The image height
         * @param unpackedData The pointer to the output unpacked data
         */
        void unpackBayer12p(const uint8_t* data, const uint32_t width, const uint32_t height, uint16_t* unpackedData);

        /**
         * @brief Decode a JPEG image to GRAY.
         *
         * @param imd The ImageData object - encoded as JPEG - to be decoded as GRAY
         */
        void decodeJPEG(karabo::xms::ImageData& imd);

        /**
         * @brief Encode a GRAY image to JPEG.
         *
         * @param imd The ImageData object - encoded as JPEG - to be decoded as GRAY
         * @param quality The compression quality. Levels of 90% or higher are
         * considered "high quality", 80-90% is "medium quality", 70-80% is "low
         * quality".
         * @param comment An optional comment to be added to the JPEG image. Its maximum
         * length is 65533 Bytes.
         */
        void encodeJPEG(karabo::xms::ImageData& imd, unsigned int quality = 100, const std::string& comment = "");

        /**
         * @brief Rotate an image by 90, 180 or 270 degrees.
         *
         * @param imd The ImageData object - to be rotated.
         * @param angle The rotation angle. Allowed values are: 0, 90, 180, 270 degrees.
         * @param buffer An optional buffer, to be used for image rotation. Its size
         * must be at least (width * height * bytesPerPixel). If a null pointer is
         * passed, than the buffer is allocated internally in the function.
         *
         */
        void rotateImage(karabo::xms::ImageData& imd, unsigned int angle, void* buffer = nullptr);

        /**
         * @brief Rotate an image by 90, 180 or 270 degrees.
         *
         * @param T The pixel data type, e.g. uint16_t.
         * @param arr The NDArray object - to be rotated.
         * @param angle The rotation angle. Allowed values are: 0, 90, 180, 270 degrees.
         * @param buffer An optional buffer, to be used for image rotation. Its size
         * must be at least (width * height * bytesPerPixel). If a null pointer is
         * passed, than the buffer is allocated internally in the function.
         *
         */
        template <class T>
        void rotate_image(karabo::data::NDArray& arr, unsigned int angle, void* buffer = nullptr);

        /**
         * @brief Flip an image in X and/or Y.
         *
         * @param imd The ImageData object - to be rotated.
         * @param flipX If this is true, the image will be flipped in the horizontal
         * direction.
         * @param flipY If this is true, the image will be flipped in the vertical
         * direction.
         * @param buffer An optional buffer, to be used for image flip. Its size must be
         * at least (width * height * bytesPerPixel). If a null pointer is passed, than
         * the buffer is allocated internally in the function.
         *
         */
        void flipImage(karabo::xms::ImageData& imd, bool flipX, bool flipY, void* buffer = nullptr);

        /**
         * @brief Flip an image in X and/or Y.
         *
         * @param T The pixel data type, e.g. uint16_t.
         * @param arr The NDArray object - to be rotated.
         * @param flipX If this is true, the image will be flipped in the horizontal
         * direction.
         * @param flipY If this is true, the image will be flipped in the vertical
         * direction.
         * @param buffer An optional buffer, to be used for image flip. Its size must be
         * at least (width * height * bytesPerPixel). If a null pointer is passed, than
         * the buffer is allocated internally in the function.
         *
         */
        template <class T>
        void flip_image(karabo::data::NDArray& arr, bool flipX, bool flipY, void* buffer = nullptr);

    } // namespace util
} // namespace karabo

#endif
