import {JSONConstructor, JSONWrapper} from '../Wrapper';
import {CommonMfencedMixin} from 'mathjax-full/js/output/common/Wrappers/mfenced';
import {MmlMfenced} from 'mathjax-full/js/core/MmlTree/MmlNodes/mfenced';
import {JSONinferredMrow} from './mrow';

/*****************************************************************/
/**
 * The JSONmfenced wrapper for the MmlMfenced object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
export class JSONmfenced<N, T, D> extends CommonMfencedMixin<JSONConstructor<any, any, any>>(JSONWrapper) {

    /**
     * The mfenced wrapper
     */
    public static kind = MmlMfenced.prototype.kind;

    /**
     * An mrow used to render the result
     */
    public mrow: JSONinferredMrow<N, T, D>;

    /**
     * @override
     */
    public toJSON(parent: N) {
        const svg = this.standardJSONnode(parent);
        this.setChildrenParent(this.mrow);  // temporarily change parents to the mrow
        this.mrow.toJSON(svg);
        this.setChildrenParent(this);       // put back the correct parents
    }

    /**
     * @param {JSONWrapper} parent   The parent to use for the fenced children
     */
    protected setChildrenParent(parent: JSONWrapper<N, T, D>) {
        for (const child of this.childNodes) {
            child.parent = parent;
        }
    }

}
