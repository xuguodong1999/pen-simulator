import {JSONConstructor, JSONWrapper} from '../Wrapper';
import {CommonMnMixin} from 'mathjax-full/js/output/common/Wrappers/mn';
import {MmlMn} from 'mathjax-full/js/core/MmlTree/MmlNodes/mn';

/*****************************************************************/
/**
 * The JSONmn wrapper for the MmlMn object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class JSONmn<N, T, D> extends CommonMnMixin<JSONConstructor<any, any, any>>(JSONWrapper) {

    /**
     * The mn wrapper
     */
    public static kind = MmlMn.prototype.kind;

}
