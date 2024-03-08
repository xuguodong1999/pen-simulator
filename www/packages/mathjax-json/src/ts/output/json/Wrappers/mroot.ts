import {Constructor, JSONWrapper} from '../Wrapper';
import {JSONmsqrt} from './msqrt';
import {CommonMrootMixin} from 'mathjax-full/js/output/common/Wrappers/mroot';
import {BBox} from 'mathjax-full/js/util/BBox';
import {MmlMroot} from 'mathjax-full/js/core/MmlTree/MmlNodes/mroot';

/*****************************************************************/
/**
 * The JSONmroot wrapper for the MmlMroot object (extends JSONmsqrt)
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
export class JSONmroot<N, T, D> extends CommonMrootMixin<Constructor<JSONmsqrt<any, any, any>>>(JSONmsqrt) {

    /**
     * The mroot wrapper
     */
    public static kind = MmlMroot.prototype.kind;

    /**
     * @override
     */
    protected addRoot(ROOT: N, root: JSONWrapper<N, T, D>, sbox: BBox, H: number) {
        root.toJSON(ROOT);
        const [x, h, dx] = this.getRootDimens(sbox, H);
        const bbox = root.getOuterBBox();
        root.place(dx * bbox.rscale, h);
        this.dx = x;
    }

}
