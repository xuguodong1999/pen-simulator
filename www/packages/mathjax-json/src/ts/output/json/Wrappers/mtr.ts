import {Constructor, JSONConstructor, JSONWrapper} from '../Wrapper';
import {CommonMlabeledtrMixin, CommonMtrMixin} from 'mathjax-full/js/output/common/Wrappers/mtr';
import {JSONmtd} from './mtd';
import {MmlMlabeledtr, MmlMtr} from 'mathjax-full/js/core/MmlTree/MmlNodes/mtr';


/**
 * The data needed for placeCell()
 */
export type SizeData = {
    x: number,
    y: number,
    w: number,
    lSpace: number,
    rSpace: number,
    lLine: number,
    rLine: number
};

/*****************************************************************/
/**
 * The JSONmtr wrapper for the MmlMtr object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
export class JSONmtr<N, T, D> extends CommonMtrMixin<JSONmtd<any, any, any>, JSONConstructor<any, any, any>>(JSONWrapper) {

    /**
     * The mtr wrapper
     */
    public static kind = MmlMtr.prototype.kind;

    /**
     * The height of the row
     */
    public H: number;
    /**
     * The depth of the row
     */
    public D: number;
    /**
     * The space above the row
     */
    public tSpace: number;
    /**
     * The space below the row
     */
    public bSpace: number;
    /**
     * The line space above the row
     */
    public tLine: number;
    /**
     * The line space below the row
     */
    public bLine: number;

    /**
     * @override
     */
    public toJSON(parent: N) {
        const svg = this.standardJSONnode(parent);
        this.placeCells(svg);
        this.placeColor();
    }

    /**
     * @param {JSONmtd} cell      The cell to place
     * @param {SizeData} sizes   The positioning information
     * @return {number}          The new x position
     */
    public placeCell(cell: JSONmtd<N, T, D>, sizes: SizeData): number {
        const {x, y, lSpace, w, rSpace, lLine, rLine} = sizes;
        const scale = 1 / this.getBBox().rscale;
        const [h, d] = [this.H * scale, this.D * scale];
        const [t, b] = [this.tSpace * scale, this.bSpace * scale];
        const [dx, dy] = cell.placeCell(x + lSpace, y, w, h, d);
        const W = lSpace + w + rSpace;
        cell.placeColor(-(dx + lSpace + lLine / 2), -(d + b + dy), W + (lLine + rLine) / 2, h + d + t + b);
        return W + rLine;
    }

    /**
     * Set the location of the cell contents in the row and expand the cell background colors
     *
     * @param {N} svg   The container for the table
     */
    protected placeCells(svg: N) {
        const cSpace = this.parent.getColumnHalfSpacing();
        const cLines = [this.parent.fLine, ...this.parent.cLines, this.parent.fLine];
        const cWidth = this.parent.getComputedWidths();
        const scale = 1 / this.getBBox().rscale;
        let x = cLines[0];
        for (let i = 0; i < this.numCells; i++) {
            const child = this.getChild(i);
            child.toJSON(svg);
            x += this.placeCell(child, {
                x: x, y: 0, lSpace: cSpace[i] * scale, rSpace: cSpace[i + 1] * scale, w: cWidth[i] * scale,
                lLine: cLines[i] * scale, rLine: cLines[i + 1] * scale
            });
        }
    }

    /**
     * Expand the backgound color to fill the entire row
     */
    protected placeColor() {
        const scale = 1 / this.getBBox().rscale;
        const adaptor = this.adaptor;
        const child = this.firstChild();
        if (child && adaptor.kind(child) === 'rect' && adaptor.getAttribute(child, 'data-bgcolor')) {
            const [TL, BL] = [(this.tLine / 2) * scale, (this.bLine / 2) * scale];
            const [TS, BS] = [this.tSpace * scale, this.bSpace * scale];
            const [H, D] = [this.H * scale, this.D * scale];
            adaptor.setAttribute(child, 'y', this.fixed(-(D + BS + BL)));
            adaptor.setAttribute(child, 'width', this.fixed(this.parent.getWidth() * scale));
            adaptor.setAttribute(child, 'height', this.fixed(TL + TS + H + D + BS + BL));
        }
    }

}

/*****************************************************************/
/**
 * The JSONlabeledmtr wrapper for the MmlMlabeledtr object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class JSONmlabeledtr<N, T, D> extends CommonMlabeledtrMixin<JSONmtd<any, any, any>, Constructor<JSONmtr<any, any, any>>>(JSONmtr) {

    /**
     * The mlabeledtr wrapper
     */
    public static kind = MmlMlabeledtr.prototype.kind;

    /**
     * @override
     */
    public toJSON(parent: N) {
        super.toJSON(parent);
        const child = this.childNodes[0];
        if (child) {
            child.toJSON(this.parent.labels);
        }
    }

}
