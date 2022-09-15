import os

from conans import ConanFile, CMake, tools


class McCoolDSPTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake", "CMakeDeps", "CMakeToolchain"
    requires = "catch2/3.1.0"

    def build(self):
        print("Building McCoolDSPTestConan!")
        cmake = CMake(self)
        # Current dir is "test_package/build/<build_id>" and CMakeLists.txt is
        # in "test_package"
        # TODO need to add args=["-g", "-O0"] to CXXFLAGS, compiler flags, whatever you want to call them
        # This is really what a profile is for I think... 
        # We can cat out the default profile build under jenkins, then use it as a template to create a jenkins profile
        # Add the flags there
        cmake.configure()
        cmake.build()

    #def configure(self):
    #    print("Configure for McCoolDSPTestConan!")
    #    self.settings.compiler

    # packge info is actually for CONSUMERS, not used in production of the package...
    #def package_info(self):
    #    print("Package Info for McCoolDSPTestConan!")
    #    if not self.settings.os == "Windows":
    #        self.cpp_info.cxxflags.append("-g")
    #        self.cpp_info.cxxflags.append("-O0")
    #    print(f"CXXFLAGS={self.cpp_info.cxxflags}")

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib*", dst="bin", src="lib")
        self.copy('*.so*', dst='bin', src='lib')

    def test(self):
        if not tools.cross_building(self):
            os.chdir("bin")
            if self.settings.os == "Windows":
                self.run(f".{os.sep}test")
            else:
                self.run(f"valgrind .{os.sep}test")
