import {JSONConstructor, JSONWrapper} from '../Wrapper';
import {CommonMglyphMixin} from 'mathjax-full/js/output/common/Wrappers/mglyph';
import {MmlMglyph} from 'mathjax-full/js/core/MmlTree/MmlNodes/mglyph';
import {JSONTextNode} from './TextNode';
import {OptionList} from 'mathjax-full/js/util/Options';

/*****************************************************************/
/**
 * The JSONmglyph wrapper for the MmlMglyph object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class JSONmglyph<N, T, D> extends CommonMglyphMixin<JSONConstructor<any, any, any>>(JSONWrapper) {

    /**
     * The mglyph wrapper
     */
    public static kind = MmlMglyph.prototype.kind;

    /**
     * @override
     */
    public toJSON(parent: N) {
        const svg = this.standardJSONnode(parent);
        if (this.charWrapper) {
            (this.charWrapper as JSONTextNode<N, T, D>).toJSON(svg);
            return;
        }
        const {src, alt} = this.node.attributes.getList('src', 'alt');
        const h = this.fixed(this.height);
        const w = this.fixed(this.width);
        const y = this.fixed(this.height + (this.valign || 0));
        const properties: OptionList = {
            width: w, height: h,
            transform: 'translate(0 ' + y + ') matrix(1 0 0 -1 0 0)',
            preserveAspectRatio: 'none',
            'aria-label': alt,
            href: src
        };
        const img = this.svg('image', properties);
        this.adaptor.append(svg, img);
    }

}
