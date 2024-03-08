import * as path from 'path';
import {ICMakeBuildConfig} from '../ts-cmake/cmake-config';
import {EnvEmccKey, EnvKey} from '../ts-cmake/env';
import {CMAKE_BIN_DIR, NINJA_BIN_DIR} from './env';

const QT_ROOT = path.resolve('C:/Shared/Qt/6.5.3/wasm_32');

const EMSDK = path.resolve('C:/Shared/emsdk');
const EMSDK_NODE_BIN_DIR = path.resolve('C:/Shared/node');
const EMSDK_NODE = path.resolve(EMSDK_NODE_BIN_DIR, 'node.exe');
const EMSDK_PYTHON_BIN_DIR = path.resolve(EMSDK, 'python/3.9.2-nuget_64bit');
const EMSDK_PYTHON = path.resolve(EMSDK_PYTHON_BIN_DIR, 'python.exe');
const CLANG_BIN_DIR = path.resolve(EMSDK, 'upstream/bin');
const EMCC_BIN_DIR = path.resolve(EMSDK, 'upstream/emscripten');

const PATH = [
    EMSDK,
    EMSDK_NODE_BIN_DIR,
    CLANG_BIN_DIR,
    CMAKE_BIN_DIR,
    EMSDK_PYTHON_BIN_DIR,
    EMCC_BIN_DIR,
    process.env.PATH ?? ''
].join(path.delimiter);

const config: ICMakeBuildConfig = {
    bin: {
        c: path.resolve(EMCC_BIN_DIR, 'emcc.bat'),
        cxx: path.resolve(EMCC_BIN_DIR, 'em++.bat'),
        cmake: path.resolve(CMAKE_BIN_DIR, 'cmake'),
        ninja: path.resolve(NINJA_BIN_DIR, 'ninja'),
        ctest: path.resolve(CMAKE_BIN_DIR, 'ctest'),
    },
    sourceDir: '.',
    buildDir: 'cmake-build-all/wasm-simd-mt',
    env: [
        [EnvKey.PATH, PATH],
        [EnvEmccKey.EMSDK, EMSDK],
        [EnvEmccKey.EMSDK_NODE, EMSDK_NODE],
        [EnvEmccKey.EMSDK_PYTHON, EMSDK_PYTHON]
    ],
    config: {
        generator: 'Ninja',
        options: [
            'CMAKE_NINJA_FORCE_RESPONSE_FILE=1',
            // 'CMAKE_CXX_RESPONSE_FILE_LINK_FLAG=@',
            // 'CMAKE_CXX_USE_RESPONSE_FILE_FOR_LIBRARIES=1',
            // 'CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS=1',
            'BUILD_SHARED_LIBS:BOOL=OFF',
            'CMAKE_BUILD_TYPE:STRING=Release',
            `CMAKE_TOOLCHAIN_FILE:FILEPATH=${QT_ROOT}/lib/cmake/Qt6/qt.toolchain.cmake`,
            '-Wno-dev',
        ]
    },
    build: {
        parallel: true,
        options: ['--config Release'],
        targets: ['all'],
    },
    test: {
        timeout: 15,
        parallel: false,
        targets: ['all'],
        excludes: [
            'AzTestTex',
            'AzTestData',
            'TestQtHybrid',
            'HelloQtSvg',
            'HelloQmlJS',
        ],
    },
};
export {config};
