import {JSONmenclose} from './Wrappers/menclose';
import * as Notation from 'mathjax-full/js/output/common/Notation';

export * from 'mathjax-full/js/output/common/Notation';

/*******************************************************************/

/**
 * Shorthand for JSONmenclose
 */
export type Menclose = JSONmenclose<any, any, any>;


/*
 * Shorthands for common types
 */
export type RENDERER<N, T, D> = Notation.Renderer<JSONmenclose<N, T, D>, N>;
export type DEFPAIR<N, T, D> = Notation.DefPair<JSONmenclose<N, T, D>, N>;

/**
 * The kinds of lines that can be drawn
 */
export type LineName = Notation.Side | ('vertical' | 'horizontal' | 'up' | 'down');

/**
 * [x1,y1, x2,y2] endpoints for a line
 */
export type LineData = [number, number, number, number];

/**
 * Functions for computing the line data for each type of line
 */
export const computeLineData = {
    top: (h, _d, w, t) => [0, h - t, w, h - t],
    right: (h, d, w, t) => [w - t, -d, w - t, h],
    bottom: (_h, d, w, t) => [0, t - d, w, t - d],
    left: (h, d, _w, t) => [t, -d, t, h],
    vertical: (h, d, w, _t) => [w / 2, h, w / 2, -d],
    horizontal: (h, d, w, _t) => [0, (h - d) / 2, w, (h - d) / 2],
    up: (h, d, w, t) => [t, t - d, w - t, h - t],
    down: (h, d, w, t) => [t, h - t, w - t, t - d]
} as { [kind: string]: (h: number, d: number, w: number, t: number) => LineData };

/**
 * The data for a given line as two endpoints: [x1, y1, x2, y1]
 *
 * @param {Menclose} node   The node whose line is to be drawn
 * @param {LineName} kind   The type of line to draw for the node
 * @param {string} offset   The offset direction, if any
 * @return {LineData}       The coordinates of the two endpoints
 */
export const lineData = function (node: Menclose, kind: LineName, offset: string = ''): LineData {
    const {h, d, w} = node.getBBox();
    const t = node.thickness / 2;
    return lineOffset(computeLineData[kind](h, d, w, t), node, offset);
};

/**
 * Recenter the line data for vertical and horizontal lines
 *
 * @param {LineData} data   The line endpoints to adjust
 * @param {Menclose} node   The menclose node
 * @param {string} offset   The direction to offset
 */
export const lineOffset = function (data: LineData, node: Menclose, offset: string): LineData {
    if (offset) {
        const d = node.getOffset(offset);
        if (d) {
            if (offset === 'X') {
                data[0] -= d;
                data[2] -= d;
            } else {
                data[1] -= d;
                data[3] -= d;
            }
        }
    }
    return data;
};


/*******************************************************************/

/**
 * @param {LineName} line  The name of the line to create
 * @return {RENDERER}      The renderer function for the given line
 */
export const RenderLine = function <N, T, D>(line: LineName, offset: string = ''): RENDERER<N, T, D> {
    return ((node, _child) => {
        const L = node.line(lineData(node, line, offset));
        node.adaptor.append(node.element, L);
    });
};

/*******************************************************************/

/**
 * @param {Notation.Side} side   The kind of line (side, diagonal, etc.)
 * @return {DEFPAIR}      The notation definition for the notation having a line on the given side
 */
export const Border = function <N, T, D>(side: Notation.Side): DEFPAIR<N, T, D> {
    return Notation.CommonBorder<JSONmenclose<N, T, D>, N>((node, _child) => {
        node.adaptor.append(node.element, node.line(lineData(node, side)));
    })(side);
};


/**
 * @param {string} name    The name of the notation to define
 * @param {Notation.Side} side1   The first side to get a border
 * @param {Notation.Side} side2   The second side to get a border
 * @return {DEFPAIR}       The notation definition for the notation having lines on two sides
 */
export const Border2 = function <N, T, D>(name: string, side1: Notation.Side, side2: Notation.Side): DEFPAIR<N, T, D> {
    return Notation.CommonBorder2<JSONmenclose<N, T, D>, N>((node, _child) => {
        node.adaptor.append(node.element, node.line(lineData(node, side1)));
        node.adaptor.append(node.element, node.line(lineData(node, side2)));
    })(name, side1, side2);
};

/*******************************************************************/

/**
 * @param {LineName} name  The name of the diagonal strike to define
 * @return {DEFPAIR}       The notation definition for the diagonal strike
 */
export const DiagonalStrike = function <N, T, D>(name: LineName): DEFPAIR<N, T, D> {
    return Notation.CommonDiagonalStrike<JSONmenclose<N, T, D>, N>((_cname: string) => (node, _child) => {
        node.adaptor.append(node.element, node.line(lineData(node, name)));
    })(name);
};

/*******************************************************************/

/**
 * @param {string} name   The name of the diagonal arrow to define
 * @return {DEFPAIR}      The notation definition for the diagonal arrow
 */
export const DiagonalArrow = function <N, T, D>(name: string): DEFPAIR<N, T, D> {
    return Notation.CommonDiagonalArrow<JSONmenclose<N, T, D>, N>((node, arrow) => {
        node.adaptor.append(node.element, arrow);
    })(name);
};

/**
 * @param {string} name   The name of the horizontal or vertical arrow to define
 * @return {DEFPAIR}      The notation definition for the arrow
 */
export const Arrow = function <N, T, D>(name: string): DEFPAIR<N, T, D> {
    return Notation.CommonArrow<JSONmenclose<N, T, D>, N>((node, arrow) => {
        node.adaptor.append(node.element, arrow);
    })(name);
};

/*******************************************************************/
