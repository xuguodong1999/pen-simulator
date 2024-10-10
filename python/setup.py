import os
import subprocess
import sys
from pathlib import Path

from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext


class CMakeExtension(Extension):
    def __init__(self, name: str, sourcedir: str = '') -> None:
        super().__init__(name, sources=[])
        self.sourcedir = os.fspath(Path(sourcedir).resolve())


class CMakeBuild(build_ext):
    def build_extension(self, ext: CMakeExtension) -> None:
        ext_fullpath = Path.cwd() / self.get_ext_fullpath(ext.name)
        extdir = ext_fullpath.parent.resolve()

        debug = int(os.environ.get('DEBUG', 0)) if self.debug is None else self.debug
        cfg = 'Debug' if debug else 'Release'

        cmake_args = [
            f'-G Ninja',
            f'-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={extdir}{os.sep}{ext.name}{os.sep}',
            f'-DPYTHON_EXECUTABLE={sys.executable}',
            f'-DCMAKE_BUILD_TYPE={cfg}',  # not used on MSVC, but no harm

            f'-DBUILD_SHARED_LIBS=OFF',
            f'-DXGD_USE_CUDA=OFF',
            f'-DXGD_USE_QT=OFF',
            f'-DXGD_USE_VK=OFF',
            f'-DXGD_USE_TORCH=OFF',
            f'-DXGD_USE_FONTCONFIG=OFF',
            f'-DXGD_FLAG_IPO=OFF',
        ]
        if sys.platform.find('win32') != -1:
            cmake_args.append(f'-DCMAKE_MSVC_RUNTIME_LIBRARY:STRING=MultiThreaded')
        elif sys.platform.find('darwin') != -1:
            cmake_args.append(f'-DXGD_USE_OPENMP:BOOL=OFF')

        build_args = [
            '--parallel',
            '--target', 'pen_simulator',
        ]

        build_temp = Path(self.build_temp) / ext.name
        if not build_temp.exists():
            build_temp.mkdir(parents=True)

        subprocess.run(
            ['cmake', ext.sourcedir, *cmake_args], cwd=build_temp, check=True
        )
        subprocess.run(
            ['cmake', '--build', '.', *build_args], cwd=build_temp, check=True
        )


setup(
    name='pen_simulator',
    version='0.0.3',
    description='pen simulator: data synthesis for simulation of pen-based interaction',
    author='xuguodong1999',
    url='https://github.com/xuguodong1999/pen-simulator',
    license='GPLv3',
    packages=['pen_simulator'],
    include_package_data=True,
    package_data={'pen_simulator': [
        '__init__.pyi',
        '__init__.py',
        'py.typed',
    ]},
    install_requires=[
        'numpy',
    ],
    ext_modules=[CMakeExtension('pen_simulator', '..')],
    cmdclass={'build_ext': CMakeBuild},
    zip_safe=False,
    python_requires='>=3.11',
    classifiers=[
        'Development Status :: 2 - Pre-Alpha',
    ],
)
