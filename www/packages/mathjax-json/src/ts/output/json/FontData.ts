import {
    CharData,
    CharMap,
    CharOptions,
    DelimiterData,
    FontData,
    VariantData
} from 'mathjax-full/js/output/common/FontData';

export * from 'mathjax-full/js/output/common/FontData';

export type CharStringMap = { [name: number]: string };

/**
 * Add the extra data needed for CharOptions in JSON
 */
export interface JSONCharOptions extends CharOptions {
    c?: string;                   // the character value (overrides default value)
    p?: string;                   // svg path
}

/**
 * Shorthands for JSON char maps and char data
 */
export type JSONCharMap = CharMap<JSONCharOptions>;
export type JSONCharData = CharData<JSONCharOptions>;

/**
 * The extra data needed for a Variant in JSON output
 */
export interface JSONVariantData extends VariantData<JSONCharOptions> {
    cacheID: string;
}

/**
 * the extra data neede for a Delimiter in JSON output
 */
export interface JSONDelimiterData extends DelimiterData {
}


/****************************************************************************/

/**
 * The JSON FontData class
 */
export class JSONFontData extends FontData<JSONCharOptions, JSONVariantData, JSONDelimiterData> {

    /**
     * @override
     */
    public static OPTIONS = {
        ...FontData.OPTIONS,
        dynamicPrefix: './output/svg/fonts'
    };

    /**
     * @override
     */
    public static JAX = 'JSON';

    /**
     * @override
     */
    public static charOptions(font: JSONCharMap, n: number) {
        return super.charOptions(font, n) as JSONCharOptions;
    }

    public getFullVariant() {
        return this.variant;
    }
}

export type JSONFontDataClass = typeof JSONFontData;

/****************************************************************************/

/**
 * @param {CharMap} font        The font to augment
 * @param {CharStringMap} paths     The path data to use for each character
 * @param {CharStringMap} content   The string to use for remapped characters
 * @return {JSONCharMap}            The augmented font
 */
export function AddPaths(font: JSONCharMap, paths: CharStringMap, content: CharStringMap): JSONCharMap {
    for (const c of Object.keys(paths)) {
        const n = parseInt(c);
        JSONFontData.charOptions(font, n).p = paths[n];
    }
    for (const c of Object.keys(content)) {
        const n = parseInt(c);
        JSONFontData.charOptions(font, n).c = content[n];
    }
    return font;
}
