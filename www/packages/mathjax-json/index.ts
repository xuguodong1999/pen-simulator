import {
    AllPackages,
    delimiters,
    entities,
    JSONFontData,
    JSONOutput,
    liteAdaptor,
    mathjax,
    RegisterHTMLHandler,
    SVG,
    TeX,
    Wrapper
} from './src/wrapper';

// https://stackoverflow.com/questions/11616630/how-can-i-print-a-circular-structure-in-a-json-like-format
const getCircularReplacer = () => {
    const seen = new WeakSet();
    return (_key: any, value: object) => {
        if (typeof value === "object" && value !== null) {
            if (seen.has(value)) {
                return;
            }
            seen.add(value);
        }
        return value;
    };
};

function deepCopy<T>(obj: T) {
    return JSON.parse(JSON.stringify(obj), getCircularReplacer()) as T;
}

const styledCharRange: Array<Array<Number>> = (() => {
    const result = [];
    for (const k of Object.keys(JSONFontData.VariantSmp)) {
        const v = deepCopy(JSONFontData.VariantSmp[k]);
        for (let i = 0; i < v.length; i++) {
            if (!v[i]) {
                v[i] = 0;
            }
        }
        for (let i = v.length; i < 5; i++) {
            v.push(0);
        }
        result.push(v);
    }
    return result;
})();

const charRange: Array<Array<Number>> = (() => {
    const result = [];
    for (const [, beg, end] of JSONFontData.SmpRanges) {
        result.push([beg, end]);
    }
    return result;
})();

const charNameMap: { [name: string]: string } = entities;

const composedCharMap: { [name: string]: Array<string> } = (() => {
    const result = {};
    for (const k of Object.keys(delimiters)) {
        const stretch = delimiters[k]['stretch'];
        const v = []
        if (stretch instanceof Array) {
            for (const c of stretch) {
                if (c) {
                    v.push(String.fromCodePoint(c as number))
                }
            }
        }
        result[String.fromCodePoint(Number.parseInt(k, 10))] = v;
    }
    return result;
})();

const variants: {
    [fontName: string]: Array<{
        [charName: string]: {
            path: string,
        }
    }>
} = (() => {
    const variantMap = (Wrapper.jsonDocument.outputJax as JSONOutput<any, any, any>).getFullVariant();
    // const variantMap = Wrapper.jsonOutput.getFullVariant();
    //    "normal": {
    //         "linked": [],
    //         "chars": {
    //             "32": [
    //                 0,
    //                 0,
    //                 0.25,
    //                 {
    //                     "p": ""
    //                 }
    const result = {};
    for (const fontName of Object.keys(variantMap)) {
        const chars = variantMap[fontName]['chars'];
        const charMap = {};
        for (const charCode of Object.keys(chars)) {
            const charPropList = chars[charCode];
            const charName = String.fromCodePoint(Number.parseInt(charCode, 10));
            for (const charProp of charPropList) {
                if (charProp instanceof Object) {
                    const path = charProp['p'];
                    const maybePath = charMap[charName];
                    if (maybePath) {
                        if (maybePath !== path) {
                            throw new Error(`Same Char Different Path: ${maybePath} !== ${path}`);
                        }
                    } else {
                        charMap[charName] = path;
                    }
                }
            }
        }
        result[fontName] = charMap;
    }
    return result;
})();

function countUnicode(str: string) {
    return Array.from(str).length;
}

function unicodeCharToHex(str: string) {
    const unicodeLength = countUnicode(str);
    if (unicodeLength !== 1) {
        throw new Error(`unicodeCharToHex: unexpected input string length, expected 1, got ${unicodeLength}`);
    }
    return str.codePointAt(0);
}

export {
    AllPackages,
    liteAdaptor,
    mathjax,
    RegisterHTMLHandler,
    SVG,
    TeX,
    variants,
    entities,
    delimiters,
    styledCharRange,
    charRange,
    charNameMap,
    composedCharMap,
    unicodeCharToHex,
    countUnicode,
    Wrapper,
};
