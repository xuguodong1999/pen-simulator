import {JSONConstructor, JSONWrapper} from '../Wrapper';
import {CommonMtdMixin} from 'mathjax-full/js/output/common/Wrappers/mtd';
import {MmlMtd} from 'mathjax-full/js/core/MmlTree/MmlNodes/mtd';

/*****************************************************************/
/**
 * The JSONmtd wrapper for the MmlMtd object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class JSONmtd<N, T, D> extends CommonMtdMixin<JSONConstructor<any, any, any>>(JSONWrapper) {

    /**
     * The mtd wrapper
     */
    public static kind = MmlMtd.prototype.kind;

    /**
     * @param {number} x    The x offset of the left side of the cell
     * @param {number} y    The y offset of the baseline of the cell
     * @param {number} W    The width of the cell
     * @param {number} H    The height of the cell
     * @param {number} D    The depth of the cell
     * @return {[number, number]}   The x and y offsets used
     */
    public placeCell(x: number, y: number, W: number, H: number, D: number): [number, number] {
        const bbox = this.getBBox();
        const h = Math.max(bbox.h, .75);
        const d = Math.max(bbox.d, .25);
        const calign = this.node.attributes.get('columnalign') as string;
        const ralign = this.node.attributes.get('rowalign') as string;
        const alignX = this.getAlignX(W, bbox, calign);
        const alignY = this.getAlignY(H, D, h, d, ralign);
        this.place(x + alignX, y + alignY);
        return [alignX, alignY];
    }

    /**
     * @param {number} x    The x offset of the left side of the cell
     * @param {number} y    The y position of the bottom of the cell
     * @param {number} W    The width of the cell
     * @param {number} H    The height + depth of the cell
     */
    public placeColor(x: number, y: number, W: number, H: number) {
        const adaptor = this.adaptor;
        const child = this.firstChild();
        if (child && adaptor.kind(child) === 'rect' && adaptor.getAttribute(child, 'data-bgcolor')) {
            adaptor.setAttribute(child, 'x', this.fixed(x));
            adaptor.setAttribute(child, 'y', this.fixed(y));
            adaptor.setAttribute(child, 'width', this.fixed(W));
            adaptor.setAttribute(child, 'height', this.fixed(H));
        }
    }

}
