from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
from distutils.ccompiler import new_compiler
from glob import glob


class BuildExt(build_ext):
    """A custom build extension for adding compiler-specific options."""

    def build_extensions(self):
        for ext in self.extensions:
            ext.extra_compile_args = [
                "-Wno-unused-variable",
                "-Wno-missing-braces",
                "-Wno-strict-prototypes",
                "-Wno-sign-compare",
                "-Wno-comment",
            ]
        build_ext.build_extensions(self)


ext_modules = [
    Extension(
        "vplanet.vplanet_core",
        glob("src/*.c"),
        include_dirs=["src"],
        language="c",
        define_macros=[("VPLANET_PYTHON_INTERFACE", 1)],
    )
]
cmdclass = {"build_ext": BuildExt}


setup(
    name="vplanet",
    author="Rory Barnes",
    author_email="rkb9@uw.edu",
    url="https://github.com/VirtualPlanetaryLaboratory/vplanet",
    description="The virtual planet simulator",
    license="MIT",
    packages=find_packages(),
    ext_modules=ext_modules,
    cmdclass=cmdclass,
    include_package_data=True,
    zip_safe=False,
    entry_points={"console_scripts": ["vplanet=vplanet:entry_point"]},
)
