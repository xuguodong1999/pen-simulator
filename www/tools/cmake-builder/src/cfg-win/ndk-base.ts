import * as os from 'os';
import * as path from 'path';
import {ICMakeBuildConfig} from '../ts-cmake/cmake-config';
import {AndroidEnvKey, EnvKey} from '../ts-cmake/env';
import {CMAKE_BIN_DIR, GCC_HOST_ROOT, NINJA_BIN_DIR, QT_HOST_ROOT} from './env';

const ANDROID_PLATFORM = '24';
const ANDROID_ABI = 'arm64-v8a';
const NDK_VERSION = '26.1.10909125';
const ANDROID_HOME = path.resolve(os.homedir(), 'AppData/Local/Android/Sdk');
const ANDROID_NDK = path.resolve(ANDROID_HOME, 'ndk', NDK_VERSION);

const QT_ROOT = path.resolve(`C:/Shared/Qt/6.5.3/android_${ANDROID_ABI.replace('-', '_')}`);

const JAVA_HOME = path.resolve('C:/Shared/jdk11');

const PATH = [
    path.resolve(GCC_HOST_ROOT, 'bin'),
    path.resolve(QT_HOST_ROOT, 'bin'),
    path.resolve(QT_ROOT, 'bin'),
    path.resolve(JAVA_HOME, 'bin'),
    process.env.PATH ?? ''
].join(path.delimiter);

const config: ICMakeBuildConfig = {
    bin: {
        c: undefined,
        cxx: undefined,
        cmake: path.resolve(CMAKE_BIN_DIR, 'cmake.exe'),
        ninja: path.resolve(NINJA_BIN_DIR, 'ninja.exe'),
        ctest: undefined,
    },
    sourceDir: '.',
    buildDir: 'cmake-build-all/ndk-base',
    env: [
        [EnvKey.PATH, PATH],
        [AndroidEnvKey.ANDROID_HOME, ANDROID_HOME],
        [AndroidEnvKey.JAVA_HOME, JAVA_HOME],
    ],
    config: {
        generator: 'Ninja',
        options: [
            `CMAKE_TOOLCHAIN_FILE:FILEPATH=${path.resolve(ANDROID_NDK, 'build/cmake/android.toolchain.cmake')}`,
            `QT_HOST_PATH:PATH=${QT_HOST_ROOT}`,
            `CMAKE_PREFIX_PATH:PATH=${QT_ROOT}`,
            `CMAKE_FIND_ROOT_PATH:PATH=${QT_ROOT}`,
            'CMAKE_BUILD_TYPE:STRING=Release',
            // https://github.com/android/ndk/issues/243
            'ANDROID_USE_LEGACY_TOOLCHAIN_FILE:BOOL=OFF',
            `ANDROID_SDK_ROOT:PATH=${ANDROID_HOME}`,
            `ANDROID_ABI:STRING=${ANDROID_ABI}`,
            `ANDROID_NDK:PATH=${ANDROID_NDK}`,
            `ANDROID_PLATFORM:STRING=${ANDROID_PLATFORM}`,
        ]
    },
    build: {
        parallel: true,
        options: ['--config Release'],
        targets: [
            'all',
            // 'COCR_make_apk',
        ],
    },
};
export {config};
