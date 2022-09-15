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
        # TODO need to add args=["-g", "-O0"] to CXXFLAGS, compiler flags, whatever you want to call them
        # This is really what a profile is for I think... 
        # We can cat out the default profile build under jenkins, then use it as a template to create a jenkins profile
        # Add the flags there
        cmake.configure()
        cmake.build()

    def package_info(self):
        if not self.settings.os == "Windows":
            self.cpp_info.cxxflags.append("-g")
            self.cpp_info.cxxflags.append("-O0")

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib*", dst="bin", src="lib")
        self.copy('*.so*', dst='bin', src='lib')

    def test(self):
        if not tools.cross_building(self):
            os.chdir("bin")
            self.run(f"valgrind .{os.sep}test")
