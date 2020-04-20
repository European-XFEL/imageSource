.. _image_source:

*********************
The ImageSource class
*********************

The `ImageSource` class provides a base class to be used for all kind of
image providing devices, for example image processors.

For the cameras a more specific class is provided, see :ref:`here
<camera_image_source>`.

The advantage of using this class, is that it can take care of creating the
necessary output channels in the schema, and it provides functions to output
images and End-of-Stream signals to them.

.. doxygenclass:: karabo::ImageSource
   :project: ImageSource
   :members:
   :protected-members:
   :outline:
