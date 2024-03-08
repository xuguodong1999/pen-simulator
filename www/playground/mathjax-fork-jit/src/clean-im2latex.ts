import fs from "fs";
import path from "path";
import os from "os";
import {Wrapper} from "@xgd/mathjax-json";

function cleanIm2Latex() {
    const root = path.resolve(os.homedir(), 'datasets/im2latex/raw');
    const lines = fs.readFileSync(
        path.join(root, 'im2latex_formulas.lst'),
        {encoding: 'utf8'},
    ).split("\n");
    let total = 0;
    let error = 0;
    let i = 0;
    const validItems = [];
    const invalidItems = [];
    while (i < lines.length) {
        const originStart = i;
        let latex = lines[i];
        let ii = i + 1;
        while (ii < lines.length) {
            const start = ii;
            while (lines[ii].startsWith(' ')) {
                latex += lines[ii];
                ii += 1;
            }
            while (latex.endsWith(String.raw`\\`)) {
                latex += lines[ii];
                ii += 1;
            }
            if (ii === start) {
                break;
            }
        }
        const svg = Wrapper.texToSvg(latex);
        total += 1;
        if (-1 !== svg.indexOf('red') || -1 !== svg.indexOf('black')) {
            error += 1;
            invalidItems.push(latex);
            i = originStart + 1;
        } else {
            i = ii;
            validItems.push(latex);
        }
        if (i % 1000 === 0) {
            console.log(`${total}: error rate = ${error / total}`);
        }
    }
    console.log(`${total}: error rate = ${error / total}`);
    // fs.writeFileSync(path.join(root, 'im2latex_formulas.lst.json'), JSON.stringify({
    //     "valid": validItems,
    //     "invalid": invalidItems,
    // }));
    // raw: 0.015242718446601942
    // line append: 0.015223127555310696
}

export {cleanIm2Latex};