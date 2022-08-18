from conans import ConanFile, CMake


class McCoolDSPConan(ConanFile):
    name = "McCoolDSP"
    version = "0.1.1"
    license = "MIT"
    author = "Brendon McCool brendonmccool@gmail.com"
    url = "github.com/bmccool/McCoolDSP/"
    description = "C++ collection of DSP tools intended for use on embedded systems"
    topics = ("C++", "DSP", "HPS")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    generators = "cmake", "CMakeDeps", "CMakeToolchain"
    requires = "LomontFFT/0.1@mccool/release"
    exports_sources = "src/*"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder="src")
        cmake.build()

        # Explicit way:
        # self.run('cmake %s/hello %s'
        #          % (self.source_folder, cmake.command_line))
        # self.run("cmake --build . %s" % cmake.build_config)

    def package(self):
        self.copy("*.h", dst="include", src="src")
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.dylib*", dst="lib", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["McCoolDSP"]
