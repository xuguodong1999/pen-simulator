import {
    CharMapMap,
    DelimiterMap,
    JSONCharOptions,
    JSONDelimiterData,
    JSONFontData,
    JSONFontDataClass,
    JSONVariantData
} from '../FontData';
import {CommonTeXFontMixin} from 'mathjax-full/js/output/common/fonts/tex';
import {OptionList} from 'mathjax-full/js/util/Options';

import {boldItalic} from 'mathjax-full/js/output/svg/fonts/tex/bold-italic';
import {bold} from 'mathjax-full/js/output/svg/fonts/tex/bold';
import {doubleStruck} from 'mathjax-full/js/output/svg/fonts/tex/double-struck';
import {frakturBold} from 'mathjax-full/js/output/svg/fonts/tex/fraktur-bold';
import {fraktur} from 'mathjax-full/js/output/svg/fonts/tex/fraktur';
import {italic} from 'mathjax-full/js/output/svg/fonts/tex/italic';
import {largeop} from 'mathjax-full/js/output/svg/fonts/tex/largeop';
import {monospace} from 'mathjax-full/js/output/svg/fonts/tex/monospace';
import {normal} from 'mathjax-full/js/output/svg/fonts/tex/normal';
import {sansSerifBoldItalic} from 'mathjax-full/js/output/svg/fonts/tex/sans-serif-bold-italic';
import {sansSerifBold} from 'mathjax-full/js/output/svg/fonts/tex/sans-serif-bold';
import {sansSerifItalic} from 'mathjax-full/js/output/svg/fonts/tex/sans-serif-italic';
import {sansSerif} from 'mathjax-full/js/output/svg/fonts/tex/sans-serif';
import {scriptBold} from 'mathjax-full/js/output/svg/fonts/tex/script-bold';
import {script} from 'mathjax-full/js/output/svg/fonts/tex/script';
import {smallop} from 'mathjax-full/js/output/svg/fonts/tex/smallop';
import {texCalligraphicBold} from 'mathjax-full/js/output/svg/fonts/tex/tex-calligraphic-bold';
import {texCalligraphic} from 'mathjax-full/js/output/svg/fonts/tex/tex-calligraphic';
import {texMathit} from 'mathjax-full/js/output/svg/fonts/tex/tex-mathit';
import {texOldstyleBold} from 'mathjax-full/js/output/svg/fonts/tex/tex-oldstyle-bold';
import {texOldstyle} from 'mathjax-full/js/output/svg/fonts/tex/tex-oldstyle';
import {texSize3} from 'mathjax-full/js/output/svg/fonts/tex/tex-size3';
import {texSize4} from 'mathjax-full/js/output/svg/fonts/tex/tex-size4';
import {texVariant} from 'mathjax-full/js/output/svg/fonts/tex/tex-variant';

import {delimiters} from 'mathjax-full/js/output/common/fonts/tex/delimiters';

/***********************************************************************************/
/**
 *  The TeXFont class
 */
export class TeXFont extends CommonTeXFontMixin<JSONCharOptions, JSONVariantData, JSONDelimiterData, JSONFontDataClass>(JSONFontData) {

    /**
     *  The stretchy delimiter data
     */
    protected static defaultDelimiters: DelimiterMap<JSONDelimiterData> = delimiters;

    /**
     *  The character data by variant
     */
    protected static defaultChars: CharMapMap<JSONCharOptions> = {
        'normal': normal,
        'bold': bold,
        'italic': italic,
        'bold-italic': boldItalic,
        'double-struck': doubleStruck,
        'fraktur': fraktur,
        'bold-fraktur': frakturBold,
        'script': script,
        'bold-script': scriptBold,
        'sans-serif': sansSerif,
        'bold-sans-serif': sansSerifBold,
        'sans-serif-italic': sansSerifItalic,
        'sans-serif-bold-italic': sansSerifBoldItalic,
        'monospace': monospace,
        '-smallop': smallop,
        '-largeop': largeop,
        '-size3': texSize3,
        '-size4': texSize4,
        '-tex-calligraphic': texCalligraphic,
        '-tex-bold-calligraphic': texCalligraphicBold,
        '-tex-mathit': texMathit,
        '-tex-oldstyle': texOldstyle,
        '-tex-bold-oldstyle': texOldstyleBold,
        '-tex-variant': texVariant
    };

    /**
     * The cacheIDs to use for the variants in font-caching
     */
    protected static variantCacheIds: { [name: string]: string } = {
        'normal': 'N',
        'bold': 'B',
        'italic': 'I',
        'bold-italic': 'BI',
        'double-struck': 'D',
        'fraktur': 'F',
        'bold-fraktur': 'BF',
        'script': 'S',
        'bold-script': 'BS',
        'sans-serif': 'SS',
        'bold-sans-serif': 'BSS',
        'sans-serif-italic': 'SSI',
        'sans-serif-bold-italic': 'SSBI',
        'monospace': 'M',
        '-smallop': 'SO',
        '-largeop': 'LO',
        '-size3': 'S3',
        '-size4': 'S4',
        '-tex-calligraphic': 'C',
        '-tex-bold-calligraphic': 'BC',
        '-tex-mathit': 'MI',
        '-tex-oldstyle': 'OS',
        '-tex-bold-oldstyle': 'BOS',
        '-tex-variant': 'V'
    };

    /**
     * @override
     */
    constructor(options: OptionList = null) {
        super(options);
        //
        //  Add the cacheIDs to the variants
        //
        const CLASS = this.constructor as typeof TeXFont;
        for (const variant of Object.keys(CLASS.variantCacheIds)) {
            this.variant[variant].cacheID = 'TEX-' + CLASS.variantCacheIds[variant];
        }
    }

}
