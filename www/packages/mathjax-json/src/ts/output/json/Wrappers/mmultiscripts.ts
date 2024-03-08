import {Constructor, JSONWrapper} from '../Wrapper';
import {JSONmsubsup} from './msubsup';
import {CommonMmultiscriptsMixin} from 'mathjax-full/js/output/common/Wrappers/mmultiscripts';
import {MmlMmultiscripts} from 'mathjax-full/js/core/MmlTree/MmlNodes/mmultiscripts';
import {split} from 'mathjax-full/js/util/string';

/*****************************************************************/

/**
 * A function taking two widths and returning an offset of the first in the second
 */
export type AlignFunction = (w: number, W: number) => number;

/**
 * Get the function for aligning scripts horizontally (left, center, right)
 */
export function AlignX(align: string) {
    return ({
        left: (_w, _W) => 0,
        center: (w, W) => (W - w) / 2,
        right: (w, W) => W - w
    } as { [name: string]: AlignFunction })[align] || ((_w, _W) => 0) as AlignFunction;
}

/*****************************************************************/
/**
 * The JSONmmultiscripts wrapper for the MmlMmultiscripts object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class JSONmmultiscripts<N, T, D> extends CommonMmultiscriptsMixin<JSONWrapper<any, any, any>, Constructor<JSONmsubsup<any, any, any>>>(JSONmsubsup) {

    /**
     * The mmultiscripts wrapper
     */
    public static kind = MmlMmultiscripts.prototype.kind;

    /**
     * @override
     */
    public toJSON(parent: N) {
        const svg = this.standardJSONnode(parent);
        const data = this.scriptData;
        //
        //  Get the alignment for the scripts
        //
        const scriptalign = this.node.getProperty('scriptalign') || 'right left';
        const [preAlign, postAlign] = split(scriptalign + ' ' + scriptalign);
        //
        //  Combine the bounding boxes of the pre- and post-scripts,
        //  and get the resulting baseline offsets
        //
        const sub = this.combinePrePost(data.sub, data.psub);
        const sup = this.combinePrePost(data.sup, data.psup);
        const [u, v] = this.getUVQ(sub, sup);
        //
        //  Place the pre-scripts, then the base, then the post-scripts
        //
        let x = 0;  // scriptspace
        if (data.numPrescripts) {
            x = this.addScripts(.05, u, v, this.firstPrescript, data.numPrescripts, preAlign);
        }
        const base = this.baseChild;
        base.toJSON(svg);
        base.place(x, 0);
        x += base.getOuterBBox().w;
        if (data.numScripts) {
            this.addScripts(x, u, v, 1, data.numScripts, postAlign);
        }
    }

    /**
     * Create a table with the super and subscripts properly separated and aligned.
     *
     * @param {number} x       The x offset of the scripts
     * @param {number} u       The baseline offset for the superscripts
     * @param {number} v       The baseline offset for the subscripts
     * @param {number} i       The starting index for the scripts
     * @param {number} n       The number of sub/super-scripts
     * @param {string} align   The alignment for the scripts
     * @return {number}        The right-hand offset of the scripts
     */
    protected addScripts(x: number, u: number, v: number, i: number, n: number, align: string): number {
        const adaptor = this.adaptor;
        const alignX = AlignX(align);
        const supRow = adaptor.append(this.element, this.svg('g'));
        const subRow = adaptor.append(this.element, this.svg('g'));
        this.place(x, u, supRow);
        this.place(x, v, subRow);
        let m = i + 2 * n;
        let dx = 0;
        while (i < m) {
            const [sub, sup] = [this.childNodes[i++], this.childNodes[i++]];
            const [subbox, supbox] = [sub.getOuterBBox(), sup.getOuterBBox()];
            const [subr, supr] = [subbox.rscale, supbox.rscale];
            const w = Math.max(subbox.w * subr, supbox.w * supr);
            sub.toJSON(subRow);
            sup.toJSON(supRow);
            sub.place(dx + alignX(subbox.w * subr, w), 0);
            sup.place(dx + alignX(supbox.w * supr, w), 0);
            dx += w;
        }
        return x + dx;
    }

}
