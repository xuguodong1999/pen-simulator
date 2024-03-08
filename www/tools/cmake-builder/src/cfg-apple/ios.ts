import * as os from 'os';
import * as path from 'path';
import {ICMakeBuildConfig} from '../ts-cmake/cmake-config';
import {EnvGccKey, EnvKey} from '../ts-cmake/env';

const LLVM_INSTALL_DIR = path.resolve('/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr');
const CC = path.resolve(LLVM_INSTALL_DIR, 'bin/clang');
const CXX = path.resolve(LLVM_INSTALL_DIR, 'bin/clang++');
const CMAKE_BIN_DIR = path.resolve(os.homedir(), 'shared/cmake/CMake.app/Contents/bin');

const QT_ROOT = path.resolve(os.homedir(), 'shared/Qt/6.5.3/ios');
const VULKAN_SDK = path.resolve(os.homedir(), 'shared/vulkan-sdk/macOS');
const VK_LAYER_PATH = path.resolve(VULKAN_SDK, 'share/vulkan/explicit_layer.d');

const PATH = [
    path.resolve(QT_ROOT, 'bin'),
    path.resolve(VULKAN_SDK, 'bin'),
    process.env.PATH ?? ''
].join(path.delimiter);

const LD_LIBRARY_PATH = [
    path.resolve(QT_ROOT, 'lib'),
    path.resolve(VULKAN_SDK, 'lib'),
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
    buildDir: 'cmake-build-all/apple-ios-shared',
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
            'XGD_USE_TORCH=OFF',
            `CMAKE_TOOLCHAIN_FILE:FILEPATH=${path.resolve(QT_ROOT, 'lib/cmake/Qt6/qt.toolchain.cmake')}`,
            'BUILD_SHARED_LIBS:BOOL=ON',
            'CMAKE_BUILD_TYPE:STRING=Release',
        ]
    },
    build: {
        parallel: true,
        options: ['--config Release'],
        targets: ['all']
    },
};
export {config};
