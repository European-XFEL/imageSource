# Copyright (c) European XFEL GmbH Schenefeld. All rights reserved.

import unittest

import numpy as np
from PIL import Image


class Jpeg_TestCase(unittest.TestCase):
    def test_encode_jpeg(self):
        for fname in (("4.2.03.raw", "rgb_test.jpg"),  # RGB image
                      ("gray21.512.raw", "gray_test.jpg")):  # GRAY image

            in_file = f"src/test/{fname[0]}"  # RGB or GRAY image
            out_file = f"src/test/{fname[1]}"  # compressed image

            image2 = Image.open(out_file)
            data2 = np.array(image2)

            data1 = np.fromfile(in_file, dtype=np.uint8).reshape(data2.shape)

            diff = data2.astype(np.float) - data1.astype(np.float)

            # Verify that images are similar:
            # not more than 10% of the pixel can differ by more than 10 counts
            self.assertLess(np.sum(diff > 10) / data1.size, 0.10)

    def test_encode_rgb16(self):
        in_file = "src/test/rgb16_test.raw"  # RGB16 image
        out_file = "src/test/rgb16_test.jpg"  # compressed image

        image2 = Image.open(out_file)
        data2 = np.array(image2)

        data1 = np.fromfile(in_file, dtype=np.uint16).reshape(data2.shape)
        data1 = data1 / 256  # 16 -> 8 bpp

        diff = data2.astype(np.float) - data1.astype(np.float)

        # Verify that images are similar:
        # not more than 10% of the pixel can differ by more than 10 counts
        self.assertLess(np.sum(diff > 10) / data1.size, 0.10)


    def test_decode_jpeg(self):
        for fname in (("rgb_test.jpg", "rgb_test.raw"),  # RGB image
                      ("gray_test.jpg", "gray_test.raw")):  # GRAY image

            in_file = f"src/test/{fname[0]}"  # JPEG image
            out_file = f"src/test/{fname[1]}"  # uncompressed image

            image1 = Image.open(in_file)
            data1 = np.array(image1)

            data2 = np.fromfile(out_file, dtype=np.uint8).reshape(data1.shape)

            diff = data2.astype(np.float) - data1.astype(np.float)

            # Verify that images are similar:
            # not more than 10% of the pixel can differ by more than 10 counts
            self.assertLess(np.sum(diff > 10) / data1.size, 0.10)
