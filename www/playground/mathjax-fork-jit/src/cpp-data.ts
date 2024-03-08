function groupSimilarChars() {
    const block = String.raw`
-_¯ˉ–—―‾−￣
'‘’
${'`'}ˋ‵
´ˊ′,
.·˙•∙⋅
"“”
(⟮
)⟯
*⋆
/⁄∕⧸
${'\\'}∖
^ˆΛ⋀⋏
ˇ˘
∂𝜕𝝏
∆△▵Δ
∇𝛁▽▿
∋∍
+†∔
<≺〈〈⟨
=‗
>≻〉〉⟩
~˜∼
∝α
⋯…
AΑ
b6♭
BΒ
Cc∁
dⅆ
eⅇ
EΕ
hℎ
HℍΗ
1lⅠ∣⎜⎟⎢⎥⎪⏐
IΙ
Jȷ
Ⅱ‖
iⅰⅈ
kKκΚ
Σ∑
7
MΜ
n∏∩⋂
NΝℕ
OoΟο◯
PΡpℙρ
q
9
Qℚ
Rℝ
Ss
TΤ⊤⊺
Uu∐∪⋃
VvνⅤⅴ∨⋁
Wwω𝝕𝜛
Xx×ϰΧⅩⅹ𝝒⨯
YγΥ⋎
y𝑦
Z2zΖℤ
𝝐∈ϵ
⊖⊝
⊕⨁
⨂⊗
⨀⊙
⟶→
𝜙ϕΦ`;
    const lines = block.split('\n').sort((a: string, b: string) => {
        return Array.from(a).sort().join('') < Array.from(b).sort().join('') ? -1 : 1;
    });
    let result = '';
    const getChar = (char: string) => {
        if (char == '"' || char == '\\') {
            return `\\${char}`;
        }
        return char;
    }
    for (const line of lines) {
        const chars = Array.from(line).sort();
        if (chars.length === 0) {
            continue;
        }
        result += `{"${getChar(chars[0])}",{`;
        for (const char of chars) {
            result += `"${getChar(char)}",`;
        }
        result += '}},\n';
    }
    console.log(result);
}

export {groupSimilarChars};