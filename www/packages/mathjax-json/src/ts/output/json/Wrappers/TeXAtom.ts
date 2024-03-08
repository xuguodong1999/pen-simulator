import {JSONConstructor, JSONWrapper} from '../Wrapper';
import {CommonTeXAtomMixin} from 'mathjax-full/js/output/common/Wrappers/TeXAtom';
import {TeXAtom} from 'mathjax-full/js/core/MmlTree/MmlNodes/TeXAtom';
import {TEXCLASS, TEXCLASSNAMES} from 'mathjax-full/js/core/MmlTree/MmlNode';

/*****************************************************************/
/**
 * The JSONTeXAtom wrapper for the TeXAtom object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class JSONTeXAtom<N, T, D> extends CommonTeXAtomMixin<JSONConstructor<any, any, any>>(JSONWrapper) {

    /**
     * The TeXAtom wrapper
     */
    public static kind = TeXAtom.prototype.kind;

    /**
     * @override
     */
    public toJSON(parent: N) {
        super.toJSON(parent);
        this.adaptor.setAttribute(this.element, 'data-mjx-texclass', TEXCLASSNAMES[this.node.texClass]);
        //
        // Center VCENTER atoms vertically
        //
        if (this.node.texClass === TEXCLASS.VCENTER) {
            const bbox = this.childNodes[0].getBBox();  // get unmodified bbox of children
            const {h, d} = bbox;
            const a = this.font.params.axis_height;
            const dh = ((h + d) / 2 + a) - h;  // new height minus old height
            const translate = 'translate(0 ' + this.fixed(dh) + ')';
            this.adaptor.setAttribute(this.element, 'transform', translate);
        }
    }

}
