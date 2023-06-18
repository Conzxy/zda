from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import get
from conan.tools.scm import Git
import os

class ZdaRecipe(ConanFile):
    name = "zda"
    version = "1.0.0"

    # Optional metadata
    license = "MIT"
    author = "Conzxy zengxiaoyu0@gmail.com"
    url = "https://github.com/Conzxy/zda"
    description = "Practical toolbox of data structures and algorithms"
    topics = ("bidirectional-linked-list", "algorithms", "data-structures")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    def source(self):
        git = Git(self)
        git.clone(url=self.url, target='.')

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        self.folders.source = '.'
        self.folders.build = os.path.join("build", str(self.settings.build_type))
        self.folders.generators = os.path.join(self.folders.build, "generators")

        self.cpp.source.includedirs = ['.']
        self.cpp.build.libdirs = ['.']
        self.cpp.build.libs = ["zda"]

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["zda"]
