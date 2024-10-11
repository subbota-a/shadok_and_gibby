# This file is managed by Conan, contents will be overwritten.
# To keep your changes, remove these comment lines, but the plugin won't be able to modify your requirements

from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeToolchain
from conan.tools.files import copy

class ConanApplication(ConanFile):
    package_type = "application"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.user_presets_path = False
        tc.generate()
        for dep in self.dependencies.values():
            if (len(dep.cpp_info.bindirs)>0):
                copy(self, "*.dll", dep.cpp_info.bindirs[0], self.build_folder)

    def requirements(self):
        requirements = self.conan_data.get('requirements', [])
        for requirement in requirements:
            self.requires(requirement)
        if self.settings.os == "Windows":
            self.requires('sdl_ttf/2.22.0', options={'shared':'True'})
            self.requires("sdl/2.28.3", options={'shared':'True'})
            self.requires("sdl_image/2.6.3", options={'shared':'True'})
