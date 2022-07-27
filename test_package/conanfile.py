import os

from conans import ConanFile, CMake, tools


class McCoolDSPTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake", "CMakeDeps", "CMakeToolchain"
    requires = "catch2/3.1.0"

    def build(self):
        cmake = CMake(self)
        # Current dir is "test_package/build/<build_id>" and CMakeLists.txt is
        # in "test_package"
        cmake.configure()
        cmake.build()

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib*", dst="bin", src="lib")
        self.copy('*.so*', dst='bin', src='lib')

    def test(self):
        if not tools.cross_building(self):
            os.chdir("bin")
            print("Hello!")
            self.run(f".{os.sep}test")
