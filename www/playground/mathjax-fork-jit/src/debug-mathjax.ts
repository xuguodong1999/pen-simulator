import fs from "fs";
import path from "path";
import os from "os";
import {Wrapper} from "@xgd/mathjax-fork";

function writeDebugFile() {
    const tex1 = String.raw`
\displaylines{
😅+😅😅=😅😅😅 \\
😅\times😅😅=😅😅
}`;
    const tex2 = String.raw`
\begin{pmatrix}
 你好 & c_1 & c_1^2 & \cdots & c_1^n \\
 😅 & c_2 & c_2^2 & \cdots & c_2^n \\
 \vdots  & \vdots& \vdots & \ddots & \vdots \\
 1 & c_m & c_m^2 & \cdots & c_m^n \\
\end{pmatrix}
`;
    const tex3 = String.raw`
\begin{pmatrix}
 你好 & c_1 & c_1^2 & \cdots & c_1^n \\
 😅 & c_2 & c_2^2 & \cdots & c_2^n \\
 \vdots  & \vdots& \vdots & \ddots & \vdots \\
 1 & c_m & c_m^2 & \cdots & c_m^n \\
 \sum_{n=1}^{\infty} \frac{1}{3^n \cdot n} & \sum_{n=1}^{\infty} \frac{1}{4^n \cdot n} & 😅 & 😅 & 😅 \\
 \frac{2}{5} \sum_{k=0}^{\infty} \frac{1}{25^k(2 k+1)} & \frac{2}{7} \sum_{k=0}^{\infty} \frac{1}{49^k(2 k+1)} & 😅 & 😅 & 😅 \\
 7 \sum_{n=1}^{\infty} \frac{1}{16^n \cdot n} & 5 \sum_{n=1}^{\infty} \frac{1}{25^n \cdot n} &  \sum_{n=1}^{\infty} \frac{1}{81^n \cdot n} & 😅 & 😅
\end{pmatrix}    
`;
    const tex4 = String.raw`
\begin{array} {rl}{\delta \over \delta S^{\mu \nu}(z(\tau))} &        E_{\rm conf}(\bar z - \bar z^\prime,         \dot{\bar z}, \dot{\bar z} ^\prime) = \nonumber \\= & \sigma \delta (\tau - \bar \tau)       \delta(z_0 - \bar z_0^\prime)       \epsilon(\dot z_0) \dot{\bar z} _0^\prime       {(\bar z^\mu - \bar z^{\prime \mu}) \dot{\bar z} ^\nu -      (\bar z ^\nu - \bar z^{\prime \nu})\dot {\bar z} ^\mu       \over | \bar {\vec z} - \bar{\vec z}^\prime|       \sqrt {\dot {\bar z_0}^2 -       \dot {\bar {\vec z}}_{\rm T}^2}} - \nonumber \\ & \qquad \qquad - \sigma \delta (\tau - \bar \tau ^\prime)        \delta(\bar z_0 -        \bar z_0^\prime) \epsilon(\dot z_0^\prime) \dot{\bar z} _0       {(\bar z^\mu - \bar z^{\prime \mu}) \dot{\bar z} ^{\prime \nu} -      (\bar z ^\nu - \bar z^{\prime \nu})\dot {\bar z} ^{\prime \mu}       \over | \bar {\vec z} - \bar{\vec z}^\prime|       \sqrt {\dot {\bar z_0}^{\prime 2} -       \dot {\bar {\vec z}}_{\rm T}^{\prime 2}}} \end{array}
`;
    const tex5 = String.raw`
\label{e16}q_{\!_J}^2 \ = \ c T_{\!_J}
`;
    const tex = tex4;

    fs.writeFileSync(path.resolve(os.homedir(), 'Desktop/TEMP.json'), Wrapper.texToJSON(tex));
    fs.writeFileSync(path.resolve(os.homedir(), 'Desktop/TEMP.svg'), Wrapper.texToSvg(tex));
}
export {writeDebugFile};