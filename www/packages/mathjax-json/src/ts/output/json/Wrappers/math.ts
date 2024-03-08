import {JSONConstructor, JSONWrapper} from '../Wrapper';
import {CommonMathMixin} from 'mathjax-full/js/output/common/Wrappers/math';
import {MmlMath} from 'mathjax-full/js/core/MmlTree/MmlNodes/math';
import {StyleList} from 'mathjax-full/js/util/StyleList';
import {BBox} from 'mathjax-full/js/util/BBox';

/*****************************************************************/
/**
 * The JSONmath wrapper for the MmlMath object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class JSONmath<N, T, D> extends CommonMathMixin<JSONConstructor<any, any, any>>(JSONWrapper) {

    /**
     * The math wrapper
     */
    public static kind = MmlMath.prototype.kind;

    /**
     * @overreide
     */
    public static styles: StyleList = {
        'mjx-container[jax="JSON"][display="true"]': {
            display: 'block',
            'text-align': 'center',
            margin: '1em 0'
        },
        'mjx-container[jax="JSON"][display="true"][width="full"]': {
            display: 'flex'
        },
        'mjx-container[jax="JSON"][justify="left"]': {
            'text-align': 'left'
        },
        'mjx-container[jax="JSON"][justify="right"]': {
            'text-align': 'right'
        }
    };

    /**
     * @override
     */
    public toJSON(parent: N) {
        super.toJSON(parent);
        const adaptor = this.adaptor;
        const display = (this.node.attributes.get('display') === 'block');
        if (display) {
            adaptor.setAttribute(this.jax.container, 'display', 'true');
            this.handleDisplay();
        }
        if (this.jax.document.options.internalSpeechTitles) {
            this.handleSpeech();
        }
    }

    /**
     * @override
     */
    public setChildPWidths(recompute: boolean, w: number = null, _clear: boolean = true) {
        return super.setChildPWidths(recompute,
            this.parent ? w : this.metrics.containerWidth / this.jax.pxPerEm,
            false);
    }

    /**
     * Set the justification, and get the minwidth and shift needed
     * for the displayed equation.
     */
    protected handleDisplay() {
        const [align, shift] = this.getAlignShift();
        if (align !== 'center') {
            this.adaptor.setAttribute(this.jax.container, 'justify', align);
        }
        if (this.bbox.pwidth === BBox.fullWidth) {
            this.adaptor.setAttribute(this.jax.container, 'width', 'full');
            if (this.jax.table) {
                let {L, w, R} = this.jax.table.getOuterBBox();
                if (align === 'right') {
                    R = Math.max(R || -shift, -shift);
                } else if (align === 'left') {
                    L = Math.max(L || shift, shift);
                } else if (align === 'center') {
                    w += 2 * Math.abs(shift);
                }
                this.jax.minwidth = Math.max(0, L + w + R);
            }
        } else {
            this.jax.shift = shift;
        }
    }

    /**
     * Handle adding speech to the top-level node, if any.
     */
    protected handleSpeech() {
        const adaptor = this.adaptor;
        const attributes = this.node.attributes;
        const speech = (attributes.get('aria-label') || attributes.get('data-semantic-speech')) as string;
        if (speech) {
            const id = this.getTitleID();
            const label = this.svg('title', {id}, [this.text(speech)]);
            adaptor.insert(label, adaptor.firstChild(this.element));
            adaptor.setAttribute(this.element, 'aria-labeledby', id);
            adaptor.removeAttribute(this.element, 'aria-label');
            for (const child of this.childNodes[0].childNodes) {
                adaptor.setAttribute(child.element, 'aria-hidden', 'true');
            }
        }
    }

    /**
     * @return {string}  A unique ID to use for aria-labeledby title elements
     */
    protected getTitleID(): string {
        return 'mjx-svg-title-' + String(this.jax.options.titleID++);
    }

}
