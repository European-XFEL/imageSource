from conans import ConanFile
from conans.tools import collect_libs
from conan.tools.cmake import CMake, CMakeToolchain
from conan.tools.layout import cmake_layout


class ImageSourceConan(ConanFile):
    generators = "CMakeToolchain", "CMakeDeps"
    settings = "os", "compiler", "build_type", "arch"

    name = "imageSource"
    requires = ( "karaboFramework/2.16.4@karaboDevices+depends/any",
                 "opencv/4.5.5@karaboDevices+depends/any",
    )
    options = { "build_tests": [True, False] }
    default_options = { "build_tests": False }

    def generate(self):
        toolchain = CMakeToolchain(self)
        toolchain.variables["BUILD_TESTS"] = eval(self.options.build_tests._value)
        toolchain.variables["Boost_NO_BOOST_CMAKE"] = True
        toolchain.variables["Boost_NO_SYSTEM_PATHS"] = True
        toolchain.generate()

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.libdirs = ["plugins"]
        self.cpp_info.libs = collect_libs(self)

    def deploy(self):
        self.copy("*", dst=self.install_folder)
        self.copy_deps("*", dst=self.install_folder)
