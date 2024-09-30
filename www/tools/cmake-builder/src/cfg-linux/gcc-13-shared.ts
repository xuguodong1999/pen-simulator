import * as os from 'os';
import * as path from 'path';
import {ICMakeBuildConfig} from '../ts-cmake/cmake-config';
import {EnvGccKey, EnvKey} from '../ts-cmake/env';
import {CMAKE_BIN_DIR, QT_HOST_ROOT as QT_ROOT, VK_LAYER_PATH, VULKAN_SDK} from './env';

const GCC_HOST_ROOT = path.resolve(os.homedir(), 'shared/gcc-13.2.0');

const CC = path.resolve(GCC_HOST_ROOT, 'bin/gcc');
const CXX = path.resolve(GCC_HOST_ROOT, 'bin/g++');


const PATH = [
    path.resolve(QT_ROOT, 'bin'),
    path.resolve(VULKAN_SDK, 'bin'),
    path.resolve(GCC_HOST_ROOT, 'bin'),
    process.env.PATH ?? ''
].join(path.delimiter);

const LD_LIBRARY_PATH = [
    path.resolve(QT_ROOT, 'lib'),
    path.resolve(VULKAN_SDK, 'lib'),
    path.resolve(GCC_HOST_ROOT, 'lib64'),
    process.env.LD_LIBRARY_PATH ?? ''
].join(path.delimiter);

const config: ICMakeBuildConfig = {
    bin: {
        c: CC,
        cxx: CXX,
        cmake: path.resolve(CMAKE_BIN_DIR, 'cmake'),
        ninja: 'ninja',
        ctest: path.resolve(CMAKE_BIN_DIR, 'ctest'),
    },
    sourceDir: '.',
    buildDir: 'cmake-build-all/linux-gcc-13-shared',
    env: [
        [EnvGccKey.CC, CC],
        [EnvGccKey.CXX, CXX],
        [EnvKey.VULKAN_SDK, VULKAN_SDK],
        [EnvKey.VK_LAYER_PATH, VK_LAYER_PATH],
        [EnvKey.PATH, PATH],
        [EnvKey.LD_LIBRARY_PATH, LD_LIBRARY_PATH],
    ],
    config: {
        generator: 'Ninja',
        options: [
            'CMAKE_BUILD_TYPE:STRING=Release',
            'XGD_USE_CUDA:BOOL=OFF',
            'BUILD_SHARED_LIBS:BOOL=ON',
            'Python_EXECUTABLE=/home/xgd/miniconda3/envs/nn/bin/python'
        ]
    },
    build: {
        parallel: true,
        options: ['--config Release'],
        targets: ['all']
    },
    test: {
        parallel: true,
    },
};

export {config};