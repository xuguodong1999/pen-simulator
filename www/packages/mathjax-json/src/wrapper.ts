import {LiteElement, LiteNode} from 'mathjax-full/js/adaptors/lite/Element';
import {LiteComment, LiteText} from 'mathjax-full/js/adaptors/lite/Text';
import {LiteAdaptor, liteAdaptor} from 'mathjax-full/js/adaptors/liteAdaptor';
import {MathDocument} from 'mathjax-full/js/core/MathDocument';
import {RegisterHTMLHandler} from 'mathjax-full/js/handlers/html';
import {TeX} from 'mathjax-full/js/input/tex';
import {AllPackages} from 'mathjax-full/js/input/tex/AllPackages';
import {mathjax} from 'mathjax-full/js/mathjax';
import {JSONOutput} from './ts/output/json';
import {SVG} from 'mathjax-full/js/output/svg';
import {JSONFontData} from "./ts/output/json/FontData";
import {entities} from "mathjax-full/js/util/Entities";
import {delimiters} from "mathjax-full/js/output/common/fonts/tex/delimiters";

class Wrapper {
    static adaptor: LiteAdaptor;
    static jsonDocument: MathDocument<any, any, any>;
    static svgDocument: MathDocument<any, any, any>;
    static {
        Wrapper.adaptor = liteAdaptor();
        RegisterHTMLHandler(Wrapper.adaptor);
        Wrapper.refresh_document();
    }

    public static texToSvg(math: string): string {
        Wrapper.refresh_document();
        const mathjax_options = {};
        const {adaptor, svgDocument} = Wrapper;
        const node = svgDocument.convert(math, mathjax_options);
        return adaptor.innerHTML(node);
    }

    public static texToJSON(math: string): string {
        Wrapper.refresh_document();
        const mathjax_options = {};
        const {jsonDocument} = Wrapper;
        const node = jsonDocument.convert(math, mathjax_options);
        return Wrapper.nodeGraphToJSON(math, node);
    }

    private static nodeGraphToJSON(tex: string, n: LiteElement): string {
        const {adaptor} = Wrapper;
        const nodes = new Map<string, Object>();
        const edges = [];
        let globalNodeIndex = 0;
        const nodeToIndexMap = new Map<LiteNode, number>();
        const visited = new Set<LiteNode>();

        const dataClassSummary = new Set<string>();
        const mmlNodeSummary = new Set<string>();
        const valueSummary = new Set<string>();
        const kindSummary = new Set<string>();
        const attrSummary = new Set<string>();
        const edgeRelationSummary = new Set<string>();

        Wrapper.traverse(n, x => {
            const kind = adaptor.kind(x);
            let attributes = undefined;
            let styles = undefined;
            let value = undefined;
            kindSummary.add(kind);
            if (kind === '#text') {
                value = (x as LiteText).value;
            } else if (kind === '#comment') {
                value = (x as LiteComment).value;
            } else {
                x = x as LiteElement
                attributes = x.attributes;
                styles = x.styles ? x.styles : undefined;
                value = x.attributes['data-mml-text'];
                if (kind === 'text') {
                    let root = x.parent;
                    while (root && !value) {
                        value = (root as LiteElement).attributes?.['data-mml-text'];
                        root = root.parent;
                    }
                }
                const maybeDataClass = x.attributes['data-c'];
                const maybeMmlNode = x.attributes['data-mml-node'];
                for (const k in x.attributes) {
                    attrSummary.add(k);
                }
                if (maybeDataClass) {
                    dataClassSummary.add(maybeDataClass);
                }
                if (maybeMmlNode) {
                    mmlNodeSummary.add(maybeMmlNode);
                }
            }
            if (value) {
                valueSummary.add(value);
            }

            const index = globalNodeIndex++;
            nodeToIndexMap.set(x, index);
            let nodeLabel = `[${index}]-[${kind}]`;
            if (value) {
                nodeLabel += `-[${value}]`;
            }
            nodes[index.toString()] = {
                label: nodeLabel,
                metadata: Object.assign({
                    kind,
                    attributes,
                    value,
                    // path_box,
                }, styles),
            };
            if (x.parent) {
                const relation = `${x.parent?.kind} -> ${x.kind}`;
                const edge = {
                    source: nodeToIndexMap.get(x.parent).toString(),
                    target: index.toString(),
                    relation: relation,
                    directed: true,
                    label: '',
                    metadata: {},
                };
                edgeRelationSummary.add(relation);
                edges.push(edge);
            }
        }, visited);
        /**
         * https://github.com/jsongraph/json-graph-specification
         */
        return JSON.stringify({
            graph: {
                directed: true,
                type: "annotated mathjax json output",
                label: tex,
                metadata: {
                    node_mmls: Array.from(mmlNodeSummary),
                    node_data_classes: Array.from(dataClassSummary),
                    node_values: Array.from(valueSummary),
                    node_kinds: Array.from(kindSummary),
                    node_attrs: Array.from(attrSummary),
                    edge_relations: Array.from(edgeRelationSummary),
                },
                nodes,
                edges,
            },
        });
    }

    private static traverse(
        node: LiteElement,
        callback: (node: LiteNode) => void,
        visited: Set<LiteNode>,
    ) {
        const {adaptor} = Wrapper;
        if (!visited.has(node)) {
            callback(node);
            visited.add(node);
        }
        adaptor.childNodes(node).map(x => {
            const kind = adaptor.kind(x);
            switch (kind) {
                case '#text':
                case '#comment':
                    break;
                default:
                    x = x as LiteElement
                    Wrapper.traverse(x, callback, visited);
            }
        });
    }

    private static refresh_document() {
        const tex = new TeX({packages: AllPackages});
        const json = new JSONOutput({fontCache: 'local'});
        const svg = new SVG({fontCache: 'local'});
        Wrapper.jsonDocument = mathjax.document('', {InputJax: tex, OutputJax: json,});
        Wrapper.svgDocument = mathjax.document('', {InputJax: tex, OutputJax: svg,});
    }
}

export {
    Wrapper,
    JSONOutput,
    JSONFontData,
    entities,
    delimiters,
    AllPackages,
    liteAdaptor,
    mathjax,
    RegisterHTMLHandler,
    SVG,
    TeX,
};
