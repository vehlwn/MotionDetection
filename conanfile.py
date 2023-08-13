from conan import ConanFile
from conan.tools.gnu import PkgConfigDeps
from conan.tools.meson import MesonToolchain, Meson


class MotionDetectionConan(ConanFile):
    settings = "os", "arch", "compiler", "build_type"

    def requirements(self):
        self.requires("drogon/1.8.4")
        self.requires("boost/1.82.0")

    def generate(self):
        tc = MesonToolchain(self)
        tc.project_options["use_conan_boost"] = "true"
        tc.generate()
        pc = PkgConfigDeps(self)
        pc.generate()

    def build(self):
        meson = Meson(self)
        meson.configure()
        meson.build()
