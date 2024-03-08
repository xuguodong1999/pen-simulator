import {composedCharMap} from '@xgd/mathjax-fork'; // remove "module": "./dist/index.js" in mathjax-fork/package.json
// import {Wrapper} from '@xgd/mathjax-fork/dist/index.cjs';
import {groupSimilarChars} from "./src/cpp-data";
import {writeDebugFile} from "./src/debug-mathjax";
import {cleanIm2Latex} from "./src/clean-im2latex";


function printDebugVar() {
    console.log(composedCharMap);
    // console.log(unicodeCharToHex('😅'));
    // console.log(JSON.stringify(variants));
    return;
}

const _ = [
    groupSimilarChars,
    writeDebugFile,
    cleanIm2Latex,
    printDebugVar,
];

// cleanIm2Latex();
// printDebugVar();
writeDebugFile();

// batched operation generation with a max count limit,
// embed result with all operations applied, forward the network along with origin input again.
