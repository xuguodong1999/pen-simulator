import {Constructor, JSONWrapper} from '../Wrapper';
import {JSONmsub, JSONmsubsup, JSONmsup} from './msubsup';
import {
    CommonMoverMixin,
    CommonMunderMixin,
    CommonMunderoverMixin
} from 'mathjax-full/js/output/common/Wrappers/munderover';
import {MmlMover, MmlMunder, MmlMunderover} from 'mathjax-full/js/core/MmlTree/MmlNodes/munderover';

/*****************************************************************/
/**
 * The JSONmunder wrapper for the MmlMunder object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class JSONmunder<N, T, D> extends CommonMunderMixin<JSONWrapper<any, any, any>, Constructor<JSONmsub<any, any, any>>>(JSONmsub) {

    /**
     * The munder wrapper
     */
    public static kind = MmlMunder.prototype.kind;

    /**
     * @override
     */
    public toJSON(parent: N) {
        if (this.hasMovableLimits()) {
            super.toJSON(parent);
            return;
        }

        const svg = this.standardJSONnode(parent);
        const [base, script] = [this.baseChild, this.scriptChild];
        const [bbox, sbox] = [base.getOuterBBox(), script.getOuterBBox()];

        base.toJSON(svg);
        script.toJSON(svg);

        const delta = (this.isLineBelow ? 0 : this.getDelta(true));
        const v = this.getUnderKV(bbox, sbox)[1];
        const [bx, sx] = this.getDeltaW([bbox, sbox], [0, -delta]);

        base.place(bx, 0);
        script.place(sx, v);
    }

}

/*****************************************************************/
/**
 * The JSONmover wrapper for the MmlMover object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class JSONmover<N, T, D> extends CommonMoverMixin<JSONWrapper<any, any, any>, Constructor<JSONmsup<any, any, any>>>(JSONmsup) {

    /**
     * The mover wrapper
     */
    public static kind = MmlMover.prototype.kind;

    /**
     * @override
     */
    public toJSON(parent: N) {
        if (this.hasMovableLimits()) {
            super.toJSON(parent);
            return;
        }
        const svg = this.standardJSONnode(parent);
        const [base, script] = [this.baseChild, this.scriptChild];
        const [bbox, sbox] = [base.getOuterBBox(), script.getOuterBBox()];

        base.toJSON(svg);
        script.toJSON(svg);

        const delta = (this.isLineAbove ? 0 : this.getDelta());
        const u = this.getOverKU(bbox, sbox)[1];
        const [bx, sx] = this.getDeltaW([bbox, sbox], [0, delta]);

        base.place(bx, 0);
        script.place(sx, u);
    }

}

/*****************************************************************/
/*
 * The JSONmunderover wrapper for the MmlMunderover object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */

// @ts-ignore
export class JSONmunderover<N, T, D> extends CommonMunderoverMixin<JSONWrapper<any, any, any>, Constructor<JSONmsubsup<any, any, any>>>(JSONmsubsup) {

    /**
     * The munderover wrapper
     */
    public static kind = MmlMunderover.prototype.kind;

    /**
     * @override
     */
    public toJSON(parent: N) {
        if (this.hasMovableLimits()) {
            super.toJSON(parent);
            return;
        }
        const svg = this.standardJSONnode(parent);
        const [base, over, under] = [this.baseChild, this.overChild, this.underChild];
        const [bbox, obox, ubox] = [base.getOuterBBox(), over.getOuterBBox(), under.getOuterBBox()];

        base.toJSON(svg);
        under.toJSON(svg);
        over.toJSON(svg);

        const delta = this.getDelta();
        const u = this.getOverKU(bbox, obox)[1];
        const v = this.getUnderKV(bbox, ubox)[1];
        const [bx, ux, ox] = this.getDeltaW([bbox, ubox, obox],
            [0, this.isLineBelow ? 0 : -delta, this.isLineAbove ? 0 : delta]);

        base.place(bx, 0);
        under.place(ux, v);
        over.place(ox, u);
    }

}
