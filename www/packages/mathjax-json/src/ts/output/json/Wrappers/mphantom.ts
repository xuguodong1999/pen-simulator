import {JSONWrapper} from '../Wrapper';
import {MmlMphantom} from 'mathjax-full/js/core/MmlTree/MmlNodes/mphantom';

/*****************************************************************/
/**
 *  The JSONmi wrapper for the MmlMi object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
export class JSONmphantom<N, T, D> extends JSONWrapper<N, T, D> {

    /**
     * The mphantom wrapper
     */
    public static kind = MmlMphantom.prototype.kind;

    /**
     * @override
     */
    public toJSON(parent: N) {
        this.standardJSONnode(parent);
    }

}
