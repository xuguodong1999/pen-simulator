import {JSONConstructor, JSONWrapper} from '../Wrapper';
import {CommonMspaceMixin} from 'mathjax-full/js/output/common/Wrappers/mspace';
import {MmlMspace} from 'mathjax-full/js/core/MmlTree/MmlNodes/mspace';

/*****************************************************************/
/**
 * The JSONmspace wrapper for the MmlMspace object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class JSONmspace<N, T, D> extends CommonMspaceMixin<JSONConstructor<any, any, any>>(JSONWrapper) {

    /**
     * The mspace wrapper
     */
    public static kind = MmlMspace.prototype.kind;

}
