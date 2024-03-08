import {JSONOutput} from '../json';
import {CommonWrapperFactory} from 'mathjax-full/js/output/common/WrapperFactory';
import {JSONWrapper, JSONWrapperClass} from './Wrapper';
import {JSONWrappers} from './Wrappers';
import {JSONCharOptions, JSONDelimiterData, JSONFontData} from './FontData';

/*****************************************************************/

/*
 *  The JSONWrapperFactory class for creating CHTMLWrapper nodes
 *
 * @template N  The HTMLElement node class
 * @template T  The Text node class
 * @template D  The Document class
 */
export class JSONWrapperFactory<N, T, D> extends CommonWrapperFactory<
    JSONOutput<N, T, D>,
    JSONWrapper<N, T, D>,
    JSONWrapperClass,
    JSONCharOptions,
    JSONDelimiterData,
    JSONFontData
> {

    /**
     * The default list of wrapper nodes this factory can create
     */
    public static defaultNodes = JSONWrappers;

    /**
     * The CHTML output jax associated with this factory
     */
    public jax: JSONOutput<N, T, D> = null;

}
