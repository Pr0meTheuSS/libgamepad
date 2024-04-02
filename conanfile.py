from conans import ConanFile, CMake, tools

class Gamepad(ConanFile):
    name = "gamepad"
    version = "1.0.0"
    license = "MIT"
    author = "Your Name"
    url = "https://github.com/univrsal/libgamepad"
    description = "Описание вашей библиотеки"

    options = {"shared": [True, False]}
    default_options = {"shared": False}

    requires = []

    def source(self):
        print("source")
        git = tools.Git(self.source_folder)
        git.clone("https://github.com/univrsal/libgamepad")
    
    def build(self):
        print("build")
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        print("package")
        self.copy("*.hpp", dst="include")
        self.copy("*.h",   dst="include")
        self.copy("*.a", dst="lib")

    
    def package_info(self):
        print("package_info")
        self.cpp_info.libs = ["gamepad"]
