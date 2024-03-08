import {Constructor, JSONConstructor, JSONWrapper} from '../Wrapper';
import {CommonInferredMrowMixin, CommonMrowMixin} from 'mathjax-full/js/output/common/Wrappers/mrow';
import {MmlInferredMrow, MmlMrow} from 'mathjax-full/js/core/MmlTree/MmlNodes/mrow';

/*****************************************************************/
/**
 * The JSONmrow wrapper for the MmlMrow object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class JSONmrow<N, T, D> extends CommonMrowMixin<JSONConstructor<any, any, any>>(JSONWrapper) {

    /**
     * The mrow wrapper
     */
    public static kind = MmlMrow.prototype.kind;

    /**
     * @override
     */
    public toJSON(parent: N) {
        const svg = (this.node.isInferred ? (this.element = parent) : this.standardJSONnode(parent));
        this.addChildren(svg);
        // FIXME:  handle line breaks
    }

}

/*****************************************************************/
/**
 *  The JSONinferredMrow wrapper for the MmlInferredMrow object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class JSONinferredMrow<N, T, D> extends CommonInferredMrowMixin<Constructor<JSONmrow<any, any, any>>>(JSONmrow) {

    /**
     * The inferred-mrow wrapper
     */
    public static kind = MmlInferredMrow.prototype.kind;

}
