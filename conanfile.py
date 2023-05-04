from conan import ConanFile
from conan.tools.gnu import PkgConfigDeps
from conan.tools.meson import MesonToolchain, Meson


class MotionDetectionConan(ConanFile):
    settings = "os", "arch", "compiler", "build_type"

    def requirements(self):
        self.requires("drogon/1.8.3")
        self.requires("openssl/1.1.1t", force=True)

    def generate(self):
        tc = MesonToolchain(self)
        tc.generate()
        pc = PkgConfigDeps(self)
        pc.generate()

    def build(self):
        meson = Meson(self)
        meson.configure()
        meson.build()
