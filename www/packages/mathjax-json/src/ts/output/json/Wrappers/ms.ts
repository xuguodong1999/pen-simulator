import {JSONConstructor, JSONWrapper} from '../Wrapper';
import {CommonMsMixin} from 'mathjax-full/js/output/common/Wrappers/ms';
import {MmlMs} from 'mathjax-full/js/core/MmlTree/MmlNodes/ms';

/*****************************************************************/
/**
 * The JSONms wrapper for the MmlMs object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class JSONms<N, T, D> extends CommonMsMixin<JSONConstructor<any, any, any>>(JSONWrapper) {

    /**
     * The ms wrapper
     */
    public static kind = MmlMs.prototype.kind;

}
