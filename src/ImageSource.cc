/*
 * Author: <parenti>
 *
 * Created on January 7, 2020, 05:11 PM
 *
 * Copyright (c) European XFEL GmbH Schenefeld. All rights reserved.
 */

#include <cstdio>
#include <future>

extern "C" {
#include <jpeglib.h>
}

#include <opencv2/core.hpp>

#include "ImageSource.hh"

using namespace std;

USING_KARABO_NAMESPACES;

namespace karabo {

    const std::vector<unsigned long long> initial_shape = {1, 1};
    const Encoding initial_encoding = Encoding::GRAY;
    const Types::ReferenceType initial_kType = Types::UINT16;

    KARABO_REGISTER_FOR_CONFIGURATION(Device, ImageSource)

    void ImageSource::expectedParameters(Schema& expected) {
        Schema data;

        NODE_ELEMENT(data).key("data").displayedName("Data").setDaqDataType(DaqDataType::TRAIN).commit();

        IMAGEDATA(data)
              .key("data.image")
              .displayedName("Image")
              // Set initial dummy values for DAQ
              .setDimensions(initial_shape)
              .setEncoding(initial_encoding)
              .setType(initial_kType)
              .commit();

        OUTPUT_CHANNEL(expected).key("output").displayedName("Output").dataSchema(data).commit();

        // Second output channel for the DAQ
        OUTPUT_CHANNEL(expected).key("daqOutput").displayedName("DAQ Output").dataSchema(data).commit();
    }

    ImageSource::ImageSource(const karabo::data::Hash& config)
        : Device(config) {}

    ImageSource::~ImageSource() {}

    void ImageSource::updateOutputSchema(const std::vector<unsigned long long>& shape, const Encoding& encoding,
                                         const Types::ReferenceType& kType) {
        boost::mutex::scoped_lock lock(m_updateSchemaMtx);

        const Schema& schema = this->getFullSchema();
        const std::vector<unsigned long long>& currentShape = schema.getDefaultValue<std::vector<unsigned long long>>("output.schema.data.image.dims");
        const int currentEncoding = schema.getDefaultValue<int>("output.schema.data.image.encoding");
        const int currentKType = schema.getDefaultValue<int>("output.schema.data.image.pixels.type");

        if (shape == currentShape && static_cast<const int>(encoding) == currentEncoding && kType == currentKType) {
            // Nothing to be updated
            KARABO_LOG_FRAMEWORK_DEBUG << "No need to update the output schema";
            return;
        }

        Schema schemaUpdate;
        this->schema_update_helper(schemaUpdate, "output", "Output", shape, encoding, kType);

        std::vector<unsigned long long> daqShape = shape;
        std::reverse(daqShape.begin(),
                     daqShape.end()); // NB DAQ wants fastest changing index first, e.g.
                                      // (width, height) or (channel, width, height)
        this->schema_update_helper(schemaUpdate, "daqOutput", "DAQ Output", daqShape, encoding, kType);

        this->appendSchema(schemaUpdate);
    }

    void ImageSource::schema_update_helper(Schema& schemaUpdate, const std::string& nodeKey,
                                           const std::string& displayedName,
                                           const std::vector<unsigned long long>& shape, const Encoding& encoding,
                                           const Types::ReferenceType& kType) {
        Schema dataSchema;
        NODE_ELEMENT(dataSchema).key("data").displayedName("Data").setDaqDataType(DaqDataType::TRAIN).commit();

        IMAGEDATA(dataSchema)
              .key("data.image")
              .displayedName("Image")
              .setDimensions(shape)
              .setType(kType)
              .setEncoding(encoding)
              .commit();

        OUTPUT_CHANNEL(schemaUpdate).key(nodeKey).displayedName(displayedName).dataSchema(dataSchema).commit();
    }

    std::future<void> ImageSource::startDataSending(const char* channelName, karabo::xms::ImageData& imageData,
                                                    const Timestamp& timestamp, bool safeNDArray) {
        const OutputChannel::Pointer channel = this->getOutputChannel(channelName);
        const xms::OutputChannel::MetaData meta{m_instanceId + ":" + channelName, timestamp};
        channel->write(Hash{"data.image", imageData}, meta);

        auto output_promise = std::make_shared<std::promise<void>>();
        auto output_future = output_promise->get_future();
        channel->asyncUpdate(safeNDArray, [output_promise]() { output_promise->set_value(); });

        return output_future;
    }

    void ImageSource::writeChannels(const NDArray& data, const Dims& binning, const unsigned short bpp,
                                    const Encoding& encoding, const Dims& roiOffsets,
                                    const karabo::data::Timestamp& timestamp, bool safeNDArray) {
        karabo::xms::ImageData imageData(data, encoding);
        imageData.setBitsPerPixel(bpp);
        imageData.setROIOffsets(roiOffsets);
        imageData.setBinning(binning);

        const std::future<void> output_future = this->startDataSending("output", imageData, timestamp, safeNDArray);

        // NB DAQ wants fastest changing index first, e.g. (width, height) or
        // (channel, width, height)
        Dims daqShape = data.getShape();
        daqShape.reverse();
        imageData.setDimensions(daqShape);
        const std::future<void> daq_future = this->startDataSending("daqOutput", imageData, timestamp, safeNDArray);

        output_future.wait();
        daq_future.wait();
    }

    void ImageSource::signalEOS() {
        this->signalEndOfStream("output");
        this->signalEndOfStream("daqOutput");
    }

    void util::unpackMono12Packed(const uint8_t* data, const uint32_t width, const uint32_t height,
                                  uint16_t* unpackedData) {
        size_t idx = 0, px = 0, image_size = width * height;
        while (px + 1 < image_size) {
            unpackedData[px] = (data[idx] << 4) | (data[idx + 1] & 0xF);
            unpackedData[px + 1] = (data[idx + 2] << 4) | (data[idx + 1] >> 4);
            idx += 3;
            px += 2;
        }
    }

    void util::unpackAnyFormat(const uint8_t* data, const size_t image_size, const uint8_t bpp,
                               uint16_t* unpackedData) {
        if (bpp < 9 || bpp > 15) {
            throw KARABO_PARAMETER_EXCEPTION("Invalid bpp value: " + std::to_string(bpp) + ". It must be in [9, 15].");
        }

        const uint16_t mask = 0xFFFF >> (16 - bpp);
        size_t bits = 0, px = 0;
        while (px < image_size) {
            const size_t idx = bits / 8;
            const size_t shift = bits % 8;
            unpackedData[px] = (*reinterpret_cast<const uint16_t*>(data + idx) >> shift) & mask;

            bits += bpp;
            px += 1;
        }
    }

    void util::unpackMono10p(const uint8_t* data, const uint32_t width, const uint32_t height, uint16_t* unpackedData) {
        util::unpackAnyFormat(data, width * height, 10, unpackedData);
    }

    void util::unpackMono12p(const uint8_t* data, const uint32_t width, const uint32_t height, uint16_t* unpackedData) {
        util::unpackAnyFormat(data, width * height, 12, unpackedData);
    }

    void util::unpackBayer10p(const uint8_t* data, const uint32_t width, const uint32_t height,
                              uint16_t* unpackedData) {
        util::unpackAnyFormat(data, width * height, 10, unpackedData);
    }

    void util::unpackBayer12p(const uint8_t* data, const uint32_t width, const uint32_t height,
                              uint16_t* unpackedData) {
        util::unpackAnyFormat(data, width * height, 12, unpackedData);
    }

    void util::decodeJPEG(karabo::xms::ImageData& imd) {
        // The array which we assign the decoded JPEG stream to
        NDArray ndarr(imd.getDimensions(), Types::UINT8);

        NDArray& arr = imd.getData();
        unsigned char* cdata = arr.getData<unsigned char>();
        struct jpeg_decompress_struct cinfo;
        struct jpeg_error_mgr jerr;

        const unsigned long jpg_size = arr.byteSize();

        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_decompress(&cinfo);

        // We can directly source the data, but need to const_cast here as cdata is
        // returned as const. This is not a problem though, as we create and assign a
        // new NDarray to the image data object in the process of conversion.
        jpeg_mem_src(&cinfo, cdata, jpg_size);

        // Test if this is a valid JPEG before we fail miserably
        const int rc = jpeg_read_header(&cinfo, TRUE);

        if (rc != 1) {
            throw KARABO_PARAMETER_EXCEPTION("Image advertised as JPEG, but does not seem to be JPEG data");
        }
        jpeg_start_decompress(&cinfo);
        const int width = cinfo.output_width;
        const int pixel_size = cinfo.output_components;

        // We directly read into the NDArray to avoid further copies
        unsigned char* bmp_buffer = ndarr.getData<unsigned char>();

        // The row_stride is the total number of bytes it takes to store an
        // entire scanline (row).
        const int row_stride = width * pixel_size;

        while (cinfo.output_scanline < cinfo.output_height) {
            unsigned char* buffer_array[1];
            buffer_array[0] = bmp_buffer + cinfo.output_scanline * row_stride;
            jpeg_read_scanlines(&cinfo, buffer_array, 1);
        }

        // Clean up
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);

        // Karabo-ize our data.
        // Any raw pointer referenced to previous NDarray data
        // of imd have been destroyed at this point.
        imd.setData(ndarr);
        imd.setEncoding(Encoding::GRAY);
    }

    void util::encodeJPEG(karabo::xms::ImageData& imd, unsigned int quality, const std::string& comment) {
        NDArray& arr = imd.getData();

        const Types::ReferenceType kType = arr.getType();
        std::shared_ptr<unsigned char[]> sdata;
        if (kType == Types::UINT8) {
            sdata = std::shared_ptr<unsigned char[]>(arr.getData<unsigned char>(), [](unsigned char* p) {});
        } else if (kType == Types::UINT16) {
            const size_t pixels = arr.size();
            sdata = std::shared_ptr<unsigned char[]>(new unsigned char[pixels]);
            const unsigned short* ldata = arr.getData<unsigned short>();

            if (arr.isBigEndian()) {
                for (size_t i = 0; i < pixels; ++i) {
                    sdata[i] = ldata[i] & 0xFF; // MSB is 0
                }
            } else {
                for (size_t i = 0; i < pixels; ++i) {
                    sdata[i] = ldata[i] >> 8; // MSB is 1
                }
            }
        } else {
            throw KARABO_PARAMETER_EXCEPTION("Conversion from Type " + toString(kType) + " is not implemented.");
        }

        struct jpeg_compress_struct cinfo;
        struct jpeg_error_mgr jerr;

        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_compress(&cinfo);

        const Encoding encoding = imd.getEncoding();
        switch (encoding) {
            case Encoding::GRAY:
                cinfo.input_components = 1;
                cinfo.in_color_space = JCS_GRAYSCALE;
                break;
            case Encoding::RGB:
                cinfo.input_components = 3;
                cinfo.in_color_space = JCS_RGB;
                break;
            default:
                throw KARABO_PARAMETER_EXCEPTION("Conversion from " + toString(static_cast<int>(encoding)) +
                                                 " to JPEG is not implemented.");
        }

        const Dims& dims = imd.getDimensions();
        cinfo.image_width = dims.x2();
        cinfo.image_height = dims.x1();

        jpeg_set_defaults(&cinfo);
        jpeg_set_quality(&cinfo, quality, TRUE);

        // Instruct libjpeg to encode to memory.
        // The library will allocate the memory.
        unsigned char* jpeg_buffer = nullptr;
        unsigned long jpeg_size;
        jpeg_mem_dest(&cinfo, &jpeg_buffer, &jpeg_size);

        jpeg_start_compress(&cinfo, TRUE);

        // Add comment section if any
        if (comment.size() > 0) {
            // The comment can be (0xFF-2) Bytes long. Truncate if longer.
            const size_t commentSize = std::min<size_t>(65533, comment.size());
            jpeg_write_marker(&cinfo, JPEG_COM, (const JOCTET*)comment.data(), commentSize);
        }

        // The row_stride is the total number of bytes it takes to store an
        // entire scanline (row).
        const int row_stride = cinfo.input_components * cinfo.image_width;

        while (cinfo.next_scanline < cinfo.image_height) {
            unsigned char* buffer_array[1];
            buffer_array[0] = sdata.get() + cinfo.next_scanline * row_stride;
            jpeg_write_scanlines(&cinfo, buffer_array, 1);
        }

        jpeg_finish_compress(&cinfo);
        jpeg_destroy_compress(&cinfo);

        // Karabo-ize our data.
        NDArray ndarr(jpeg_buffer, jpeg_size);
        imd.setData(ndarr);
        imd.setEncoding(Encoding::JPEG);
        imd.setDimensions(dims);
    }

    void util::rotateImage(karabo::xms::ImageData& imd, unsigned int angle, void* buffer) {
        if (!imd.isIndexable()) {
            throw KARABO_PARAMETER_EXCEPTION("Cannot rotate non-indexable image");
        }

        NDArray& arr = imd.getData();
        const size_t itemSize = arr.itemSize();

        switch (itemSize) {
            case 1:
                util::rotate_image<uint8_t>(arr, angle, buffer);
                break;
            case 2:
                util::rotate_image<uint16_t>(arr, angle, buffer);
                break;
            case 4:
                util::rotate_image<uint32_t>(arr, angle, buffer);
                break;
            // XXX Not supported by NDArray
            // case 8:
            //     util::rotate_image<uint64_t>(arr, angle, buffer);
            //     break;
            default:
                const Types::ReferenceType kType = arr.getType();
                throw KARABO_PARAMETER_EXCEPTION("Cannot rotate images of type " + std::to_string(kType));
        }

        if (angle == 90 || angle == 270) {
            std::vector<unsigned long long> vec;

            Dims roiOffsets = imd.getROIOffsets();
            vec = roiOffsets.toVector();
            // Swap x and y
            vec[0] = roiOffsets.x2();
            vec[1] = roiOffsets.x1();
            roiOffsets.fromVector(vec);
            imd.setROIOffsets(roiOffsets);

            Dims binning = imd.getBinning();
            vec = binning.toVector();
            // Swap x and y
            vec[0] = binning.x2();
            vec[1] = binning.x1();
            binning.fromVector(vec);
            imd.setBinning(binning);

            Dims dimensions = imd.getDimensions();
            vec = dimensions.toVector();
            // Swap x and y
            vec[0] = dimensions.x2();
            vec[1] = dimensions.x1();
            dimensions.fromVector(vec);
            imd.setDimensions(dimensions);

            const bool flipX = imd.getFlipX();
            const bool flipY = imd.getFlipY();
            imd.setFlipY(flipX);
            imd.setFlipX(flipY);
        }

        if (imd.getRotation() != Rotation::UNDEFINED) {
            int rotation = static_cast<int>(imd.getRotation());
            rotation = (rotation + angle) % 360;
            switch (rotation) {
                case 0:
                    imd.setRotation(Rotation::ROT_0);
                    break;
                case 90:
                    imd.setRotation(Rotation::ROT_90);
                    break;
                case 180:
                    imd.setRotation(Rotation::ROT_180);
                    break;
                case 270:
                    imd.setRotation(Rotation::ROT_270);
                    break;
                default:
                    imd.setRotation(Rotation::UNDEFINED);
            }
        }
    }

    template <class T>
    void util::rotate_image(karabo::data::NDArray& arr, unsigned int angle, void* buffer) {
        const Dims shape = arr.getShape();
        unsigned long long channels;
        if (shape.rank() == 2) {
            // Monochromatic image
            channels = 1;
        } else if (shape.rank() == 3) {
            channels = shape.x3();
            // Color images have 3 or 4 channels (e.g. RGB, YUV, RGBA)
            // XXX What about Bayer Formats?
            // (see https://en.wikipedia.org/wiki/Bayer_filter)
            if (channels != 3 && channels != 4) {
                throw KARABO_NOT_IMPLEMENTED_EXCEPTION("Can only flip monochromatic/color images");
            }
        } else {
            throw KARABO_NOT_IMPLEMENTED_EXCEPTION("Can only rotate monochromatic/color images");
        }

        int type;
        switch (sizeof(T)) {
            case 1:
                type = CV_8UC(channels);
                break;
            case 2:
                type = CV_16UC(channels);
                break;
            case 4:
                type = CV_32SC(channels);
                break;
            default:
                throw KARABO_NOT_IMPLEMENTED_EXCEPTION("CV Cannot handle data type of size " +
                                                       std::to_string(sizeof(T)));
        }

        const size_t width = shape.x2();
        const size_t height = shape.x1();
        const size_t size = shape.size();
        const size_t byteSize = arr.byteSize();

        int rotateCode;
        size_t width_out;
        size_t height_out;
        switch (angle) {
            case 0:
                // nothing to be done
                return;
                break;
            case 90:
                rotateCode = cv::ROTATE_90_CLOCKWISE;
                width_out = height;
                height_out = width;
                break;
            case 180:
                rotateCode = cv::ROTATE_180;
                width_out = width;
                height_out = height;
                break;
            case 270:
                rotateCode = cv::ROTATE_90_COUNTERCLOCKWISE;
                width_out = height;
                height_out = width;
                break;
            default:
                throw KARABO_PARAMETER_EXCEPTION("Invalid rotation angle: " + std::to_string(angle) +
                                                 ". It must be in {0, 90, 180, 270}.");
        }

        T* data = arr.getData<T>();
        T* data_copy;
        if (buffer == nullptr) {
            data_copy = new T[size];
        } else {
            data_copy = reinterpret_cast<T*>(buffer);
        }
        memcpy(data_copy, data, byteSize);

        cv::Mat in(height, width, type, (void*)data_copy);
        cv::Mat out(height_out, width_out, type, (void*)data);
        if (shape.rank() == 2) {
            cv::rotate(in, out, rotateCode);
            arr.setShape(Dims(height_out, width_out));
        } else { // shape.rank() == 3
            cv::Mat channels_in[channels];
            cv::Mat channels_out[channels];
            cv::split(in, channels_in);
            for (size_t i = 0; i < channels; ++i) {
                cv::rotate(channels_in[i], channels_out[i], rotateCode);
            }
            cv::merge(channels_out, channels, out);
            arr.setShape(Dims(height_out, width_out, shape.x3()));
        }

        if (buffer == nullptr) {
            delete[] data_copy;
        }
    }

    void util::flipImage(karabo::xms::ImageData& imd, bool flipX, bool flipY, void* buffer) {
        if (!imd.isIndexable()) {
            throw KARABO_PARAMETER_EXCEPTION("Cannot flip non-indexable image");
        }

        NDArray& arr = imd.getData();
        const size_t itemSize = arr.itemSize();

        switch (itemSize) {
            case 1:
                util::flip_image<uint8_t>(arr, flipX, flipY, buffer);
                break;
            case 2:
                util::flip_image<uint16_t>(arr, flipX, flipY, buffer);
                break;
            case 4:
                util::flip_image<uint32_t>(arr, flipX, flipY, buffer);
                break;
            // XXX Not supported by NDArray
            // case 8:
            //     util::flip_image<uint64_t>(arr, flipX, flipY, buffer);
            //     break;
            default:
                const Types::ReferenceType kType = arr.getType();
                throw KARABO_PARAMETER_EXCEPTION("Cannot flip images of type " + std::to_string(kType));
        }

        if (flipX) {
            imd.setFlipX(!imd.getFlipX());
        }

        if (flipY) {
            imd.setFlipY(!imd.getFlipY());
        }
    }

    template <class T>
    void util::flip_image(karabo::data::NDArray& arr, bool flipX, bool flipY, void* buffer) {
        const Dims shape = arr.getShape();
        unsigned long long channels;
        if (shape.rank() == 2) {
            // Monochromatic image
            channels = 1;
        } else if (shape.rank() == 3) {
            channels = shape.x3();
            // Color images have 3 or 4 channels (e.g. RGB, YUV, RGBA)
            // XXX What about Bayer Formats?
            // (see https://en.wikipedia.org/wiki/Bayer_filter)
            if (channels != 3 && channels != 4) {
                throw KARABO_NOT_IMPLEMENTED_EXCEPTION("Can only flip monochromatic/color images");
            }
        } else {
            throw KARABO_NOT_IMPLEMENTED_EXCEPTION("Can only flip monochromatic/color images");
        }

        int type;
        switch (sizeof(T)) {
            case 1:
                type = CV_8UC(channels);
                break;
            case 2:
                type = CV_16UC(channels);
                break;
            case 4:
                type = CV_32SC(channels);
                break;
            default:
                throw KARABO_NOT_IMPLEMENTED_EXCEPTION("CV Cannot handle data type of size " +
                                                       std::to_string(sizeof(T)));
        }

        int flipCode;
        if (flipX && flipY) {
            flipCode = -1;
        } else if (flipX) {
            // Swap columns, i.e. flip around the the y-axis
            flipCode = 1;
        } else if (flipY) {
            // Swap rows, i.e. flip around the x-axis
            flipCode = 0;
        } else {
            // Nothing to be done
            return;
        }

        T* data = arr.getData<T>();
        const size_t width = shape.x2();
        const size_t height = shape.x1();
        const size_t size = shape.size();
        const size_t byteSize = arr.byteSize();

        T* data_copy;
        if (buffer == nullptr) {
            data_copy = new T[size];
        } else {
            data_copy = reinterpret_cast<T*>(buffer);
        }
        memcpy(data_copy, data, byteSize);

        cv::Mat in(height, width, type, (void*)data_copy);
        cv::Mat out(height, width, type, (void*)data);
        if (shape.rank() == 2) {
            cv::flip(in, out, flipCode);
        } else { // shape.rank() == 3
            cv::Mat channels_in[channels];
            cv::Mat channels_out[channels];
            cv::split(in, channels_in);
            for (size_t i = 0; i < channels; ++i) {
                cv::flip(channels_in[i], channels_out[i], flipCode);
            }
            cv::merge(channels_out, channels, out);
        }

        if (buffer == nullptr) {
            delete[] data_copy;
        }
    }

} // namespace karabo
