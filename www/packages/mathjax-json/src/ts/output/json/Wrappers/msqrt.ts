import {JSONConstructor, JSONWrapper} from '../Wrapper';
import {CommonMsqrtMixin} from 'mathjax-full/js/output/common/Wrappers/msqrt';
import {BBox} from 'mathjax-full/js/util/BBox';
import {MmlMsqrt} from 'mathjax-full/js/core/MmlTree/MmlNodes/msqrt';

/*****************************************************************/
/**
 * The JSONmsqrt wrapper for the MmlMsqrt object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
export class JSONmsqrt<N, T, D> extends CommonMsqrtMixin<JSONConstructor<any, any, any>>(JSONWrapper) {

    /**
     * The msqrt wrapper
     */
    public static kind = MmlMsqrt.prototype.kind;

    /**
     * Indent due to root
     */
    public dx: number = 0;

    /**
     * @override
     */
    public toJSON(parent: N) {
        const surd = this.childNodes[this.surd];
        const base = this.childNodes[this.base];
        const root = (this.root ? this.childNodes[this.root] : null);
        //
        //  Get the parameters for the spacing of the parts
        //
        const sbox = surd.getBBox();
        const bbox = base.getOuterBBox();
        const q = this.getPQ(sbox)[1];
        const t = this.font.params.rule_thickness * this.bbox.scale;
        const H = bbox.h + q + t;
        //
        //  Create the JSON structure for the root
        //
        const JSON = this.standardJSONnode(parent);
        const BASE = this.adaptor.append(JSON, this.svg('g'));
        //
        //  Place the children
        //
        this.addRoot(JSON, root, sbox, H);
        surd.toJSON(JSON);
        surd.place(this.dx, H - sbox.h);
        base.toJSON(BASE);
        base.place(this.dx + sbox.w, 0);
        this.adaptor.append(JSON, this.svg('rect', {
            width: this.fixed(bbox.w), height: this.fixed(t),
            x: this.fixed(this.dx + sbox.w), y: this.fixed(H - t)
        }));
    }

    /**
     * Add root HTML (overridden in mroot)
     *
     * @param {N} ROOT           The container for the root
     * @param {JSONWrapper} root  The wrapped MML root content
     * @param {BBox} sbox        The bounding box of the surd
     * @param {number} H         The height of the root as a whole
     */
    protected addRoot(_ROOT: N, _root: JSONWrapper<N, T, D>, _sbox: BBox, _H: number) {
    }

}
