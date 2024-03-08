import {JSONConstructor, JSONWrapper} from '../Wrapper';
import {CommonMfracMixin} from 'mathjax-full/js/output/common/Wrappers/mfrac';
import {MmlMfrac} from 'mathjax-full/js/core/MmlTree/MmlNodes/mfrac';
import {JSONmo} from './mo';

/*****************************************************************/
/**
 * The JSONmfrac wrapper for the MmlMfrac object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
export class JSONmfrac<N, T, D> extends CommonMfracMixin<JSONConstructor<any, any, any>>(JSONWrapper) {

    /**
     * The mfrac wrapper
     */
    public static kind = MmlMfrac.prototype.kind;

    /**
     * An mo element used to render bevelled fractions
     */
    public bevel: JSONmo<N, T, D>;

    /************************************************/

    /**
     * @override
     */
    public toJSON(parent: N) {
        this.standardJSONnode(parent);
        const {linethickness, bevelled} = this.node.attributes.getList('linethickness', 'bevelled');
        const display = this.isDisplay();
        if (bevelled) {
            this.makeBevelled(display);
        } else {
            const thickness = this.length2em(String(linethickness), .06);
            if (thickness === 0) {
                this.makeAtop(display);
            } else {
                this.makeFraction(display, thickness);
            }
        }
    }

    /************************************************/

    /**
     * @param {boolean} display  True when fraction is in display mode
     * @param {number} t         The rule line thickness
     */
    protected makeFraction(display: boolean, t: number) {
        const svg = this.element;
        const {numalign, denomalign} = this.node.attributes.getList('numalign', 'denomalign');
        const [num, den] = this.childNodes;
        const nbox = num.getOuterBBox();
        const dbox = den.getOuterBBox();

        const tex = this.font.params;
        const a = tex.axis_height;
        const d = .1; // line's extra left- and right-padding
        const pad = (this.node.getProperty('withDelims') ? 0 : tex.nulldelimiterspace);
        const W = Math.max((nbox.L + nbox.w + nbox.R) * nbox.rscale,
            (dbox.L + dbox.w + dbox.R) * dbox.rscale);
        const nx = this.getAlignX(W, nbox, numalign as string) + d + pad;
        const dx = this.getAlignX(W, dbox, denomalign as string) + d + pad;
        const {T, u, v} = this.getTUV(display, t);

        num.toJSON(svg);
        num.place(nx, a + T + Math.max(nbox.d * nbox.rscale, u));
        den.toJSON(svg);
        den.place(dx, a - T - Math.max(dbox.h * dbox.rscale, v));

        this.adaptor.append(svg, this.svg('rect', {
            width: this.fixed(W + 2 * d), height: this.fixed(t),
            x: this.fixed(pad), y: this.fixed(a - t / 2)
        }));
    }

    /************************************************/

    /**
     * @param {boolean} display  True when fraction is in display mode
     */
    protected makeAtop(display: boolean) {
        const svg = this.element;
        const {numalign, denomalign} = this.node.attributes.getList('numalign', 'denomalign');
        const [num, den] = this.childNodes;
        const nbox = num.getOuterBBox();
        const dbox = den.getOuterBBox();

        const tex = this.font.params;
        const pad = (this.node.getProperty('withDelims') ? 0 : tex.nulldelimiterspace);
        const W = Math.max((nbox.L + nbox.w + nbox.R) * nbox.rscale,
            (dbox.L + dbox.w + dbox.R) * dbox.rscale);
        const nx = this.getAlignX(W, nbox, numalign as string) + pad;
        const dx = this.getAlignX(W, dbox, denomalign as string) + pad;
        const {u, v} = this.getUVQ(display);

        num.toJSON(svg);
        num.place(nx, u);
        den.toJSON(svg);
        den.place(dx, -v);
    }

    /************************************************/

    /**
     * @param {boolean} display  True when fraction is in display mode
     */
    protected makeBevelled(display: boolean) {
        const svg = this.element;
        const [num, den] = this.childNodes;
        const {u, v, delta, nbox, dbox} = this.getBevelData(display);
        const w = (nbox.L + nbox.w + nbox.R) * nbox.rscale;

        num.toJSON(svg);
        this.bevel.toJSON(svg);
        den.toJSON(svg);

        num.place(nbox.L * nbox.rscale, u);
        this.bevel.place(w - delta / 2, 0);
        den.place(w + this.bevel.getOuterBBox().w + dbox.L * dbox.rscale - delta, v);
    }

}
