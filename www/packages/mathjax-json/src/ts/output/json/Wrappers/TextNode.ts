import {TextNode} from 'mathjax-full/js/core/MmlTree/MmlNode';
import {JSONConstructor, JSONWrapper} from '../Wrapper';
import {CommonTextNodeMixin} from 'mathjax-full/js/output/common/Wrappers/TextNode';
import {StyleList} from 'mathjax-full/js/util/StyleList';

/*****************************************************************/
/**
 *  The JSONTextNode wrapper for the TextNode object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class JSONTextNode<N, T, D> extends CommonTextNodeMixin<JSONConstructor<any, any, any>>(JSONWrapper) {

    /**
     * The TextNode wrapper
     */
    public static kind = TextNode.prototype.kind;

    /**
     * @override
     */
    public static styles: StyleList = {
        'mjx-container[jax="JSON"] path[data-c], mjx-container[jax="JSON"] use[data-c]': {
            'stroke-width': 3
        }
    };

    /**
     * @override
     */
    public toJSON(parent: N) {
        const text = (this.node as TextNode).getText();
        const variant = this.parent.variant;
        if (text.length === 0) return;
        if (variant === '-explicitFont') {
            this.element = this.adaptor.append(parent, this.jax.unknownText(text, variant));
        } else {
            const chars = this.remappedText(text, variant);
            if (this.parent.childNodes.length > 1) {
                parent = this.element = this.adaptor.append(parent, this.svg('g', {'data-mml-node': 'text'}));
            }
            let x = 0;
            for (const n of chars) {
                x += this.placeChar(n, x, 0, parent, variant);
            }
        }
    }

}
