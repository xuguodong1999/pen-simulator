import {JSONConstructor, JSONWrapper} from '../Wrapper';
import {CommonSemanticsMixin} from 'mathjax-full/js/output/common/Wrappers/semantics';
import {BBox} from 'mathjax-full/js/util/BBox';
import {MmlAnnotation, MmlAnnotationXML, MmlSemantics} from 'mathjax-full/js/core/MmlTree/MmlNodes/semantics';
import {XMLNode} from 'mathjax-full/js/core/MmlTree/MmlNode';
import {StyleList} from 'mathjax-full/js/util/StyleList';

/*****************************************************************/
/**
 * The JSONsemantics wrapper for the MmlSemantics object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class JSONsemantics<N, T, D> extends CommonSemanticsMixin<JSONConstructor<any, any, any>>(JSONWrapper) {

    /**
     * The semantics wrapper
     */
    public static kind = MmlSemantics.prototype.kind;

    /**
     * @override
     */
    public toJSON(parent: N) {
        const svg = this.standardJSONnode(parent);
        if (this.childNodes.length) {
            this.childNodes[0].toJSON(svg);
        }
    }

}


/*****************************************************************/
/**
 * The JSONannotation wrapper for the MmlJSON object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
export class JSONannotation<N, T, D> extends JSONWrapper<N, T, D> {

    /**
     * The annotation wrapper
     */
    public static kind = MmlAnnotation.prototype.kind;

    /**
     * @override
     */
    public toJSON(parent: N) {
        // FIXME:  output as plain text
        super.toJSON(parent);
    }

    /**
     * @override
     */
    public computeBBox() {
        // FIXME:  compute using the DOM, if possible
        return this.bbox;
    }

}

/*****************************************************************/
/**
 * The JSONannotationXML wrapper for the MmlAnnotationXML object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
export class JSONannotationXML<N, T, D> extends JSONWrapper<N, T, D> {

    /**
     * The annotation-xml wrapper
     */
    public static kind = MmlAnnotationXML.prototype.kind;

    /**
     * @override
     */
    public static styles: StyleList = {
        'foreignObject[data-mjx-xml]': {
            'font-family': 'initial',
            'line-height': 'normal',
            overflow: 'visible'
        }
    };

}

/*****************************************************************/
/**
 * The JSONxml wrapper for the XMLNode object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
export class JSONxml<N, T, D> extends JSONWrapper<N, T, D> {

    /**
     * The XMLNode wrapper
     */
    public static kind = XMLNode.prototype.kind;

    /**
     * Don't include inline-block CSS for this element
     */
    public static autoStyle = false;

    /**
     * @override
     */
    public toJSON(parent: N) {
        const xml = this.adaptor.clone((this.node as XMLNode).getXML() as N);
        const em = this.jax.math.metrics.em * this.jax.math.metrics.scale;
        const scale = this.fixed(1 / em);
        const {w, h, d} = this.getBBox();
        this.element = this.adaptor.append(parent, this.svg('foreignObject', {
            'data-mjx-xml': true,
            y: this.jax.fixed(-h * em) + 'px',
            width: this.jax.fixed(w * em) + 'px',
            height: this.jax.fixed((h + d) * em) + 'px',
            transform: `scale(${scale}) matrix(1 0 0 -1 0 0)`
        }, [xml]));
    }

    /**
     * @override
     */
    public computeBBox(bbox: BBox, _recompute: boolean = false) {
        const {w, h, d} = this.jax.measureXMLnode((this.node as XMLNode).getXML() as N);
        bbox.w = w;
        bbox.h = h;
        bbox.d = d;
    }

    /**
     * @override
     */
    protected getStyles() {
    }

    /**
     * @override
     */
    protected getScale() {
    }

    /**
     * @override
     */
    protected getVariant() {
    }
}
