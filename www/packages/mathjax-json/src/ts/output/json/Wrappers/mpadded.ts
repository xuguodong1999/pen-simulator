import {JSONConstructor, JSONWrapper} from '../Wrapper';
import {CommonMpaddedMixin} from 'mathjax-full/js/output/common/Wrappers/mpadded';
import {MmlMpadded} from 'mathjax-full/js/core/MmlTree/MmlNodes/mpadded';

/*****************************************************************/
/**
 * The JSONmpadded wrapper for the MmlMpadded object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
// @ts-ignore
export class JSONmpadded<N, T, D> extends CommonMpaddedMixin<JSONConstructor<any, any, any>>(JSONWrapper) {

    /**
     * The mpadded wrapper
     */
    public static kind = MmlMpadded.prototype.kind;

    /**
     * @override
     */
    public toJSON(parent: N) {
        let svg = this.standardJSONnode(parent);
        const [, , , , , dw, x, y, dx] = this.getDimens();
        const align = (this.node.attributes.get('data-align') as string) || 'left';
        const X = x + dx - (dw < 0 && align !== 'left' ? align === 'center' ? dw / 2 : dw : 0);
        //
        // If there is a horizontal or vertical shift,
        //   use relative positioning to move the contents
        //
        if (X || y) {
            svg = this.adaptor.append(svg, this.svg('g'));
            this.place(X, y, svg);
        }
        this.addChildren(svg);
    }

}
