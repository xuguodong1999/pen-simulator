import {JSONConstructor, JSONWrapper} from '../Wrapper';
import {CommonMtextMixin} from 'mathjax-full/js/output/common/Wrappers/mtext';
import {MmlMtext} from 'mathjax-full/js/core/MmlTree/MmlNodes/mtext';

/*****************************************************************/
/**
 * The JSONmtext wrapper for the MmlMtext object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class JSONmtext<N, T, D> extends CommonMtextMixin<JSONConstructor<any, any, any>>(JSONWrapper) {

    /**
     * The mtext wrapper
     */
    public static kind = MmlMtext.prototype.kind;

}
