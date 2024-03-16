import {mergeBIntoAByKey} from '../ts-cmake/utils';
import {config as base} from './clang-static-base';

const config = mergeBIntoAByKey(base, {
    buildDir: 'cmake-build-all/apple-clang-static-arm',
    config: {
        options: [
            'CMAKE_OSX_ARCHITECTURES="arm64"',
            'PYTHON_EXECUTABLE=/Users/xgd/miniforge3/envs/nn/bin/python',
        ]
    },
    test: {
        parallel: true,
    },
});
export {config};