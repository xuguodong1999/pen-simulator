import {mergeBIntoAByKey} from '../ts-cmake/utils';
import {config as base} from './ndk-base';

const config = mergeBIntoAByKey(base, {
    buildDir: 'cmake-build-all/android-ndk-shared',
    config: {
        options: [
            'BUILD_SHARED_LIBS:BOOL=ON',
            'ANDROID_STL=c++_shared',
        ]
    },
});

export {config};