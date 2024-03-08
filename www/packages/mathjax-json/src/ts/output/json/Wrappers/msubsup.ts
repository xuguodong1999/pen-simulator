import {Constructor, JSONWrapper} from '../Wrapper';
import {JSONscriptbase} from './scriptbase';
import {CommonMsubMixin, CommonMsubsupMixin, CommonMsupMixin} from 'mathjax-full/js/output/common/Wrappers/msubsup';
import {MmlMsub, MmlMsubsup, MmlMsup} from 'mathjax-full/js/core/MmlTree/MmlNodes/msubsup';

/*****************************************************************/
/**
 * The JSONmsub wrapper for the MmlMsub object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class JSONmsub<N, T, D> extends CommonMsubMixin<JSONWrapper<any, any, any>, Constructor<JSONscriptbase<any, any, any>>>(JSONscriptbase) {

    /**
     * The msub wrapper
     */
    public static kind = MmlMsub.prototype.kind;

}

/*****************************************************************/
/**
 * The JSONmsup wrapper for the MmlMsup object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class JSONmsup<N, T, D> extends CommonMsupMixin<JSONWrapper<any, any, any>, Constructor<JSONscriptbase<any, any, any>>>(JSONscriptbase) {

    /**
     * The msup wrapper
     */
    public static kind = MmlMsup.prototype.kind;

}

/*****************************************************************/
/**
 * The JSONmsubsup wrapper for the MmlMsubsup object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class JSONmsubsup<N, T, D> extends CommonMsubsupMixin<JSONWrapper<any, any, any>, Constructor<JSONscriptbase<any, any, any>>>(JSONscriptbase) {

    /**
     * The msubsup wrapper
     */
    public static kind = MmlMsubsup.prototype.kind;

    /**
     * @override
     */
    public toJSON(parent: N) {
        const svg = this.standardJSONnode(parent);
        const [base, sup, sub] = [this.baseChild, this.supChild, this.subChild];
        const w = this.getBaseWidth();
        const x = this.getAdjustedIc();
        const [u, v] = this.getUVQ();

        base.toJSON(svg);
        sup.toJSON(svg);
        sub.toJSON(svg);

        sub.place(w, v);
        sup.place(w + x, u);
    }

}
