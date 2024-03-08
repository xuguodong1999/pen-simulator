import {JSONConstructor, JSONWrapper} from '../Wrapper';
import {CommonMiMixin} from 'mathjax-full/js/output/common/Wrappers/mi';
import {MmlMi} from 'mathjax-full/js/core/MmlTree/MmlNodes/mi';

/*****************************************************************/
/**
 *  The JSONmi wrapper for the MmlMi object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class JSONmi<N, T, D> extends CommonMiMixin<JSONConstructor<any, any, any>>(JSONWrapper) {

    /**
     * The mi wrapper
     */
    public static kind = MmlMi.prototype.kind;

}
