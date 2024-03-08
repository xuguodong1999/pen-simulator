import {JSONConstructor, JSONWrapper} from '../Wrapper';
import {CommonScriptbaseMixin} from 'mathjax-full/js/output/common/Wrappers/scriptbase';

/*****************************************************************/
/**
 * A base class for msup/msub/msubsup and munder/mover/munderover
 * wrapper implementations
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class JSONscriptbase<N, T, D> extends CommonScriptbaseMixin<JSONWrapper<any, any, any>, JSONConstructor<any, any, any>>(JSONWrapper) {

    /**
     * The scriptbase wrapper
     */
    public static kind = 'scriptbase';

    /**
     * This gives the common output for msub and msup.  It is overridden
     * for all the others (msubsup, munder, mover, munderover).
     *
     * @override
     */
    public toJSON(parent: N) {
        const svg = this.standardJSONnode(parent);
        const w = this.getBaseWidth();
        const [x, v] = this.getOffset();
        this.baseChild.toJSON(svg);
        this.scriptChild.toJSON(svg);
        this.scriptChild.place(w + x, v);
    }

}
