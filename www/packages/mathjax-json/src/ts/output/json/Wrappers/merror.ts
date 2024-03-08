import {JSONWrapper} from '../Wrapper';
import {MmlMerror} from 'mathjax-full/js/core/MmlTree/MmlNodes/merror';
import {StyleList} from 'mathjax-full/js/util/StyleList';

/*****************************************************************/
/**
 *  The JSONmerror wrapper for the MmlMerror object
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
export class JSONmerror<N, T, D> extends JSONWrapper<N, T, D> {

    /**
     * The merror wrapper
     */
    public static kind = MmlMerror.prototype.kind;

    /**
     * @override
     */
    public static styles: StyleList = {
        'g[data-mml-node="merror"] > g': {
            fill: 'red',
            stroke: 'red'
        },
        'g[data-mml-node="merror"] > rect[data-background]': {
            fill: 'yellow',
            stroke: 'none'
        }
    };

    /**
     * @override
     */
    public toJSON(parent: N) {
        const svg = this.standardJSONnode(parent);
        const {h, d, w} = this.getBBox();
        this.adaptor.append(this.element, this.svg('rect', {
            'data-background': true,
            width: this.fixed(w), height: this.fixed(h + d), y: this.fixed(-d)
        }));
        const title = this.node.attributes.get('title') as string;
        if (title) {
            this.adaptor.append(this.element, this.svg('title', {}, [this.adaptor.text(title)]));
        }
        this.addChildren(svg);
    }

}
